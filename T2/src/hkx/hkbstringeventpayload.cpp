#include "hkbstringeventpayload.h"
#include "Global.h"

using namespace std;

namespace stringeventpayload
{
	const string key = "s";
	const string classname = "hkbStringEventPayload";
	const string signature = "0xed04256a";
}

string hkbstringeventpayload::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbstringeventpayload>> hkbstringeventpayloadList;
safeStringUMap<shared_ptr<hkbstringeventpayload>> hkbstringeventpayloadList_E;

void hkbstringeventpayload::regis(string id, bool isEdited)
{
	isEdited ? hkbstringeventpayloadList_E[id] = shared_from_this() : hkbstringeventpayloadList[id] = shared_from_this();
	ID = id;
}

void hkbstringeventpayload::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	for (auto& line : nodelines)
	{
		if (readParam("data", line, data)) break;
	}

	if ((Debug) && (!Error))
	{
		cout << stringeventpayload::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstringeventpayload::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + stringeventpayload::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			auto parent_itr = hkb_parent.find(shared_from_this());

			// existed
			if (addressID.find(address) != addressID.end() && parent_itr != hkb_parent.end() && IsForeign.find(parent_itr->second->ID) == IsForeign.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbstringeventpayloadList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbstringeventpayloadList_E[ID] = protect;
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
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare, true);

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
				hkbstringeventpayloadList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbstringeventpayloadList_E[ID] = protect;
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

string hkbstringeventpayload::getClassCode()
{
	return stringeventpayload::key;
}

void hkbstringeventpayload::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbstringeventpayload* ctrpart = static_cast<hkbstringeventpayload*>(counterpart.get());

	output.push_back(openObject(base, ID, stringeventpayload::classname, stringeventpayload::signature));		// 1
	paramMatch("data", data, ctrpart->data, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", stringeventpayload::classname, output, isEdited);
}

void hkbstringeventpayload::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, stringeventpayload::classname, stringeventpayload::signature));		// 1
	output.push_back(autoParam(base, "data", data));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, stringeventpayload::classname, output, true);
}
