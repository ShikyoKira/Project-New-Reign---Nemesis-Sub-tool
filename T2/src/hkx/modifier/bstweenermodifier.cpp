#include "bstweenermodifier.h"

using namespace std;

namespace tweenermodifier
{
	const string key = "bi";
	const string classname = "BSTweenerModifier";
	const string signature = "0xd2d9a04";
}

string bstweenermodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bstweenermodifier>> bstweenermodifierList;
safeStringUMap<shared_ptr<bstweenermodifier>> bstweenermodifierList_E;

void bstweenermodifier::regis(string id, bool isEdited)
{
	isEdited ? bstweenermodifierList_E[id] = shared_from_this() : bstweenermodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bstweenermodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("tweenPosition", line, tweenPosition)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("tweenRotation", line, tweenRotation)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("useTweenDuration", line, useTweenDuration)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("targetPosition", line, targetPosition)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("targetRotation", line, targetRotation)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << tweenermodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bstweenermodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + tweenermodifier::key + to_string(functionlayer) + ">";
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
				bstweenermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bstweenermodifierList_E[ID] = protect;
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
				bstweenermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bstweenermodifierList_E[ID] = protect;
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

string bstweenermodifier::getClassCode()
{
	return tweenermodifier::key;
}

void bstweenermodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(11);
	output.reserve(11);
	usize base = 2;
	bstweenermodifier* ctrpart = static_cast<bstweenermodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, tweenermodifier::classname, tweenermodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("tweenPosition", tweenPosition, ctrpart->tweenPosition, output, storeline, base, false, open, isEdited);
	paramMatch("tweenRotation", tweenRotation, ctrpart->tweenRotation, output, storeline, base, false, open, isEdited);
	paramMatch("useTweenDuration", useTweenDuration, ctrpart->useTweenDuration, output, storeline, base, false, open, isEdited);
	paramMatch("targetPosition", targetPosition, ctrpart->targetPosition, output, storeline, base, false, open, isEdited);
	paramMatch("targetRotation", targetRotation, ctrpart->targetRotation, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", tweenermodifier::classname, output, isEdited);
}

void bstweenermodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(11);

	output.push_back(openObject(base, ID, tweenermodifier::classname, tweenermodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "tweenPosition", tweenPosition));
	output.push_back(autoParam(base, "tweenRotation", tweenRotation));
	output.push_back(autoParam(base, "useTweenDuration", useTweenDuration));
	output.push_back(autoParam(base, "targetPosition", targetPosition));
	output.push_back(autoParam(base, "targetRotation", targetRotation));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, tweenermodifier::classname, output, true);
}

void bstweenermodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void bstweenermodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
