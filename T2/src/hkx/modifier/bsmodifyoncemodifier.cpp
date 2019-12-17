#include <boost\thread.hpp>
#include "bsmodifyoncemodifier.h"

using namespace std;

namespace modifyoncemodifier
{
	const string key = "ay";
	const string classname = "BSModifyOnceModifier";
	const string signature = "0x1e20a97a";
}

string bsmodifyoncemodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsmodifyoncemodifier>> bsmodifyoncemodifierList;
safeStringUMap<shared_ptr<bsmodifyoncemodifier>> bsmodifyoncemodifierList_E;

void bsmodifyoncemodifier::regis(string id, bool isEdited)
{
	isEdited ? bsmodifyoncemodifierList_E[id] = shared_from_this() : bsmodifyoncemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsmodifyoncemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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

					if (readParam("pOnActivateModifier", line, output))
					{
						pOnActivateModifier = (isEdited ? hkbmodifierList_E : hkbmodifierList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("pOnDeactivateModifier", line, output))
					{
						pOnDeactivateModifier = (isEdited ? hkbmodifierList_E : hkbmodifierList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << modifyoncemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsmodifyoncemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + modifyoncemodifier::key + to_string(functionlayer) + ">";
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

			if (pOnActivateModifier) threadedNextNode(pOnActivateModifier, filepath, curadd + "0", functionlayer, graphroot);

			if (pOnDeactivateModifier) threadedNextNode(pOnDeactivateModifier, filepath, curadd + "1", functionlayer, graphroot);
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
				bsmodifyoncemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsmodifyoncemodifierList_E[ID] = protect;
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
			bsmodifyoncemodifierList_E.erase(ID);
			hkbmodifierList_E.erase(ID);
			editedBehavior.erase(ID);
			IsExist.erase(ID);
			ID = addressID[address];
			bsmodifyoncemodifierList_E[ID] = protect;
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

string bsmodifyoncemodifier::getClassCode()
{
	return modifyoncemodifier::key;
}

void bsmodifyoncemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(8);
	output.reserve(8);
	usize base = 2;
	bsmodifyoncemodifier* ctrpart = static_cast<bsmodifyoncemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, modifyoncemodifier::classname, modifyoncemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("pOnActivateModifier", pOnActivateModifier, ctrpart->pOnActivateModifier, output, storeline, base, false, open, isEdited);
	paramMatch("pOnDeactivateModifier", pOnDeactivateModifier, ctrpart->pOnDeactivateModifier, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", modifyoncemodifier::classname, output, isEdited);
}

void bsmodifyoncemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(8);

	output.push_back(openObject(base, ID, modifyoncemodifier::classname, modifyoncemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "pOnActivateModifier", pOnActivateModifier));
	output.push_back(autoParam(base, "pOnDeactivateModifier", pOnDeactivateModifier));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, modifyoncemodifier::classname, output, true);
}

void bsmodifyoncemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pOnActivateModifier) hkb_parent[pOnActivateModifier] = shared_from_this();
	if (pOnDeactivateModifier) hkb_parent[pOnDeactivateModifier] = shared_from_this();
}

void bsmodifyoncemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pOnActivateModifier)
	{
		parentRefresh();
		pOnActivateModifier->connect(filepath, isOld ? address + "0" : address, functionlayer, true, graphroot);
	}

	if (pOnDeactivateModifier)
	{
		parentRefresh();
		pOnDeactivateModifier->connect(filepath, isOld ? address + "1" : address, functionlayer, true, graphroot);
	}
}

void bsmodifyoncemodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
