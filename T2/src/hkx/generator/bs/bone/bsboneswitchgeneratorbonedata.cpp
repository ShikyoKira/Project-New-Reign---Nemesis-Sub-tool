#include <boost\thread.hpp>
#include "bsboneswitchgeneratorbonedata.h"

using namespace std;

namespace boneswitchgeneratorbonedata
{
	const string key = "ct";
	const string classname = "BSBoneSwitchGeneratorBoneData";
	const string signature = "0xc1215be6";
}

string bsboneswitchgeneratorbonedata::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsboneswitchgeneratorbonedata>> bsboneswitchgeneratorbonedataList;
safeStringUMap<shared_ptr<bsboneswitchgeneratorbonedata>> bsboneswitchgeneratorbonedataList_E;

void bsboneswitchgeneratorbonedata::regis(string id, bool isEdited)
{
	isEdited ? bsboneswitchgeneratorbonedataList_E[id] = shared_from_this() : bsboneswitchgeneratorbonedataList[id] = shared_from_this();
	ID = id;
}

void bsboneswitchgeneratorbonedata::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("pGenerator", line, output))
					{
						pGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 2:
				{
					string output;

					if (readParam("spBoneWeight", line, output))
					{
						spBoneWeight = (isEdited ? hkbboneweightarrayList_E : hkbboneweightarrayList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << boneswitchgeneratorbonedata::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsboneswitchgeneratorbonedata::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + boneswitchgeneratorbonedata::key + to_string(functionlayer) + ">";
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

			if (pGenerator) threadedNextNode(pGenerator, filepath, curadd, functionlayer, graphroot);

			if (spBoneWeight) threadedNextNode(spBoneWeight, filepath, curadd, functionlayer, graphroot);
		}
		else
		{
			// existed
			if (addressID.find(address) != addressID.end() && IsForeign.find(parent[ID]) == IsForeign.end())
			{
				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				bsboneswitchgeneratorbonedataList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsboneswitchgeneratorbonedataList_E[ID] = protect;
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
				bsboneswitchgeneratorbonedataList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsboneswitchgeneratorbonedataList_E[ID] = protect;
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

string bsboneswitchgeneratorbonedata::getClassCode()
{
	return boneswitchgeneratorbonedata::key;
}

void bsboneswitchgeneratorbonedata::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(5);
	output.reserve(5);
	usize base = 2;
	bsboneswitchgeneratorbonedata* ctrpart = static_cast<bsboneswitchgeneratorbonedata*>(counterpart.get());

	output.push_back(openObject(base, ID, boneswitchgeneratorbonedata::classname, boneswitchgeneratorbonedata::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("pGenerator", pGenerator, ctrpart->pGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("spBoneWeight", spBoneWeight, ctrpart->spBoneWeight, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", boneswitchgeneratorbonedata::classname, output, isEdited);
}

void bsboneswitchgeneratorbonedata::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(5);

	output.push_back(openObject(base, ID, boneswitchgeneratorbonedata::classname, boneswitchgeneratorbonedata::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "pGenerator", pGenerator));
	output.push_back(autoParam(base, "spBoneWeight", spBoneWeight));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, boneswitchgeneratorbonedata::classname, output, true);
}

void bsboneswitchgeneratorbonedata::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pGenerator) hkb_parent[pGenerator] = shared_from_this();
	if (spBoneWeight) hkb_parent[spBoneWeight] = shared_from_this();
}

void bsboneswitchgeneratorbonedata::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pGenerator)
	{
		parentRefresh();
		pGenerator->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (spBoneWeight)
	{
		parentRefresh();
		spBoneWeight->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bsboneswitchgeneratorbonedata::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
