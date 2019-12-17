#include <boost\thread.hpp>
#include "hkbeventdrivenmodifier.h"

using namespace std;

extern vector<usize> datapacktracker;

namespace eventdrivenmodifier
{
	const string key = "aa";
	const string classname = "hkbEventDrivenModifier";
	const string signature = "0x7ed3f44e";
}

string hkbeventdrivenmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbeventdrivenmodifier>> hkbeventdrivenmodifierList;
safeStringUMap<shared_ptr<hkbeventdrivenmodifier>> hkbeventdrivenmodifierList_E;

void hkbeventdrivenmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbeventdrivenmodifierList_E[id] = shared_from_this() : hkbeventdrivenmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbeventdrivenmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("modifier", line, output))
					{
						modifier = (isEdited ? hkbmodifierList_E : hkbmodifierList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					if (readParam("activateEventId", line, activateEventId)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("deactivateEventId", line, deactivateEventId)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("activeByDefault", line, activeByDefault)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << eventdrivenmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbeventdrivenmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + eventdrivenmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	activateEventId.connectEventInfo(ID, graphroot);
	deactivateEventId.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (modifier) threadedNextNode(modifier, filepath, curadd, functionlayer, graphroot);
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
				hkbeventdrivenmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbeventdrivenmodifierList_E[ID] = protect;
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
				hkbeventdrivenmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbeventdrivenmodifierList_E[ID] = protect;
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

string hkbeventdrivenmodifier::getClassCode()
{
	return eventdrivenmodifier::key;
}

void hkbeventdrivenmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(10);
	usize base = 2;
	hkbeventdrivenmodifier* ctrpart = static_cast<hkbeventdrivenmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, eventdrivenmodifier::classname, eventdrivenmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("modifier", modifier, ctrpart->modifier, output, storeline, base, false, open, isEdited);
	paramMatch("activateEventId", activateEventId, ctrpart->activateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("deactivateEventId", deactivateEventId, ctrpart->deactivateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("activeByDefault", activeByDefault, ctrpart->activeByDefault, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", eventdrivenmodifier::classname, output, isEdited);
}

void hkbeventdrivenmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, eventdrivenmodifier::classname, eventdrivenmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "modifier", modifier));
	output.push_back(autoParam(base, "activateEventId", activateEventId));
	output.push_back(autoParam(base, "deactivateEventId", deactivateEventId));
	output.push_back(autoParam(base, "activeByDefault", activeByDefault));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, eventdrivenmodifier::classname, output, true);
}

void hkbeventdrivenmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (modifier) hkb_parent[modifier] = shared_from_this();
}

void hkbeventdrivenmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (modifier)
	{
		parentRefresh();
		modifier->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbeventdrivenmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
