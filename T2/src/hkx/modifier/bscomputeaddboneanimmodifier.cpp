#include "bscomputeaddboneanimmodifier.h"

using namespace std;

namespace computeaddboneanimmodifier
{
	const string key = "bh";
	const string classname = "BSComputeAddBoneAnimModifier";
	const string signature = "0xa67f8c46";
}

string bscomputeaddboneanimmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bscomputeaddboneanimmodifier>> bscomputeaddboneanimmodifierList;
safeStringUMap<shared_ptr<bscomputeaddboneanimmodifier>> bscomputeaddboneanimmodifierList_E;

void bscomputeaddboneanimmodifier::regis(string id, bool isEdited)
{
	isEdited ? bscomputeaddboneanimmodifierList_E[id] = shared_from_this() : bscomputeaddboneanimmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bscomputeaddboneanimmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("boneIndex", line, boneIndex)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("translationLSOut", line, translationLSOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("rotationLSOut", line, rotationLSOut)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("scaleLSOut", line, scaleLSOut)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << computeaddboneanimmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bscomputeaddboneanimmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + computeaddboneanimmodifier::key + to_string(functionlayer) + ">";
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
				bscomputeaddboneanimmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bscomputeaddboneanimmodifierList_E[ID] = protect;
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
				bscomputeaddboneanimmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bscomputeaddboneanimmodifierList_E[ID] = protect;
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

string bscomputeaddboneanimmodifier::getClassCode()
{
	return computeaddboneanimmodifier::key;
}

void bscomputeaddboneanimmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(10);
	usize base = 2;
	bscomputeaddboneanimmodifier* ctrpart = static_cast<bscomputeaddboneanimmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, computeaddboneanimmodifier::classname, computeaddboneanimmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("boneIndex", boneIndex, ctrpart->boneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("translationLSOut", translationLSOut, ctrpart->translationLSOut, output, storeline, base, false, open, isEdited);
	paramMatch("rotationLSOut", rotationLSOut, ctrpart->rotationLSOut, output, storeline, base, false, open, isEdited);
	paramMatch("scaleLSOut", scaleLSOut, ctrpart->scaleLSOut, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", computeaddboneanimmodifier::classname, output, isEdited);
}

void bscomputeaddboneanimmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, computeaddboneanimmodifier::classname, computeaddboneanimmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "boneIndex", boneIndex));
	output.push_back(autoParam(base, "translationLSOut", translationLSOut));
	output.push_back(autoParam(base, "alignWithGroundDuration", rotationLSOut));
	output.push_back(autoParam(base, "rootBoneIndex", rotationLSOut));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, computeaddboneanimmodifier::classname, output, true);
}

void bscomputeaddboneanimmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void bscomputeaddboneanimmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
