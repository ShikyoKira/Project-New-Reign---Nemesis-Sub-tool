#include <boost\thread.hpp>
#include "hkbdelayedmodifier.h"

using namespace std;

namespace delayedmodifier
{
	const string key = "bn";
	const string classname = "hkbDelayedModifier";
	const string signature = "0x8e101a7a";
}

string hkbdelayedmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbdelayedmodifier>> hkbdelayedmodifierList;
safeStringUMap<shared_ptr<hkbdelayedmodifier>> hkbdelayedmodifierList_E;

void hkbdelayedmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbdelayedmodifierList_E[id] = shared_from_this() : hkbdelayedmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbdelayedmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("delaySeconds", line, delaySeconds)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("durationSeconds", line, durationSeconds)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("secondsElapsed", line, secondsElapsed)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << delayedmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbdelayedmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + delayedmodifier::key + to_string(functionlayer) + ">";
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
				hkbdelayedmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbdelayedmodifierList_E[ID] = protect;
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
				hkbdelayedmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbdelayedmodifierList_E[ID] = protect;
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

string hkbdelayedmodifier::getClassCode()
{
	return delayedmodifier::key;
}

void hkbdelayedmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(10);
	usize base = 2;
	hkbdelayedmodifier* ctrpart = static_cast<hkbdelayedmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, delayedmodifier::classname, delayedmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("groundNormal", modifier, ctrpart->modifier, output, storeline, base, false, open, isEdited);
	paramMatch("duration", delaySeconds, ctrpart->delaySeconds, output, storeline, base, false, open, isEdited);
	paramMatch("durationSeconds", durationSeconds, ctrpart->durationSeconds, output, storeline, base, false, open, isEdited);
	paramMatch("secondsElapsed", secondsElapsed, ctrpart->secondsElapsed, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", delayedmodifier::classname, output, isEdited);
}

void hkbdelayedmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, delayedmodifier::classname, delayedmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "modifier", modifier));
	output.push_back(autoParam(base, "delaySeconds", delaySeconds));
	output.push_back(autoParam(base, "durationSeconds", durationSeconds));
	output.push_back(autoParam(base, "secondsElapsed", secondsElapsed));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, delayedmodifier::classname, output, true);
}

void hkbdelayedmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (modifier) hkb_parent[modifier] = shared_from_this();
}

void hkbdelayedmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

void hkbdelayedmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
