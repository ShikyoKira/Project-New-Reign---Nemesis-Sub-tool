#include "bsisactivemodifier.h"

using namespace std;

namespace isactivatemodifier
{
	const string key = "ab";
	const string classname = "BSIsActiveModifier";
	const string signature = "0xb0fde45a";
}

string bsisactivemodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsisactivemodifier>> bsisactivemodifierList;
safeStringUMap<shared_ptr<bsisactivemodifier>> bsisactivemodifierList_E;

void bsisactivemodifier::regis(string id, bool isEdited)
{
	isEdited ? bsisactivemodifierList_E[id] = shared_from_this() : bsisactivemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsisactivemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{		if (line.find("<hkparam name=\"") != string::npos)		{			switch (type)			{				case 0:				{					string output;					if (readParam("variableBindingSet", line, output))					{						variableBindingSet = (isEdited ? hkbvariablebindingsetList_E : hkbvariablebindingsetList)[output];						++type;					}					break;				}				case 1:				{					if (readParam("userData", line, userData)) ++type;					break;				}				case 2:				{					if (readParam("name", line, name)) ++type;					break;				}				case 3:				{					if (readParam("enable", line, enable)) ++type;					break;				}				case 4:				{					if (readParam("bIsActive0", line, bIsActive0)) ++type;					break;				}				case 5:				{					if (readParam("bInvertActive0", line, bInvertActive0)) ++type;					break;				}				case 6:				{					if (readParam("bIsActive1", line, bIsActive1)) ++type;					break;				}				case 7:				{					if (readParam("bInvertActive1", line, bInvertActive1)) ++type;					break;				}				case 8:				{					if (readParam("bIsActive2", line, bIsActive2)) ++type;					break;				}				case 9:				{					if (readParam("bInvertActive2", line, bInvertActive2)) ++type;					break;				}				case 10:				{					if (readParam("bIsActive3", line, bIsActive3)) ++type;					break;				}				case 11:				{					if (readParam("bInvertActive3", line, bInvertActive3)) ++type;					break;				}				case 12:				{					if (readParam("bIsActive4", line, bIsActive4)) ++type;					break;				}				case 13:				{					if (readParam("bInvertActive4", line, bInvertActive4)) ++type;				}			}		}
	}

	if ((Debug) && (!Error))
	{
		cout << isactivatemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsisactivemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + isactivatemodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);			string curadd = address;		// protect changing address upon release lock			curLock.unlock();

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
				bsisactivemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsisactivemodifierList_E[ID] = protect;
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
				bsisactivemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsisactivemodifierList_E[ID] = protect;
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

string bsisactivemodifier::getClassCode()
{
	return isactivatemodifier::key;
}

void bsisactivemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(16);
	output.reserve(16);
	usize base = 2;
	bsisactivemodifier* ctrpart = static_cast<bsisactivemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, isactivatemodifier::classname, isactivatemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("bIsActive0", bIsActive0, ctrpart->bIsActive0, output, storeline, base, false, open, isEdited);
	paramMatch("bInvertActive0", bInvertActive0, ctrpart->bInvertActive0, output, storeline, base, false, open, isEdited);
	paramMatch("bIsActive1", bIsActive1, ctrpart->bIsActive1, output, storeline, base, false, open, isEdited);
	paramMatch("bInvertActive1", bInvertActive1, ctrpart->bInvertActive1, output, storeline, base, false, open, isEdited);
	paramMatch("bIsActive2", bIsActive2, ctrpart->bIsActive2, output, storeline, base, false, open, isEdited);
	paramMatch("bInvertActive2", bInvertActive2, ctrpart->bInvertActive2, output, storeline, base, false, open, isEdited);
	paramMatch("bIsActive3", bIsActive3, ctrpart->bIsActive3, output, storeline, base, false, open, isEdited);
	paramMatch("bInvertActive3", bInvertActive3, ctrpart->bInvertActive3, output, storeline, base, false, open, isEdited);
	paramMatch("bIsActive4", bIsActive4, ctrpart->bIsActive4, output, storeline, base, false, open, isEdited);
	paramMatch("bInvertActive4", bInvertActive4, ctrpart->bInvertActive4, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", isactivatemodifier::classname, output, isEdited);
}

void bsisactivemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(16);

	output.push_back(openObject(base, ID, isactivatemodifier::classname, isactivatemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "bIsActive0", bIsActive0));
	output.push_back(autoParam(base, "bInvertActive0", bInvertActive0));
	output.push_back(autoParam(base, "bIsActive1", bIsActive1));
	output.push_back(autoParam(base, "bInvertActive1", bInvertActive1));
	output.push_back(autoParam(base, "bIsActive2", bIsActive2));
	output.push_back(autoParam(base, "bInvertActive2", bInvertActive2));
	output.push_back(autoParam(base, "bIsActive3", bIsActive3));
	output.push_back(autoParam(base, "bInvertActive3", bInvertActive3));
	output.push_back(autoParam(base, "bIsActive4", bIsActive4));
	output.push_back(autoParam(base, "bInvertActive4", bInvertActive4));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, isactivatemodifier::classname, output, true);
}

void bsisactivemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void bsisactivemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
