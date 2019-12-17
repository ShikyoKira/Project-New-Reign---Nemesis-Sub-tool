#include "hkbextractragdollposemodifier.h"

using namespace std;

namespace extractragdollposemodifier
{
	const string key = "ax";
	const string classname = "hkbExtractRagdollPoseModifier";
	const string signature = "0x804dcbab";
}

string hkbextractragdollposemodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbextractragdollposemodifier>> hkbextractragdollposemodifierList;
safeStringUMap<shared_ptr<hkbextractragdollposemodifier>> hkbextractragdollposemodifierList_E;

void hkbextractragdollposemodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbextractragdollposemodifierList_E[id] = shared_from_this() : hkbextractragdollposemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbextractragdollposemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("poseMatchingBone0", line, poseMatchingBone0)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("poseMatchingBone1", line, poseMatchingBone1)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("poseMatchingBone2", line, poseMatchingBone2)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("enableComputeWorldFromModel", line, enableComputeWorldFromModel)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << extractragdollposemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbextractragdollposemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + extractragdollposemodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) variableBindingSet->connect(filepath, curadd, functionlayer + 1, false, graphroot);
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
				hkbextractragdollposemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbextractragdollposemodifierList_E[ID] = protect;
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
				hkbextractragdollposemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbextractragdollposemodifierList_E[ID] = protect;
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

string hkbextractragdollposemodifier::getClassCode()
{
	return extractragdollposemodifier::key;
}

void hkbextractragdollposemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(10);
	usize base = 2;
	hkbextractragdollposemodifier* ctrpart = static_cast<hkbextractragdollposemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, extractragdollposemodifier::classname, extractragdollposemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("poseMatchingBone0", poseMatchingBone0, ctrpart->poseMatchingBone0, output, storeline, base, false, open, isEdited);
	paramMatch("poseMatchingBone1", poseMatchingBone1, ctrpart->poseMatchingBone1, output, storeline, base, false, open, isEdited);
	paramMatch("poseMatchingBone2", poseMatchingBone2, ctrpart->poseMatchingBone2, output, storeline, base, false, open, isEdited);
	paramMatch("enableComputeWorldFromModel", enableComputeWorldFromModel, ctrpart->enableComputeWorldFromModel, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", extractragdollposemodifier::classname, output, isEdited);
}

void hkbextractragdollposemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, extractragdollposemodifier::classname, extractragdollposemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "poseMatchingBone0", poseMatchingBone0));
	output.push_back(autoParam(base, "poseMatchingBone1", poseMatchingBone1));
	output.push_back(autoParam(base, "poseMatchingBone2", poseMatchingBone2));
	output.push_back(autoParam(base, "enableComputeWorldFromModel", enableComputeWorldFromModel));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, extractragdollposemodifier::classname, output, true);
}

void hkbextractragdollposemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbextractragdollposemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
