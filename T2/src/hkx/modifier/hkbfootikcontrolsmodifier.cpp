#include <boost\thread.hpp>
#include "hkbfootikcontrolsmodifier.h"
#include "boolstring.h"
#include "highestscore.h"
#include "src\hkx\modifier\base\hkbfootikgains.h"

using namespace std;

namespace footikcontrolsmodifier
{
	const string key = "af";
	const string classname = "hkbFootIkControlsModifier";
	const string signature = "0xe5b6f544";
}

string hkbfootikcontrolsmodifier::GetAddress()
{
	return address;
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
