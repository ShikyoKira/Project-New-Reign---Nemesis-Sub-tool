#include <boost\thread.hpp>
#include "bspassbytargettriggermodifier.h"

using namespace std;

namespace passbytargettriggermodifier
{
	const string key = "bx";
	const string classname = "BSPassByTargetTriggerModifier";
	const string signature = "0x703d7b66";
}

string bspassbytargettriggermodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bspassbytargettriggermodifier>> bspassbytargettriggermodifierList;
safeStringUMap<shared_ptr<bspassbytargettriggermodifier>> bspassbytargettriggermodifierList_E;

void bspassbytargettriggermodifier::regis(string id, bool isEdited)
{
	isEdited ? bspassbytargettriggermodifierList_E[id] = shared_from_this() : bspassbytargettriggermodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bspassbytargettriggermodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("targetPosition", line, targetPosition)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("radius", line, radius)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("movementDirection", line, movementDirection)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("id", line, triggerEvent.id)) ++type;

					break;
				}
				case 8:
				{
					string output;

					if (readParam("payload", line, output))
					{
						triggerEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << passbytargettriggermodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bspassbytargettriggermodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + passbytargettriggermodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	triggerEvent.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (triggerEvent.payload) threadedNextNode(triggerEvent.payload, filepath, curadd, functionlayer, graphroot);
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
				bspassbytargettriggermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bspassbytargettriggermodifierList_E[ID] = protect;
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
				bspassbytargettriggermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bspassbytargettriggermodifierList_E[ID] = protect;
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

string bspassbytargettriggermodifier::getClassCode()
{
	return passbytargettriggermodifier::key;
}

void bspassbytargettriggermodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(15);
	output.reserve(15);
	usize base = 2;
	bspassbytargettriggermodifier* ctrpart = static_cast<bspassbytargettriggermodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, passbytargettriggermodifier::classname, passbytargettriggermodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("targetPosition", targetPosition, ctrpart->targetPosition, output, storeline, base, false, open, isEdited);
	paramMatch("radius", radius, ctrpart->radius, output, storeline, base, false, open, isEdited);
	paramMatch("movementDirection", movementDirection, ctrpart->movementDirection, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "triggerEvent"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", triggerEvent.id, ctrpart->triggerEvent.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", triggerEvent.payload, ctrpart->triggerEvent.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", passbytargettriggermodifier::classname, output, isEdited);
}

void bspassbytargettriggermodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(15);

	output.push_back(openObject(base, ID, passbytargettriggermodifier::classname, passbytargettriggermodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "targetPosition", targetPosition));
	output.push_back(autoParam(base, "radius", radius));
	output.push_back(autoParam(base, "movementDirection", movementDirection));
	output.push_back(openParam(base, "triggerEvent"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", triggerEvent.id));
	output.push_back(autoParam(base, "payload", triggerEvent.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, passbytargettriggermodifier::classname, output, true);
}

void bspassbytargettriggermodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (triggerEvent.payload) hkb_parent[triggerEvent.payload] = shared_from_this();
}

void bspassbytargettriggermodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (triggerEvent.payload)
	{
		parentRefresh();
		triggerEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bspassbytargettriggermodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
