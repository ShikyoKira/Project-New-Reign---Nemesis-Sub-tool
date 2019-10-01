#include <boost\thread.hpp>
#include "hkbfootikcontrolsmodifier.h"
#include "boolstring.h"
#include "highestscore.h"
#include "src\hkx\modifier\base\hkbfootikgains.h"

using namespace std;

namespace footikcontrolsmodifier
{
	string key = "af";
	string classname = "hkbFootIkControlsModifier";
	string signature = "0xe5b6f544";
}

struct leg
{
	bool proxy = true;

	string groundPosition;
	eventproperty ungroundedEvent;
	double verticalError;
	bool hitSomething;
	bool isPlantedMS;
};

struct footik
{
	string variableBindingSet;
	int userData;
	string name;
	bool enable;

	// controlData, gains
	gain gains;

	vector<leg> legs;

	string errorOutTranslation;
	string alignWithGroundRotation;
};

void inputFootIK(vecstr& input, leg& leg);
void inputDoubleFootIK(vecstr& output, vecstr& storeline, double oriD, double newD, string key, bool& IsEdited, bool& open, bool first = false);
void footIKInfoProcess(string line, footik& footIK);
bool matchLegsScoring(vector<leg>& ori, vector<leg>& edit, string id);

hkbfootikcontrolsmodifier::hkbfootikcontrolsmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + footikcontrolsmodifier::key + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id);
		}
		else
		{
			nonCompare(filepath, id);
		}
	}
	else if (!Error)
	{
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare);

		if (compare)
		{
			if (statusChange)
			{
				Dummy(dummyID);
			}

			if (IsForeign[id])
			{
				address = preaddress;
			}
			else if (!statusChange)
			{
				IsNegated = true;
			}
		}
		else
		{
			IsNegated = true;
		}
	}
	else
	{
		return;
	}
}

void hkbfootikcontrolsmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkControlsModifier(ID: " << id
			<< ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbFootIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikcontrolsmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkControlsModifier(ID: " << id
			<< ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						int tempint = line.find(variablebindingset);
						variablebindingset = exchangeID[variablebindingset];
						line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
					}

					parent[variablebindingset] = id;
					referencingIDs[variablebindingset].push_back(id);
				}
				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

					if (payload.back() != "null")
					{
						if (!exchangeID[payload.back()].empty())
						{
							int tempint = line.find(payload.back());
							payload.back() = exchangeID[payload.back()];
							line.replace(tempint, line.find("</hkparam>") - tempint, payload.back());
						}

						parent[payload.back()] = id;
						referencingIDs[payload.back()].push_back(id);
					}
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbFootIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbFootIkControlsModifier(newID: " << id << ") with hkbFootIkControlsModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
				parent[payload[i]] = tempid;
			}
		}

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbFootIkControlsModifier(newID: " << id << ") with hkbFootIkControlsModifier(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikcontrolsmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbFootIkControlsModifier(ID: " << id
			<< ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						variablebindingset = exchangeID[variablebindingset];
					}

					parent[variablebindingset] = id;
				}
				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

					if (payload.back() != "null")
					{
						if (!exchangeID[payload.back()].empty())
						{
							payload.back() = exchangeID[payload.back()];
						}

						parent[payload.back()] = id;
					}
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbFootIkControlsModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbFootIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

int hkbfootikcontrolsmodifier::GetPayloadCount()
{
	return int(payload.size());
}

string hkbfootikcontrolsmodifier::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbfootikcontrolsmodifier::IsPayloadNull(int child)
{
	if (payload[child].find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbfootikcontrolsmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbfootikcontrolsmodifier::IsBindingNull()
{
	if (variablebindingset.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbfootikcontrolsmodifier::GetAddress()
{
	return address;
}

bool hkbfootikcontrolsmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbfootikcontrolsmodifier>> hkbfootikcontrolsmodifierList;
safeStringUMap<shared_ptr<hkbfootikcontrolsmodifier>> hkbfootikcontrolsmodifierList_E;

void hkbfootikcontrolsmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbfootikcontrolsmodifierList_E[id] = shared_from_this() : hkbfootikcontrolsmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbfootikcontrolsmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos)
		{
			switch (type)
			{
				case 0:
				{
					string output;

					if (readParam("variableBindingSet", line, output))
					{
						variableBindingSet = (isEdited ? hkbvariablebindingsetList_E : hkbvariablebindingsetList)[output];
						++type;
					}

					break;
				}
				case 1:
				{
					if (readParam("userData", line, userData)) ++type;

					break;
				}
				case 2:
				{
					if (readParam("name", line, name)) ++type;

					break;
				}
				case 3:
				{
					if (readParam("enable", line, enable)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("onOffGain", line, controlData.gains.onOffGain)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("groundAscendingGain", line, controlData.gains.groundAscendingGain)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("groundDescendingGain", line, controlData.gains.groundDescendingGain)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("footPlantedGain", line, controlData.gains.footPlantedGain)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("footRaisedGain", line, controlData.gains.footRaisedGain)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("footUnlockGain", line, controlData.gains.footUnlockGain)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("worldFromModelFeedbackGain", line, controlData.gains.worldFromModelFeedbackGain)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("errorUpDownBias", line, controlData.gains.errorUpDownBias)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("alignWorldFromModelGain", line, controlData.gains.alignWorldFromModelGain)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("hipOrientationGain", line, controlData.gains.hipOrientationGain)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("maxKneeAngleDifference", line, controlData.gains.maxKneeAngleDifference)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("ankleOrientationGain", line, controlData.gains.ankleOrientationGain)) ++type;

					break;
				}
				case 16:
				{
					usize numelement;

					if (readEleParam("legs", line, numelement))
					{
						legs.reserve(numelement);
						++type;
					}

					break;
				}
				case 17:
				{
					string output;

					if (readParam("fwdAxisLS", line, output))
					{
						legs.push_back(output);
						++type;
						break;
					}

					if (readParam("errorOutTranslation", line, errorOutTranslation)) type = 23;

					break;
				}
				case 18:
				{
					if (readParam("id", line, legs.back().ungroundedEvent.id)) ++type;

					break;
				}
				case 19:
				{
					string output;

					if (readParam("payload", line, output))
					{
						legs.back().ungroundedEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 20:
				{
					if (readParam("verticalError", line, legs.back().verticalError)) ++type;

					break;
				}
				case 21:
				{
					if (readParam("hitSomething", line, legs.back().hitSomething)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("isPlantedMS", line, legs.back().isPlantedMS)) type = 17;

					break;
				}
				case 23:
				{
					if (readParam("alignWithGroundRotation", line, alignWithGroundRotation)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << footikcontrolsmodifier::classname +" (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbfootikcontrolsmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + footikcontrolsmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& leg : legs)
	{
		leg.ungroundedEvent.id.connectEventInfo(ID, graphroot);
	}

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			usize tempint = 0;
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			for (auto& leg : legs)
			{
				if (leg.ungroundedEvent.payload) threadedNextNode(leg.ungroundedEvent.payload, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
			}
		}
		else
		{
			// existed
			if (IsOldFunction(filepath, shared_from_this(), address))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbfootikcontrolsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbfootikcontrolsmodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (!compare)
	{
		CrossReferencing(shared_from_this(), address, functionlayer, compare);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
	{
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare);

		// comparing
		if (compare)
		{
			if (ID != newID)
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto foreign_itr = IsForeign.find(ID);

				if (foreign_itr != IsForeign.end()) IsForeign.erase(foreign_itr);

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbfootikcontrolsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbfootikcontrolsmodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			else
			{
				address = preaddress;
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
	{
		if (count(address.begin(), address.end(), '>') == 3)
		{
			if (address.find("(cj", 0) != string::npos || address.find("(i", 0) != string::npos)
			{
				IsOldFunction(filepath, shared_from_this(), address);
			}
		}
	}
}

string hkbfootikcontrolsmodifier::getClassCode()
{
	return footikcontrolsmodifier::key;
}

void hkbfootikcontrolsmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbfootikcontrolsmodifier* ctrpart = static_cast<hkbfootikcontrolsmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, footikcontrolsmodifier::classname, footikcontrolsmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "controlData"));		// 3
	output.push_back(openObject(base));		// 4
	output.push_back(openParam(base, "gains"));		// 5
	output.push_back(openObject(base));		// 6
	paramMatch("onOffGain", controlData.gains.onOffGain, ctrpart->controlData.gains.onOffGain, output, storeline, base, false, open, isEdited);
	paramMatch("groundAscendingGain", controlData.gains.groundAscendingGain, ctrpart->controlData.gains.groundAscendingGain, output, storeline, base, false, open, isEdited);
	paramMatch("groundDescendingGain", controlData.gains.groundDescendingGain, ctrpart->controlData.gains.groundDescendingGain, output, storeline, base, false, open, isEdited);
	paramMatch("footPlantedGain", controlData.gains.footPlantedGain, ctrpart->controlData.gains.footPlantedGain, output, storeline, base, false, open, isEdited);
	paramMatch("footRaisedGain", controlData.gains.footRaisedGain, ctrpart->controlData.gains.footRaisedGain, output, storeline, base, false, open, isEdited);
	paramMatch("footUnlockGain", controlData.gains.footUnlockGain, ctrpart->controlData.gains.footUnlockGain, output, storeline, base, false, open, isEdited);
	paramMatch("worldFromModelFeedbackGain", controlData.gains.worldFromModelFeedbackGain, ctrpart->controlData.gains.worldFromModelFeedbackGain, output, storeline, base, false, open, isEdited);
	paramMatch("errorUpDownBias", controlData.gains.errorUpDownBias, ctrpart->controlData.gains.errorUpDownBias, output, storeline, base, false, open, isEdited);
	paramMatch("alignWorldFromModelGain", controlData.gains.alignWorldFromModelGain, ctrpart->controlData.gains.alignWorldFromModelGain, output, storeline, base, false, open, isEdited);
	paramMatch("hipOrientationGain", controlData.gains.hipOrientationGain, ctrpart->controlData.gains.hipOrientationGain, output, storeline, base, false, open, isEdited);
	paramMatch("maxKneeAngleDifference", controlData.gains.maxKneeAngleDifference, ctrpart->controlData.gains.maxKneeAngleDifference, output, storeline, base, false, open, isEdited);
	paramMatch("ankleOrientationGain", controlData.gains.ankleOrientationGain, ctrpart->controlData.gains.ankleOrientationGain, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 6
	output.push_back(closeParam(base));		// 5
	output.push_back(closeObject(base));		// 4
	output.push_back(closeParam(base));			// 3
	
	usize size = legs.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "legs", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "legs", size));		// 2

	matchScoring(legs, ctrpart->legs, ID);
	size = legs.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (legs[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "fwdAxisLS", ctrpart->legs[i].fwdAxisLS));
				output.push_back(openParam(base, "ungroundedEvent"));		// 4
				output.push_back(openObject(base));		// 5
				output.push_back(autoParam(base, "id", ctrpart->legs[i].ungroundedEvent.id));
				output.push_back(autoParam(base, "payload", ctrpart->legs[i].ungroundedEvent.payload));
				output.push_back(closeObject(base));		// 5
				output.push_back(closeParam(base));		// 4
				output.push_back(autoParam(base, "verticalError", ctrpart->legs[i].verticalError));
				output.push_back(autoParam(base, "hitSomething", ctrpart->legs[i].hitSomething));
				output.push_back(autoParam(base, "isPlantedMS", ctrpart->legs[i].isPlantedMS));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->legs[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->legs[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "fwdAxisLS", legs[i].fwdAxisLS));
				storeline.push_back(openParam(base, "ungroundedEvent"));		// 4
				storeline.push_back(openObject(base));		// 5
				storeline.push_back(autoParam(base, "id", legs[i].ungroundedEvent.id));
				storeline.push_back(autoParam(base, "payload", legs[i].ungroundedEvent.payload));
				storeline.push_back(closeObject(base));		// 5
				storeline.push_back(closeParam(base));		// 4
				storeline.push_back(autoParam(base, "verticalError", legs[i].verticalError));
				storeline.push_back(autoParam(base, "hitSomething", legs[i].hitSomething));
				storeline.push_back(autoParam(base, "isPlantedMS", legs[i].isPlantedMS));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 12)
				{
					output.push_back("");	// 12 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("fwdAxisLS", legs[i].fwdAxisLS, ctrpart->legs[i].fwdAxisLS, output, storeline, base, true, open, isEdited);
			output.push_back(openParam(base, "ungroundedEvent"));		// 4
			output.push_back(openObject(base));		// 5
			paramMatch("id", legs[i].ungroundedEvent.id, ctrpart->legs[i].ungroundedEvent.id, output, storeline, base, false, open, isEdited);
			paramMatch("payload", legs[i].ungroundedEvent.payload, ctrpart->legs[i].ungroundedEvent.payload, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 5
			output.push_back(closeParam(base));		// 4
			paramMatch("verticalError", legs[i].verticalError, ctrpart->legs[i].verticalError, output, storeline, base, false, open, isEdited);
			paramMatch("hitSomething", legs[i].hitSomething, ctrpart->legs[i].hitSomething, output, storeline, base, false, open, isEdited);
			paramMatch("isPlantedMS", legs[i].isPlantedMS, ctrpart->legs[i].isPlantedMS, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 3
		}
	}

	if (size > 0)
	{
		if (orisize == 0)
		{
			nemesis::try_open(open, isEdited, output);
			output.push_back(closeParam(base));		// 2
			nemesis::try_close(open, output, storeline);
		}
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(closeParam(base));		// 2
		}
	}
	else nemesis::try_close(open, output, storeline);

	paramMatch("errorOutTranslation", errorOutTranslation, ctrpart->errorOutTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("alignWithGroundRotation", alignWithGroundRotation, ctrpart->alignWithGroundRotation, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", footikcontrolsmodifier::classname, output, isEdited);
}

void hkbfootikcontrolsmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = legs.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, footikcontrolsmodifier::classname, footikcontrolsmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "controlData"));		// 3
	output.push_back(openObject(base));		// 4
	output.push_back(openParam(base, "gains"));		// 5
	output.push_back(openObject(base));		// 6
	output.push_back(autoParam(base, "onOffGain", controlData.gains.onOffGain));
	output.push_back(autoParam(base, "groundAscendingGain", controlData.gains.groundAscendingGain));
	output.push_back(autoParam(base, "groundDescendingGain", controlData.gains.groundDescendingGain));
	output.push_back(autoParam(base, "footPlantedGain", controlData.gains.footPlantedGain));
	output.push_back(autoParam(base, "footRaisedGain", controlData.gains.footRaisedGain));
	output.push_back(autoParam(base, "footUnlockGain", controlData.gains.footUnlockGain));
	output.push_back(autoParam(base, "worldFromModelFeedbackGain", controlData.gains.worldFromModelFeedbackGain));
	output.push_back(autoParam(base, "errorUpDownBias", controlData.gains.errorUpDownBias));
	output.push_back(autoParam(base, "alignWorldFromModelGain", controlData.gains.alignWorldFromModelGain));
	output.push_back(autoParam(base, "hipOrientationGain", controlData.gains.hipOrientationGain));
	output.push_back(autoParam(base, "maxKneeAngleDifference", controlData.gains.maxKneeAngleDifference));
	output.push_back(autoParam(base, "ankleOrientationGain", controlData.gains.ankleOrientationGain));
	output.push_back(closeObject(base));		// 6
	output.push_back(closeParam(base));		// 5
	output.push_back(closeObject(base));		// 4
	output.push_back(closeParam(base));			// 3

	if (size == 0)
	{
		output.push_back(openParam(base, "legs", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "legs", size));		// 2

	for (auto& leg : legs)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "fwdAxisLS", leg.fwdAxisLS));
		output.push_back(openParam(base, "ungroundedEvent"));		// 4
		output.push_back(openObject(base));		// 5
		output.push_back(autoParam(base, "id", leg.ungroundedEvent.id));
		output.push_back(autoParam(base, "payload", leg.ungroundedEvent.payload));
		output.push_back(closeObject(base));		// 5
		output.push_back(closeParam(base));		// 4
		output.push_back(autoParam(base, "verticalError", leg.verticalError));
		output.push_back(autoParam(base, "hitSomething", leg.hitSomething));
		output.push_back(autoParam(base, "isPlantedMS", leg.isPlantedMS));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "errorOutTranslation", errorOutTranslation));
	output.push_back(autoParam(base, "alignWithGroundRotation", alignWithGroundRotation));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, footikcontrolsmodifier::classname, output, true);
}

void hkbfootikcontrolsmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& leg : legs)
	{
		if (leg.ungroundedEvent.payload) hkb_parent[leg.ungroundedEvent.payload] = shared_from_this();
	}
}

void hkbfootikcontrolsmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (isOld)
	{
		usize tempint = 0;

		for (auto& leg : legs)
		{
			if (leg.ungroundedEvent.payload)
			{
				parentRefresh();
				leg.ungroundedEvent.payload->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(leg.ungroundedEvent.payload->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& leg : legs)
		{
			parentRefresh();
			if (leg.ungroundedEvent.payload) leg.ungroundedEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbfootikcontrolsmodifier::matchScoring(vector<hkleg>& ori, vector<hkleg>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<hkleg> newOri;
		vector<hkleg> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(hkleg());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	map<int, map<int, double>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].fwdAxisLS == edit[j].fwdAxisLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].ungroundedEvent.id == edit[j].ungroundedEvent.id)
			{
				scorelist[i][j] += 4;
			}

			if (matchIDCompare(ori[i].ungroundedEvent.payload, edit[j].ungroundedEvent.payload))
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].verticalError == edit[j].verticalError)
			{
				++scorelist[i][j];
			}

			if (ori[i].hitSomething == edit[j].hitSomething)
			{
				++scorelist[i][j];
			}

			if (ori[i].isPlantedMS == edit[j].isPlantedMS)
			{
				++scorelist[i][j];
			}

			if (i == j)
			{
				scorelist[i][j] += 2;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 2;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<hkleg> newOri;
	vector<hkleg> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(hkleg());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(hkleg());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbfootikcontrolsmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbFootIkControlsModifierExport(string id)
{
	//stage 1 reading
	string line;
	footik oriFootIK;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			footIKInfoProcess(FunctionLineTemp[id][i], oriFootIK);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkControlsModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	footik newFootIK;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			footIKInfoProcess(FunctionLineNew[id][i], newFootIK);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 identifying edits
	vecstr output;
	vecstr storeline;
	bool IsEdited = false;
	bool open = false;

	output.push_back(FunctionLineTemp[id][0]);

	if (oriFootIK.variableBindingSet != newFootIK.variableBindingSet)
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back("			<hkparam name=\"variableBindingSet\">" + newFootIK.variableBindingSet + "</hkparam>");
		storeline.push_back("			<hkparam name=\"variableBindingSet\">" + oriFootIK.variableBindingSet + "</hkparam>");
		IsEdited = true;
		open = true;
	}
	else
	{
		output.push_back("			<hkparam name=\"variableBindingSet\">" + oriFootIK.variableBindingSet + "</hkparam>");
	}

	if (oriFootIK.variableBindingSet != newFootIK.variableBindingSet)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			open = true;
		}

		output.push_back("			<hkparam name=\"userData\">" + to_string(newFootIK.userData) + "</hkparam>");
		storeline.push_back("			<hkparam name=\"userData\">" + to_string(oriFootIK.userData) + "</hkparam>");
	}
	else
	{
		if (open)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back("			<hkparam name=\"userData\">" + to_string(oriFootIK.userData) + "</hkparam>");
	}

	if (oriFootIK.name != newFootIK.name)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			open = true;
		}

		output.push_back("			<hkparam name=\"name\">" + newFootIK.name + "</hkparam>");
		storeline.push_back("			<hkparam name=\"name\">" + oriFootIK.name + "</hkparam>");
	}
	else
	{
		if (open)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back("			<hkparam name=\"name\">" + oriFootIK.name + "</hkparam>");
	}

	if (oriFootIK.enable != newFootIK.enable)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
		}

		output.push_back("			<hkparam name=\"enable\">" + from_bool(newFootIK.enable) + "</hkparam>");
		storeline.push_back("			<hkparam name=\"enable\">" + from_bool(oriFootIK.enable) + "</hkparam>");
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}
	else
	{
		if (open)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back("			<hkparam name=\"enable\">" + from_bool(oriFootIK.enable) + "</hkparam>");
	}

	output.push_back("			<hkparam name=\"controlData\">");
	output.push_back("				<hkobject>");
	output.push_back("					<hkparam name=\"gains\">");
	output.push_back("						<hkobject>");

	inputDoubleFootIK(output, storeline, oriFootIK.gains.onOffGain, newFootIK.gains.onOffGain, "onOffGain", IsEdited, open, true);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.groundAscendingGain, newFootIK.gains.groundAscendingGain, "groundAscendingGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.groundDescendingGain, newFootIK.gains.groundDescendingGain, "groundDescendingGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.footPlantedGain, newFootIK.gains.footPlantedGain, "footPlantedGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.footRaisedGain, newFootIK.gains.footRaisedGain, "footRaisedGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.footUnlockGain, newFootIK.gains.footUnlockGain, "footUnlockGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.worldFromModelFeedbackGain, newFootIK.gains.worldFromModelFeedbackGain, "worldFromModelFeedbackGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.errorUpDownBias, newFootIK.gains.errorUpDownBias, "errorUpDownBias", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.alignWorldFromModelGain, newFootIK.gains.alignWorldFromModelGain, "alignWorldFromModelGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.hipOrientationGain, newFootIK.gains.hipOrientationGain, "hipOrientationGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.maxKneeAngleDifference, newFootIK.gains.maxKneeAngleDifference, "maxKneeAngleDifference", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.gains.ankleOrientationGain, newFootIK.gains.ankleOrientationGain, "ankleOrientationGain", IsEdited, open);

	if (open)
	{
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}

	output.push_back("						</hkobject>");
	output.push_back("					</hkparam>");
	output.push_back("				</hkobject>");
	output.push_back("			</hkparam>");

	bool newZero = false;
	bool oriZero = false;
	bool notSameCount = false;

	if (oriFootIK.legs.size() != newFootIK.legs.size())
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

		if (newFootIK.legs.size() != 0)
		{
			output.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(newFootIK.legs.size()) + "\">");
		}
		else
		{
			output.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
			newZero = true;
		}

		if (oriFootIK.legs.size() != 0)
		{
			if (newFootIK.legs.size() != 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(oriFootIK.legs.size()) + "\">");
				output.push_back("<!-- CLOSE -->");
			}
		}
		else
		{
			oriZero = true;
		}

		IsEdited = true;
		notSameCount = true;
	}
	else if (oriFootIK.legs.size() > 0)
	{
		output.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(oriFootIK.legs.size()) + "\">");
	}
	else
	{
		output.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
	}

	if (!matchLegsScoring(oriFootIK.legs, newFootIK.legs, id))
	{
		return;
	}

	for (unsigned int i = 0; i < oriFootIK.legs.size(); ++i)
	{
		storeline.clear();
		bool open1 = false;

		if (newFootIK.legs[i].proxy)
		{
			vecstr instore;

			if (!newZero)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			}
			else
			{
				instore.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(oriFootIK.legs.size()) + "\">");
			}

			IsEdited = true;
			bool nobreak = true;

			while (i < oriFootIK.legs.size())
			{
				if (!newFootIK.legs[i].proxy)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), instore.begin(), instore.end());
					output.push_back("<!-- CLOSE -->");
					nobreak = false;
					--i;
					break;
				}

				int add = 0;

				while (add < 12)
				{
					output.push_back("");
					++add;
				}

				inputFootIK(instore, oriFootIK.legs[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());

				if (!newZero)
				{
					output.push_back("<!-- CLOSE -->");
				}
			}
		}
		else if (!oriFootIK.legs[i].proxy)
		{
			output.push_back("				<hkobject>");

			if (oriFootIK.legs[i].groundPosition != newFootIK.legs[i].groundPosition)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"groundPosition\">" + newFootIK.legs[i].groundPosition + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.push_back("					<hkparam name=\"groundPosition\">" + oriFootIK.legs[i].groundPosition + "</hkparam>");
				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"groundPosition\">" + oriFootIK.legs[i].groundPosition + "</hkparam>");
			}

			output.push_back("					<hkparam name=\"ungroundedEvent\">");
			output.push_back("						<hkobject>");

			if (oriFootIK.legs[i].ungroundedEvent.id != newFootIK.legs[i].ungroundedEvent.id)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("							<hkparam name=\"id\">" + newFootIK.legs[i].ungroundedEvent.id.getID() + "</hkparam>");
				open1 = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"id\">" + oriFootIK.legs[i].ungroundedEvent.id.getID() + "</hkparam>");
			}

			if (oriFootIK.legs[i].ungroundedEvent.payload != newFootIK.legs[i].ungroundedEvent.payload)
			{
				if (!open1)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("							<hkparam name=\"payload\">" + newFootIK.legs[i].ungroundedEvent.payload->ID + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"payload\">" + oriFootIK.legs[i].ungroundedEvent.payload->ID + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					IsEdited = true;
				}
				else
				{
					output.push_back("							<hkparam name=\"payload\">" + newFootIK.legs[i].ungroundedEvent.payload->ID + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + oriFootIK.legs[i].ungroundedEvent.id.getID() + "</hkparam>");
					output.push_back("							<hkparam name=\"payload\">" + oriFootIK.legs[i].ungroundedEvent.payload->ID + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open1 = false;
				}
			}
			else
			{
				if (open1)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + oriFootIK.legs[i].ungroundedEvent.id.getID() + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open1 = false;
				}

				output.push_back("							<hkparam name=\"payload\">" + oriFootIK.legs[i].ungroundedEvent.payload->ID + "</hkparam>");
			}

			output.push_back("						</hkobject>");
			output.push_back("					</hkparam>");

			if (oriFootIK.legs[i].verticalError != newFootIK.legs[i].verticalError)
			{
				stringstream ssNewD;
				ssNewD << setprecision(6) << fixed << newFootIK.legs[i].verticalError;

				stringstream ssOriD;
				ssOriD << setprecision(6) << fixed << oriFootIK.legs[i].verticalError;

				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"verticalError\">" + ssNewD.str() + "</hkparam>");
				storeline.push_back("					<hkparam name=\"verticalError\">" + ssOriD.str() + "</hkparam>");
				IsEdited = true;
				open1 = true;
			}
			else
			{
				stringstream ssD;
				ssD << setprecision(6) << fixed << newFootIK.legs[i].verticalError;
				output.push_back("					<hkparam name=\"verticalError\">" + ssD.str() + "</hkparam>");
			}

			if (oriFootIK.legs[i].hitSomething != newFootIK.legs[i].hitSomething)
			{
				if (!open1)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open1 = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"hitSomething\">" + from_bool(newFootIK.legs[i].hitSomething) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"hitSomething\">" + from_bool(oriFootIK.legs[i].hitSomething) + "</hkparam>");
			}
			else
			{
				if (open1)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					storeline.clear();
					open1 = false;
				}

				output.push_back("					<hkparam name=\"hitSomething\">" + from_bool(oriFootIK.legs[i].hitSomething) + "</hkparam>");
			}

			if (oriFootIK.legs[i].isPlantedMS != newFootIK.legs[i].isPlantedMS)
			{
				if (!open1)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(newFootIK.legs[i].isPlantedMS) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(oriFootIK.legs[i].isPlantedMS) + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
				storeline.clear();
			}
			else
			{
				if (open1)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					storeline.clear();
				}

				output.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(oriFootIK.legs[i].isPlantedMS) + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			if (!oriZero)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			while (i < oriFootIK.legs.size())
			{
				inputFootIK(output, newFootIK.legs[i]);
				++i;
			}

			if (!oriZero)
			{
				output.push_back("<!-- CLOSE -->");
			}
		}
	}

	if (notSameCount)
	{
		if (newZero)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("			</hkparam>");
		}
	}
	else if (!oriZero)
	{
		output.push_back("			</hkparam>");
	}

	if (oriFootIK.errorOutTranslation != newFootIK.errorOutTranslation)
	{
		if (notSameCount && oriZero)
		{
			storeline.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		}

		output.push_back("			<hkparam name=\"errorOutTranslation\">" + newFootIK.errorOutTranslation + "</hkparam>");
		storeline.push_back("			<hkparam name=\"errorOutTranslation\">" + oriFootIK.errorOutTranslation + "</hkparam>");
		IsEdited = true;
		open = true;
	}
	else
	{
		if (notSameCount && oriZero)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
			output.push_back("<!-- CLOSE -->");
		}

		output.push_back("			<hkparam name=\"errorOutTranslation\">" + oriFootIK.errorOutTranslation + "</hkparam>");
	}

	if (oriFootIK.alignWithGroundRotation != newFootIK.alignWithGroundRotation)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
		}

		output.push_back("			<hkparam name=\"alignWithGroundRotation\">" + newFootIK.alignWithGroundRotation + "</hkparam>");
		storeline.push_back("			<hkparam name=\"alignWithGroundRotation\">" + oriFootIK.alignWithGroundRotation + "</hkparam>");
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}
	else
	{
		if (open)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back("			<hkparam name=\"alignWithGroundRotation\">" + oriFootIK.alignWithGroundRotation + "</hkparam>");
	}
	
	output.push_back("		</hkobject>");
	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				fwrite << output[i] << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (File: " << filename << ")" << endl;
			Error = true;
		}
	}
	else
	{
		if (IsFileExist(filename))
		{
			if (remove(filename.c_str()) != 0)
			{
				perror("Error deleting file");
				Error = true;
			}
		}
	}
}

void footIKInfoProcess(string line, footik& footIK)
{
	if (line.find("<hkparam name=\"variableBindingSet\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"variableBindingSet\">") + 35;
		footIK.variableBindingSet = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"userData\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"userData\">") + 25;
		footIK.userData = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"name\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"name\">") + 21;
		footIK.name = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"enable\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"enable\">") + 23;
		footIK.enable = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"onOffGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"onOffGain\">") + 26;
		footIK.gains.onOffGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"groundAscendingGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"groundAscendingGain\">") + 36;
		footIK.gains.groundAscendingGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"groundDescendingGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"groundDescendingGain\">") + 37;
		footIK.gains.groundDescendingGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"footPlantedGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"footPlantedGain\">") + 32;
		footIK.gains.footPlantedGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"footRaisedGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"footRaisedGain\">") + 31;
		footIK.gains.footRaisedGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"footUnlockGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"footUnlockGain\">") + 31;
		footIK.gains.footUnlockGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"worldFromModelFeedbackGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"worldFromModelFeedbackGain\">") + 43;
		footIK.gains.worldFromModelFeedbackGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"errorUpDownBias\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"errorUpDownBias\">") + 32;
		footIK.gains.errorUpDownBias = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"alignWorldFromModelGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"alignWorldFromModelGain\">") + 40;
		footIK.gains.alignWorldFromModelGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"hipOrientationGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"hipOrientationGain\">") + 35;
		footIK.gains.hipOrientationGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"maxKneeAngleDifference\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"maxKneeAngleDifference\">") + 39;
		footIK.gains.maxKneeAngleDifference = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"ankleOrientationGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"ankleOrientationGain\">") + 37;
		footIK.gains.ankleOrientationGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"groundPosition\">") != string::npos)
	{
		leg dummy;
		dummy.proxy = false;
		size_t pos = line.find("<hkparam name=\"groundPosition\">") + 31;
		dummy.groundPosition = line.substr(pos, line.find("</hkparam>", pos) - pos);
		footIK.legs.push_back(dummy);
	}
	else if (line.find("<hkparam name=\"id\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"id\">") + 19;
		footIK.legs.back().ungroundedEvent.id = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"payload\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"payload\">") + 24;
		footIK.legs.back().ungroundedEvent.payload->ID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"verticalError\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"verticalError\">") + 30;
		footIK.legs.back().verticalError = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"hitSomething\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"hitSomething\">") + 29;
		footIK.legs.back().hitSomething = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"isPlantedMS\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"isPlantedMS\">") + 28;
		footIK.legs.back().isPlantedMS = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"errorOutTranslation\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"errorOutTranslation\">") + 36;
		footIK.errorOutTranslation = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"alignWithGroundRotation\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"alignWithGroundRotation\">") + 40;
		footIK.alignWithGroundRotation = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
}

bool matchLegsScoring(vector<leg>& ori, vector<leg>& edit, string id)
{
	int counter = 0;
	map<int, map<int, double>> scorelist;
	map<int, bool> taken;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].groundPosition == edit[j].groundPosition)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].ungroundedEvent.id == edit[j].ungroundedEvent.id)
			{
				scorelist[i][j] += 10;
			}

			if (matchIDCompare(ori[i].ungroundedEvent.payload, edit[j].ungroundedEvent.payload))
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].verticalError == edit[j].verticalError)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].hitSomething == edit[j].hitSomething)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].isPlantedMS == edit[j].isPlantedMS)
			{
				scorelist[i][j] += 2;
			}

			if (i == j)
			{
				++scorelist[i][j];
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<leg> newOri;
	vector<leg> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			leg dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			leg dummy;
			newEdit.push_back(dummy);
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

void inputDoubleFootIK(vecstr& output, vecstr& storeline, double oriD, double newD, string key, bool& IsEdited, bool& open, bool first)
{
	if (first)
	{
		if (oriD != newD)
		{
			stringstream ssNewD;
			ssNewD << setprecision(6) << fixed << newD;

			stringstream ssOriD;
			ssOriD << setprecision(6) << fixed << oriD;

			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			output.push_back("							<hkparam name=\"" + key + "\">" + ssNewD.str() + "</hkparam>");
			storeline.push_back("							<hkparam name=\"" + key + "\">" + ssOriD.str() + "</hkparam>");
			IsEdited = true;
			open = true;
		}
		else
		{
			stringstream ssD;
			ssD << setprecision(6) << fixed << oriD;
			output.push_back("							<hkparam name=\"" + key + "\">" + ssD.str() + "</hkparam>");
		}
	}
	else
	{
		if (oriD != newD)
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			stringstream ssNewD;
			ssNewD << setprecision(6) << fixed << newD;

			stringstream ssOriD;
			ssOriD << setprecision(6) << fixed << oriD;

			output.push_back("							<hkparam name=\"" + key + "\">" + ssNewD.str() + "</hkparam>");
			storeline.push_back("							<hkparam name=\"" + key + "\">" + ssOriD.str() + "</hkparam>");
		}
		else
		{
			if (open)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
				storeline.clear();
				open = false;
			}

			stringstream ssD;
			ssD << setprecision(6) << fixed << oriD;
			output.push_back("							<hkparam name=\"" + key + "\">" + ssD.str() + "</hkparam>");
		}
	}
}

void inputFootIK(vecstr& input, leg& leg)
{
	stringstream verticalError;
	verticalError << setprecision(6) << fixed << leg.verticalError;

	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"groundPosition\">" + leg.groundPosition + "</hkparam>");
	input.push_back("					<hkparam name=\"ungroundedEvent\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"id\">" + leg.ungroundedEvent.id.getID() + "</hkparam>");
	input.push_back("							<hkparam name=\"payload\">" + (leg.ungroundedEvent.payload ? leg.ungroundedEvent.payload->ID : "null") + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"verticalError\">" + verticalError.str() + "</hkparam>");
	input.push_back("					<hkparam name=\"hitSomething\">" + from_bool(leg.hitSomething) + "</hkparam>");
	input.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(leg.isPlantedMS) + "</hkparam>");
	input.push_back("				</hkobject>");
}

namespace keepsake
{
	void hkbFootIkControlsModifierExport(string id)
	{
		//stage 1 reading
		vecstr storeline1;
		storeline1.reserve(FunctionLineTemp[id].size());
		string line;

		if (FunctionLineTemp[id].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
			{
				line = FunctionLineTemp[id][i];

				if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) &&
					(line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
				{
					storeline1.push_back(line);
				}
			}
		}
		else
		{
			cout << "ERROR: Edit hkbFootIkControlsModifier Input Not Found (ID: " << id << ")" << endl;
			Error = true;
		}

		vecstr storeline2;
		storeline2.reserve(FunctionLineNew[id].size());

		if (FunctionLineNew[id].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
			{
				line = FunctionLineNew[id][i];

				if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) &&
					(line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
				{
					storeline2.push_back(line);
				}
			}
		}
		else
		{
			cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (ID: " << id << ")" << endl;
			Error = true;
		}

		//stage 2 identifying edits
		vecstr output;
		bool IsEdited = false;
		bool IsChanged = false;
		bool open = false;
		int curline = 0;
		int part = 0;
		int openpoint;
		int closepoint;

		for (unsigned int i = 0; i < storeline2.size(); i++)
		{
			if ((storeline1[curline].find("<hkparam name=\"legs\" numelements=", 0) == string::npos) && (part == 0)) // pre legs info
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;

				if (storeline1[curline].find("<hkparam name=\"legs\" numelements=", 0) != string::npos)
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}
			}
			else if ((storeline1[curline].find("<hkparam name=\"errorOutTranslation\">", 0) == string::npos) && (part == 1))
			{
				if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) &&
					((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
					}

					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) &&
							(storeline2[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}
				}
				else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) &&
					(storeline1[curline].length() == storeline2[i].length())) &&
					((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
					}

					if (open)
					{
						closepoint = curline;
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);
				}

				curline++;

				if ((open) && (storeline2[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos))
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				if (storeline2[i].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}
			}
			else if (part == 2) // existing leftover settings
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;
			}
			else // added variable value
			{
				if (storeline2[i].find("<hkparam name=\"legs\" numelements=", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

						if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
						{
							output.push_back("				<hkobject>");
							output.push_back(storeline2[i]);
						}
						else if (storeline2[i].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
						{
							output.push_back(storeline2[i]);
							output.push_back("				</hkobject>");
						}
						else
						{
							output.push_back(storeline2[i]);
						}

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("errorOutTranslation", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					part = 1;
					curline++;
				}
				else if (storeline2[i].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
					part = 2;
					curline++;
				}
				else
				{
					if (part == 1)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
						{
							output.push_back("				<hkobject>");
							output.push_back(storeline2[i]);
						}
						else if (storeline2[i].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
						{
							output.push_back(storeline2[i]);
							output.push_back("				</hkobject>");
						}
						else
						{
							output.push_back(storeline2[i]);
						}

						if (storeline2[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos)
						{
							if (IsChanged)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
								{
									if ((storeline1[j].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos) &&
										(output.back().find("ORIGINAL", 0) == string::npos))
									{
										output.push_back("				<hkobject>");
									}

									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}

								}

								IsChanged = false;
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
				}
			}
		}

		if (open) // close unclosed edits
		{
			if (IsChanged)
			{
				closepoint = curline;

				if (closepoint != openpoint)
				{
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}
				}

				IsChanged = false;
			}

			output.push_back("<!-- CLOSE -->");
			open = false;
		}

		NemesisReaderFormat(stoi(id.substr(1)), output);

		// stage 3 output if it is edited
		string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
		bool closeOri = false;
		bool closeEdit = false;

		if (IsEdited)
		{
			ofstream outputfile(filename);

			if (outputfile.is_open())
			{
				FunctionWriter fwrite(&outputfile);
				part = 0;

				for (unsigned int i = 0; i < output.size(); i++)
				{
					if (i < output.size() - 1)
					{
						if ((output[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos) &&
							(output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"legs\" numelements=", 0) != string::npos) &&
							(output[i + 1].find("</hkparam>", 0) != string::npos) &&
							(output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
						{
							if (!closeEdit)
							{
								fwrite << "			</hkparam>" << "\n";
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"legs\" numelements=", 0) != string::npos) &&
							(output[i].find("<hkparam name=\"ankleOrientationGain\">", 0) == string::npos) &&
							(output[i].find("ORIGINAL", 0) == string::npos) && (output[i].find("CLOSE", 0) == string::npos))
						{
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
							fwrite << "				</hkobject>" << "\n";
							fwrite << "			</hkparam>" << "\n";
						}
					}

					if (part == 0)
					{
						if (output[i].find("<hkparam name=\"legs\" numelements=", 0) != string::npos)
						{
							if (output[i - 1].find("OPEN", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
								}
								else
								{
									closeEdit = false;
								}
							}
							else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeOri = true;
								}
								else
								{
									closeOri = false;
								}
							}
							else
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
									closeOri = true;
								}
								else
								{
									closeEdit = false;
									closeOri = false;
								}
							}
						}

						fwrite << output[i] << "\n";

						if (output[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos)
						{
							part = 1;
						}
					}
					else
					{
						fwrite << output[i] << "\n";
					}

					if (output[i].find("<hkparam name=\"controlData\">", 0) != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i].find("<hkparam name=\"gains\">", 0) != string::npos)
					{
						fwrite << "						<hkobject>" << "\n";
					}

					if (i < output.size() - 1)
					{
						if (output[i + 1].find("errorOutTranslation", 0) != string::npos) // close outside if both aren't closed
						{
							if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))
							{
								if ((!closeOri) && (!closeEdit))
								{
									fwrite << "			</hkparam>" << "\n";
									closeOri = true;
									closeEdit = true;
								}
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"legs\" numelements=", 0) != string::npos) &&
							((output[i].find("<hkparam name=\"ankleOrientationGain\">", 0) != string::npos) || (output[i].find("CLOSE", 0) != string::npos)))
						{
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
							fwrite << "				</hkobject>" << "\n";
							fwrite << "			</hkparam>" << "\n";
						}
					}
				}

				fwrite << "		</hkobject>" << "\n";
				outputfile.close();
			}
			else
			{
				cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (File: " << filename << ")" << endl;
				Error = true;
			}
		}
		else
		{
			if (IsFileExist(filename))
			{
				if (remove(filename.c_str()) != 0)
				{
					perror("Error deleting file");
					Error = true;
				}
			}
		}
	}
}