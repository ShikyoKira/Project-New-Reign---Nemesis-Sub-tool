#include <boost\thread.hpp>
#include "bslookatmodifier.h"
#include "highestscore.h"

using namespace std;

namespace lookatmodifier_b
{
	string key = "bv";
	string classname = "BSLookAtModifier";
	string signature = "0xd756fc25";
}

bslookatmodifier::bslookatmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + lookatmodifier_b::key + to_string(functionlayer) + ">";

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

void bslookatmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSLookAtModifier(ID: " << id << ") has been initialized!" << endl;
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
				payload = line.substr(29, line.find("</hkparam>") - 29);

				if (payload != "null")
				{
					referencingIDs[payload].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: BSLookAtModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSLookAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bslookatmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSLookAtModifier(ID: " << id << ") has been initialized!" << endl;
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
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload = line.substr(29, line.find("</hkparam>") - 29);

				if (payload != "null")
				{
					if (!exchangeID[payload].empty())
					{
						int tempint = line.find(payload);
						payload = exchangeID[payload];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload);
					}

					parent[payload] = id;
					referencingIDs[payload].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSLookAtModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSLookAtModifier(newID: " << id << ") with BSLookAtModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (payload != "null")
		{
			referencingIDs[payload].pop_back();
			referencingIDs[payload].push_back(tempid);
			parent[payload] = tempid;
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
			cout << "Comparing BSLookAtModifier(newID: " << id << ") with BSLookAtModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSLookAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bslookatmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSLookAtModifier(ID: " << id << ") has been initialized!" << endl;
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
					payload = line.substr(29, line.find("</hkparam>") - 29);

					if (payload != "null")
					{
						if (!exchangeID[payload].empty())
						{
							payload = exchangeID[payload];
						}

						parent[payload] = id;
					}

					break;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSLookAtModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSLookAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

string bslookatmodifier::GetPayload()
{
	return "#" + boost::regex_replace(string(payload), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bslookatmodifier::IsPayloadNull()
{
	if (payload.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bslookatmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bslookatmodifier::IsBindingNull()
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

string bslookatmodifier::GetAddress()
{
	return address;
}

bool bslookatmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bslookatmodifier>> bslookatmodifierList;
safeStringUMap<shared_ptr<bslookatmodifier>> bslookatmodifierList_E;

void bslookatmodifier::regis(string id, bool isEdited)
{
	isEdited ? bslookatmodifierList_E[id] = shared_from_this() : bslookatmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bslookatmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	vector<bone>* curbone = &bones;

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
					if (readParam("lookAtTarget", line, lookAtTarget)) ++type;

					break;
				}
				case 5:
				{
					usize numelement;

					if (readEleParam("bones", line, numelement))
					{
						bones.reserve(numelement);
						++type;
					}

					break;
				}
				case 6:
				{
					int output;

					if (readParam("index", line, output))
					{
						curbone->push_back(output);
						++type;
						break;
					}

					usize numelement;

					if (readEleParam("eyeBones", line, numelement))
					{
						eyeBones.reserve(numelement);
						curbone = &eyeBones;
						break;
					}
					
					if (readParam("limitAngleDegrees", line, limitAngleDegrees)) type = 12;

					break;
				}
				case 7:
				{
					if (readParam("fwdAxisLS", line, curbone->back().fwdAxisLS)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("limitAngleDegrees", line, curbone->back().limitAngleDegrees)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("onGain", line, curbone->back().onGain)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("offGain", line, curbone->back().offGain)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("enable", line, curbone->back().enable)) type = 6;

					break;
				}
				case 12:
				{
					if (readParam("limitAngleThresholdDegrees", line, limitAngleThresholdDegrees)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("continueLookOutsideOfLimit", line, continueLookOutsideOfLimit)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("onGain", line, onGain)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("offGain", line, offGain)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("useBoneGains", line, useBoneGains)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("targetLocation", line, targetLocation)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("targetOutsideLimits", line, targetOutsideLimits)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("id", line, targetOutOfLimitEvent.id)) ++type;

					break;
				}
				case 20:
				{
					string output;

					if (readParam("payload", line, output))
					{
						targetOutOfLimitEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 21:
				{
					if (readParam("lookAtCamera", line, lookAtCamera)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("lookAtCameraX", line, lookAtCameraX)) ++type;

					break;
				}
				case 23:
				{
					if (readParam("lookAtCameraY", line, lookAtCameraY)) ++type;

					break;
				}
				case 24:
				{
					if (readParam("lookAtCameraZ", line, lookAtCameraZ)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << lookatmodifier_b::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bslookatmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + lookatmodifier_b::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	targetOutOfLimitEvent.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (targetOutOfLimitEvent.payload) threadedNextNode(targetOutOfLimitEvent.payload, filepath, curadd, functionlayer, graphroot);
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
				bslookatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bslookatmodifierList_E[ID] = protect;
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
				bslookatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bslookatmodifierList_E[ID] = protect;
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

string bslookatmodifier::getClassCode()
{
	return lookatmodifier_b::key;
}

void bslookatmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(100);
	usize base = 2;
	bslookatmodifier* ctrpart = static_cast<bslookatmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, lookatmodifier_b::classname, lookatmodifier_b::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtTarget", lookAtTarget, ctrpart->lookAtTarget, output, storeline, base, true, open, isEdited);

	elementCompare(bones, ctrpart->bones, output, storeline, "bones", base, open, isEdited);
	elementCompare(eyeBones, ctrpart->eyeBones, output, storeline, "eyeBones", base, open, isEdited);

	paramMatch("limitAngleDegrees", limitAngleDegrees, ctrpart->limitAngleDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleThresholdDegrees", limitAngleThresholdDegrees, ctrpart->limitAngleThresholdDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("continueLookOutsideOfLimit", continueLookOutsideOfLimit, ctrpart->continueLookOutsideOfLimit, output, storeline, base, false, open, isEdited);
	paramMatch("onGain", onGain, ctrpart->onGain, output, storeline, base, false, open, isEdited);
	paramMatch("offGain", offGain, ctrpart->offGain, output, storeline, base, false, open, isEdited);
	paramMatch("useBoneGains", useBoneGains, ctrpart->useBoneGains, output, storeline, base, false, open, isEdited);
	paramMatch("targetLocation", targetLocation, ctrpart->targetLocation, output, storeline, base, false, open, isEdited);
	paramMatch("targetOutsideLimits", targetOutsideLimits, ctrpart->targetOutsideLimits, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "targetOutOfLimitEvent"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", targetOutOfLimitEvent.id, ctrpart->targetOutOfLimitEvent.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", targetOutOfLimitEvent.payload, ctrpart->targetOutOfLimitEvent.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("lookAtCamera", lookAtCamera, ctrpart->lookAtCamera, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtCameraX", lookAtCameraX, ctrpart->lookAtCameraX, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtCameraY", lookAtCameraY, ctrpart->lookAtCameraY, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtCameraZ", lookAtCameraZ, ctrpart->lookAtCameraZ, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", lookatmodifier_b::classname, output, isEdited);
}

void bslookatmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, lookatmodifier_b::classname, lookatmodifier_b::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "lookAtTarget", lookAtTarget));

	usize size = bones.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "bones", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bones", size));		// 2

	for (auto& bone : bones)
	{
		elementNew(output, bone, base);
	}
	
	if (size > 0) output.push_back(closeParam(base));		// 2

	size = eyeBones.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "eyeBones", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "eyeBones", size));		// 2

	for (auto& bone : eyeBones)
	{
		elementNew(output, bone, base);
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "limitAngleDegrees", limitAngleDegrees));
	output.push_back(autoParam(base, "limitAngleThresholdDegrees", limitAngleThresholdDegrees));
	output.push_back(autoParam(base, "continueLookOutsideOfLimit", continueLookOutsideOfLimit));
	output.push_back(autoParam(base, "onGain", onGain));
	output.push_back(autoParam(base, "offGain", offGain));
	output.push_back(autoParam(base, "useBoneGains", useBoneGains));
	output.push_back(autoParam(base, "targetLocation", targetLocation));
	output.push_back(autoParam(base, "targetOutsideLimits", targetOutsideLimits));
	output.push_back(openParam(base, "targetOutOfLimitEvent"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", targetOutOfLimitEvent.id));
	output.push_back(autoParam(base, "payload", targetOutOfLimitEvent.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "lookAtCamera", lookAtCamera));
	output.push_back(autoParam(base, "lookAtCameraX", lookAtCameraX));
	output.push_back(autoParam(base, "lookAtCameraY", lookAtCameraY));
	output.push_back(autoParam(base, "lookAtCameraZ", lookAtCameraZ));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, lookatmodifier_b::classname, output, true);
}

void bslookatmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (targetOutOfLimitEvent.payload) hkb_parent[targetOutOfLimitEvent.payload] = shared_from_this();
}

void bslookatmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (targetOutOfLimitEvent.payload)
	{
		parentRefresh();
		targetOutOfLimitEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bslookatmodifier::matchScoring(vector<bone>& ori, vector<bone>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<bone> newOri;
		vector<bone> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(bone());
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
			
			if (ori[i].index == edit[j].index)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].fwdAxisLS == edit[j].fwdAxisLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].limitAngleDegrees == edit[j].limitAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].onGain == edit[j].onGain)
			{
				++scorelist[i][j];
			}

			if (ori[i].offGain == edit[j].offGain)
			{
				++scorelist[i][j];
			}

			if (ori[i].enable == edit[j].enable)
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
	vector<bone> newOri;
	vector<bone> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(bone());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(bone());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void bslookatmodifier::elementCompare(vector<bone>& ori, vector<bone>& edit, vecstr& output, vecstr& storeline, string classname, usize& base, bool& open, bool& isEdited)
{
	usize size = ori.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, classname, size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, classname, size));		// 2

	matchScoring(ori, edit, ID);
	size = ori.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (ori[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				elementNew(output, edit[i], base);
				++i;
			}
		}
		// deleted existing data
		else if (edit[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!edit[i].proxy)
				{
					--i;
					break;
				}

				elementNew(storeline, ori[i], base);
				++i;
				usize spaces = 0;

				while (spaces++ < 8)
				{
					output.push_back("");	// 8 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("index", ori[i].index, edit[i].index, output, storeline, base, false, open, isEdited);
			paramMatch("fwdAxisLS", ori[i].fwdAxisLS, edit[i].fwdAxisLS, output, storeline, base, false, open, isEdited);
			paramMatch("limitAngleDegrees", ori[i].limitAngleDegrees, edit[i].limitAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("onGain", ori[i].onGain, edit[i].onGain, output, storeline, base, false, open, isEdited);
			paramMatch("offGain", ori[i].offGain, edit[i].offGain, output, storeline, base, false, open, isEdited);
			paramMatch("enable", ori[i].enable, edit[i].enable, output, storeline, base, true, open, isEdited);
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

}

void bslookatmodifier::elementNew(vecstr& output, bone& bone, usize& base)
{
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "index", bone.index));
	output.push_back(autoParam(base, "fwdAxisLS", bone.fwdAxisLS));
	output.push_back(autoParam(base, "limitAngleDegrees", bone.limitAngleDegrees));
	output.push_back(autoParam(base, "onGain", bone.onGain));
	output.push_back(autoParam(base, "offGain", bone.offGain));
	output.push_back(autoParam(base, "enable", bone.enable));
	output.push_back(closeObject(base));		// 3
}

void bslookatmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSLookAtModifierExport(string id)
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

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit BSLookAtModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit BSLookAtModifier Output Not Found (ID: " << id << ")" << endl;
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
	int storeI = 0;

	for (unsigned int i = storeI; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"bones\" numelements=", 0) == string::npos) // pre bones info
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

			if (storeline1[curline].find("<hkparam name=\"bones\" numelements=", 0) != string::npos)
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
		else 
		{
			storeI = i;
			break;
		}
	}

	part = 0;

	for (unsigned int i = storeI; i < storeline2.size(); i++)
	{
		if ((storeline1[curline].find("<hkparam name=\"eyeBones\" numelements=", 0) == string::npos) && (part == 0)) // existing bone data
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (storeline2[i].find("<hkparam name=\"index\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

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

			if (storeline2[i + 1].find("<hkparam name=\"eyeBones\" numelements=", 0) != string::npos)
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

				if (storeline2[i].find("<hkparam name=\"enabled\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}

				part = 1;
			}
		}
		else if ((storeline1[curline + 1].find("<hkparam name=\"limitAngleThresholdDegrees\">", 0) == string::npos) && (part == 1))  // existing eye bone data
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (storeline2[i].find("<hkparam name=\"index\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

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

			if (storeline2[i + 2].find("<hkparam name=\"limitAngleThresholdDegrees\">", 0) != string::npos)
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

				if (storeline2[i].find("<hkparam name=\"enabled\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}

				storeI = i + 1;
				break;
			}
		}
		else // new data
		{
			if (part == 0)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}

				if (storeline2[i].find("<hkparam name=\"index\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				output.push_back(storeline2[i]);

				if (storeline2[i].find("<hkparam name=\"enabled\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}

				if (storeline2[i + 1].find("<hkparam name=\"eyeBones\" numelements=", 0) != string::npos)
				{
					if (open)
					{
						closepoint = curline;

						if ((IsChanged) && (closepoint != openpoint))
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"enabled\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}
							}
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					part = 1;
				}
			}
			else if (part == 1)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}

				if (storeline2[i].find("<hkparam name=\"index\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				output.push_back(storeline2[i]);

				if (storeline2[i].find("<hkparam name=\"enabled\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}

				if (storeline2[i + 2].find("<hkparam name=\"limitAngleThresholdDegrees\">", 0) != string::npos)
				{
					if (open)
					{
						closepoint = curline;

						if ((IsChanged) && (closepoint != openpoint))
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"enabled\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}
							}
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					storeI = i + 1;
					break;
				}
			}
		}
	}

	for (unsigned int i = storeI; i < storeline2.size(); i++)
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
				if (i < output.size() - 2)
				{
					if (((output[i + 2].find("<hkparam name=\"limitAngleThresholdDegrees\">", 0) != string::npos) || (output[i + 1].find("<hkparam name=\"eyeBones\" numelements=", 0) != string::npos)) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if (((output[i + 1].find("<hkparam name=\"bones\" numelements=", 0) != string::npos) || (output[i + 1].find("<hkparam name=\"eyeBones\" numelements=", 0) != string::npos)) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if (output[i + 1].find("<hkparam name=\"limitAngleDegrees\">", 0) != string::npos)
					{
						if ((output[i].find("<hkparam name=\"fwdAxisLS\">", 0) == string::npos) && (output[i - 1].find("<hkparam name=\"fwdAxisLS\">", 0) == string::npos) && (output[i].find("OPEN", 0) != string::npos))
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
					}
				}

				if (part == 0)
				{
					fwrite << output[i] << "\n";

					if (output[i + 1].find("<hkparam name=\"bones\" numelements=", 0) != string::npos)
					{
						part = 1;
					}
				}
				else if (part == 1)
				{
					if ((output[i].find("<hkparam name=\"bones\" numelements=", 0) != string::npos) || (output[i].find("<hkparam name=\"eyeBones\" numelements=", 0) != string::npos))
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

					if ((output[i + 1].find("<hkparam name=\"index\">", 0) != string::npos) && (output[i].find("<hkobject>", 0) == string::npos))
					{
						if ((output[i].find("OPEN", 0) != string::npos) && (output[i - 1].find("<hkobject>", 0) == string::npos))
						{
							fwrite << "				<hkobject>" << "\n";
						}

						fwrite << output[i] << "\n";

						if ((output[i].find("OPEN", 0) == string::npos) && (output[i].find("ORIGINAL", 0) == string::npos))
						{
							if ((output[i].find("hkparam name=\"enabled\">", 0) != string::npos) && (output[i + 1].find("</hkobject>", 0) == string::npos) && (output[i + 1].find("CLOSE", 0) == string::npos) && (output[i + 1].find("ORIGINAL", 0) == string::npos))
							{
								fwrite << "				</hkobject>" << "\n";
							}

							fwrite << "				<hkobject>" << "\n";
						}
					}
					else if ((output[i].find("hkparam name=\"enabled\">", 0) != string::npos) && (output[i + 1].find("</hkobject>", 0) == string::npos) && (output[i + 1].find("CLOSE", 0) == string::npos) && (output[i + 1].find("ORIGINAL", 0) == string::npos))
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
						
					}
								
					if (output[i + 2].find("<hkparam name=\"limitAngleThresholdDegrees\">", 0) != string::npos)
					{
						part = 2;
					}
				}
				else
				{				
					if (i < output.size() - 1)
					{
						if (output[i + 1].find("<hkparam name=\"id\">", 0) != string::npos)
						{
							if (output[i].find("OPEN", 0) != string::npos)
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";

							if ((output[i].find("OPEN", 0) == string::npos) && (output[i].find("ORIGINAL", 0) == string::npos))
							{
								fwrite << "				<hkobject>" << "\n";
							}
						}
						else if (output[i - 1].find("<hkparam name=\"payload\">", 0) != string::npos)
						{
							if ((output[i].find("CLOSE", 0) == string::npos) && (output[i].find("ORIGINAL", 0) == string::npos))
							{
								fwrite << "				</hkobject>" << "\n";
								fwrite << "			</hkparam>" << "\n";
							}

							fwrite << output[i] << "\n";

							if (output[i].find("CLOSE", 0) != string::npos)
							{
								fwrite << "				</hkobject>" << "\n";
							}
						}
						else
						{
							fwrite << output[i] << "\n";
						}

						if (output[i + 1].find("<hkparam name=\"lookAtCamera\">", 0) != string::npos)
						{
							if (output[i].find("CLOSE", 0) != string::npos)
							{
								fwrite << "			</hkparam>" << "\n";
							}
						}
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}

				if (i < output.size() - 2)
				{
					if ((output[i + 2].find("<hkparam name=\"limitAngleThresholdDegrees\">", 0) != string::npos) || (output[i + 1].find("<hkparam name=\"eyeBones\" numelements=", 0) != string::npos)) // close outside if both aren't closed
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
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit BSLookAtModifier Output Not Found (File: " << filename << ")" << endl;
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