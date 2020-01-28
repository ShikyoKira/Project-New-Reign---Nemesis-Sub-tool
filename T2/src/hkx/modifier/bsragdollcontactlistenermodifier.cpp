#include <boost/thread.hpp>

#include "bsragdollcontactlistenermodifier.h"

using namespace std;

namespace ragdollcontactlistenermodifier
{
	const string key = "bb";
	const string classname = "BSRagdollContactListenerModifier";
	const string signature = "0x8003d8ce";
}

string bsragdollcontactlistenermodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsragdollcontactlistenermodifier>> bsragdollcontactlistenermodifierList;
safeStringUMap<shared_ptr<bsragdollcontactlistenermodifier>> bsragdollcontactlistenermodifierList_E;

void bsragdollcontactlistenermodifier::regis(string id, bool isEdited)
{
	isEdited ? bsragdollcontactlistenermodifierList_E[id] = shared_from_this() : bsragdollcontactlistenermodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsragdollcontactlistenermodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("id", line, event.id)) ++type;

					break;
				}
				case 5:
				{
					string output;

					if (readParam("payload", line, output))
					{
						event.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					string output;

					if (readParam("bones", line, output))
					{
						bones = (isEdited ? hkbboneindexarrayList_E : hkbboneindexarrayList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << ragdollcontactlistenermodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsragdollcontactlistenermodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + ragdollcontactlistenermodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	event.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (event.payload) threadedNextNode(event.payload, filepath, curadd, functionlayer, graphroot);

			if (bones) threadedNextNode(bones, filepath, curadd, functionlayer, graphroot);
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
				bsragdollcontactlistenermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsragdollcontactlistenermodifierList_E[ID] = protect;
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
				bsragdollcontactlistenermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsragdollcontactlistenermodifierList_E[ID] = protect;
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

string bsragdollcontactlistenermodifier::getClassCode()
{
	return ragdollcontactlistenermodifier::key;
}

void bsragdollcontactlistenermodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(13);
	output.reserve(13);
	usize base = 2;
	bsragdollcontactlistenermodifier* ctrpart = static_cast<bsragdollcontactlistenermodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, ragdollcontactlistenermodifier::classname, ragdollcontactlistenermodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "event"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", event.id, ctrpart->event.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", event.payload, ctrpart->event.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("anotherBoneIndex", bones, ctrpart->bones, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", ragdollcontactlistenermodifier::classname, output, isEdited);
}

void bsragdollcontactlistenermodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(13);

	output.push_back(openObject(base, ID, ragdollcontactlistenermodifier::classname, ragdollcontactlistenermodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "event"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", event.id));
	output.push_back(autoParam(base, "payload", event.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "bones", bones));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, ragdollcontactlistenermodifier::classname, output, true);
}

void bsragdollcontactlistenermodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (event.payload) hkb_parent[event.payload] = shared_from_this();
	if (bones) hkb_parent[bones] = shared_from_this();
}

void bsragdollcontactlistenermodifier::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (event.payload)
	{
		parentRefresh();
		event.payload->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (bones)
	{
		parentRefresh();
		bones->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bsragdollcontactlistenermodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
