#include <boost\thread.hpp>
#include "hkbkeyframebonesmodifier.h"
#include "highestscore.h"

using namespace std;

namespace keyframebonesmodifier
{
	const string key = "bp";
	const string classname = "hkbKeyframeBonesModifier";
	const string signature = "0x95f66629";
}

string hkbkeyframebonesmodifier::GetAddress()
{
	return address;
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
