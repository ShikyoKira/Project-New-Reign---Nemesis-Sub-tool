#include <boost\thread.hpp>
#include "hkbkeyframebonesmodifier.h"
#include "highestscore.h"

using namespace std;

namespace keyframebonesmodifier
{
	string key = "bp";
	string classname = "hkbKeyframeBonesModifier";
	string signature = "0x95f66629";
}

hkbkeyframebonesmodifier::hkbkeyframebonesmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + keyframebonesmodifier::key + to_string(functionlayer) + ">";

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

void hkbkeyframebonesmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbKeyframeBonesModifier(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	string classname = "<hkobject name=\"" + id;
	bool record = false;

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
			else if (line.find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				keyframedbonelist = line.substr(38, line.find("</hkparam>") - 38);

				if (keyframedbonelist != "null")
				{
					referencingIDs[keyframedbonelist].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbKeyframeBonesModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbKeyframeBonesModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbkeyframebonesmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbKeyframeBonesModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				keyframedbonelist = line.substr(38, line.find("</hkparam>") - 38);

				if (keyframedbonelist != "null")
				{
					if (!exchangeID[keyframedbonelist].empty())
					{
						int tempint = line.find(keyframedbonelist);
						keyframedbonelist = exchangeID[keyframedbonelist];
						line.replace(tempint, line.find("</hkparam>") - tempint, keyframedbonelist);
					}

					parent[keyframedbonelist] = id;
					referencingIDs[keyframedbonelist].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbKeyframeBonesModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbKeyframeBonesModifier(newID: " << id << ") with hkbKeyframeBonesModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (keyframedbonelist != "null")
		{
			referencingIDs[keyframedbonelist].pop_back();
			referencingIDs[keyframedbonelist].push_back(tempid);
			parent[keyframedbonelist] = tempid;
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
			cout << "Comparing hkbKeyframeBonesModifier(newID: " << id << ") with hkbKeyframeBonesModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbKeyframeBonesModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbkeyframebonesmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbKeyframeBonesModifier(ID: " << id << ") has been initialized!" << endl;
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
			}
			else if (line.find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				keyframedbonelist = line.substr(38, line.find("</hkparam>") - 38);

				if (keyframedbonelist != "null")
				{
					if (!exchangeID[keyframedbonelist].empty())
					{
						keyframedbonelist = exchangeID[keyframedbonelist];
					}

					parent[keyframedbonelist] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbKeyframeBonesModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbKeyframeBonesModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbkeyframebonesmodifier::GetKeyframedBonesList()
{
	return "#" + boost::regex_replace(string(keyframedbonelist), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbkeyframebonesmodifier::IsKeyframedBonesListNull()
{
	if (keyframedbonelist.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbkeyframebonesmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbkeyframebonesmodifier::IsBindingNull()
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

string hkbkeyframebonesmodifier::GetAddress()
{
	return address;
}

bool hkbkeyframebonesmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbkeyframebonesmodifier>> hkbkeyframebonesmodifierList;
safeStringUMap<shared_ptr<hkbkeyframebonesmodifier>> hkbkeyframebonesmodifierList_E;

void hkbkeyframebonesmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbkeyframebonesmodifierList_E[id] = shared_from_this() : hkbkeyframebonesmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbkeyframebonesmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					usize numelement;

					if (readEleParam("keyframeInfo", line, numelement))
					{
						keyframeInfo.reserve(numelement);
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("keyframedPosition", line, output))
					{
						keyframeInfo.push_back(output);
						++type;
						break;
					}

					if (readParam("keyframedBonesList", line, output))
					{
						keyframedBonesList = (isEdited ? hkbboneindexarrayList_E : hkbboneindexarrayList)[output];
						type = 9;
					}

					break;
				}
				case 6:
				{
					if (readParam("keyframedRotation", line, keyframeInfo.back().keyframedRotation)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("boneIndex", line, keyframeInfo.back().boneIndex)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("isValid", line, keyframeInfo.back().isValid)) type = 4;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << keyframebonesmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbkeyframebonesmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + keyframebonesmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (keyframedBonesList) threadedNextNode(keyframedBonesList, filepath, curadd, functionlayer, graphroot);
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
				hkbkeyframebonesmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbkeyframebonesmodifierList_E[ID] = protect;
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
				hkbkeyframebonesmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbkeyframebonesmodifierList_E[ID] = protect;
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

string hkbkeyframebonesmodifier::getClassCode()
{
	return keyframebonesmodifier::key;
}

void hkbkeyframebonesmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = keyframeInfo.size();
	usize orisize = size;
	hkbkeyframebonesmodifier* ctrpart = static_cast<hkbkeyframebonesmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, keyframebonesmodifier::classname, keyframebonesmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);

	if (size == 0)
	{
		output.push_back(openParam(base, "keyframeInfo", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "keyframeInfo", size));		// 2

	matchScoring(keyframeInfo, ctrpart->keyframeInfo, ID);
	size = keyframeInfo.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (keyframeInfo[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "keyframedPosition", ctrpart->keyframeInfo[i].keyframedPosition));
				output.push_back(autoParam(base, "keyframedRotation", ctrpart->keyframeInfo[i].keyframedRotation));
				output.push_back(autoParam(base, "boneIndex", ctrpart->keyframeInfo[i].boneIndex));
				output.push_back(autoParam(base, "isValid", ctrpart->keyframeInfo[i].isValid));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->keyframeInfo[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->keyframeInfo[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "keyframedPosition", keyframeInfo[i].keyframedPosition));
				storeline.push_back(autoParam(base, "keyframedRotation", keyframeInfo[i].keyframedRotation));
				storeline.push_back(autoParam(base, "boneIndex", keyframeInfo[i].boneIndex));
				storeline.push_back(autoParam(base, "isValid", keyframeInfo[i].isValid));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 6)
				{
					output.push_back("");	// 6 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("keyframedPosition", keyframeInfo[i].keyframedPosition, ctrpart->keyframeInfo[i].keyframedPosition, output, storeline, base, false, open, isEdited);
			paramMatch("keyframedRotation", keyframeInfo[i].keyframedRotation, ctrpart->keyframeInfo[i].keyframedRotation, output, storeline, base, false, open, isEdited);
			paramMatch("boneIndex", keyframeInfo[i].boneIndex, ctrpart->keyframeInfo[i].boneIndex, output, storeline, base, false, open, isEdited);
			paramMatch("isValid", keyframeInfo[i].isValid, ctrpart->keyframeInfo[i].isValid, output, storeline, base, true, open, isEdited);
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

	paramMatch("keyframedBonesList", keyframedBonesList, ctrpart->keyframedBonesList, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", keyframebonesmodifier::classname, output, isEdited);
}

void hkbkeyframebonesmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = keyframeInfo.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, keyframebonesmodifier::classname, keyframebonesmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));

	if (size == 0)
	{
		output.push_back(openParam(base, "keyframeInfo", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "keyframeInfo", size));		// 2
	
	for (auto& info : keyframeInfo)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "keyframedPosition", info.keyframedPosition));
		output.push_back(autoParam(base, "keyframedRotation", info.keyframedRotation));
		output.push_back(autoParam(base, "boneIndex", info.boneIndex));
		output.push_back(autoParam(base, "isValid", info.isValid));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "keyframedBonesList", keyframedBonesList));
	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, keyframebonesmodifier::classname, output, true);
}

void hkbkeyframebonesmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (keyframedBonesList) hkb_parent[keyframedBonesList] = shared_from_this();
}

void hkbkeyframebonesmodifier::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (keyframedBonesList)
	{
		parentRefresh();
		keyframedBonesList->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbkeyframebonesmodifier::matchScoring(vector<keyframeinfo>& ori, vector<keyframeinfo>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<keyframeinfo> newOri;
		vector<keyframeinfo> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(keyframeinfo());
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

			if (ori[i].keyframedPosition == edit[j].keyframedPosition)
			{
				++scorelist[i][j];
			}

			if (ori[i].keyframedRotation == edit[j].keyframedRotation)
			{
				++scorelist[i][j];
			}

			if (ori[i].boneIndex == edit[j].boneIndex)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].isValid == edit[j].isValid)
			{
				++scorelist[i][j];
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
	vector<keyframeinfo> newOri;
	vector<keyframeinfo> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(keyframeinfo());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(keyframeinfo());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbkeyframebonesmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbKeyframeBonesModifierExport(string id)
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
		cout << "ERROR: Edit hkbKeyframeBonesModifier Input Not Found (ID: " << id << ")" << endl;
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
		cout << "ERROR: Edit hkbKeyframeBonesModifier Output Not Found (ID: " << id << ")" << endl;
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
		if ((storeline1[curline].find("<hkparam name=\"keyframeInfo\" numelements=", 0) == string::npos) && (part == 0)) // pre keyframeInfo info
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

			if (storeline1[curline].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos)
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
		else if ((storeline1[curline].find("<hkparam name=\"keyframedBonesList\">", 0) == string::npos) && (part == 1))
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) == string::npos))
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
			else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
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

			if ((open) && (storeline2[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos))
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

			if (storeline2[i].find("<hkparam name=\"isValid\">", 0) != string::npos)
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
			if (storeline2[i].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

					if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"isValid\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("handleOut", 0) == string::npos))
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
			else if (storeline2[i].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
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

					if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"isValid\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if (storeline2[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
					{
						if (IsChanged)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < curline; j++)
							{
								if ((storeline1[j].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos) && (output.back().find("ORIGINAL", 0) == string::npos))
								{
									output.push_back("				<hkobject>");
								}

								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"isValid\">", 0) != string::npos)
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
					if ((output[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
				}

				if (part == 0)
				{
					if (output[i].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos)
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

					if (output[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
					{
						part = 1;
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("keyframedBonesList", 0) != string::npos) // close outside if both aren't closed
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
			cout << "ERROR: Edit hkbKeyframeBonesModifier Output Not Found (File: " << filename << ")" << endl;
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