#include <boost\thread.hpp>
#include "bssynchronizedclipgenerator.h"

using namespace std;

namespace synchronizedclipgenerator
{
	const string key = "ca";
	const string classname = "BSSynchronizedClipGenerator";
	const string signature = "0xd83bea64";
}

string bssynchronizedclipgenerator::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bssynchronizedclipgenerator>> bssynchronizedclipgeneratorList;
safeStringUMap<shared_ptr<bssynchronizedclipgenerator>> bssynchronizedclipgeneratorList_E;

void bssynchronizedclipgenerator::regis(string id, bool isEdited)
{
	isEdited ? bssynchronizedclipgeneratorList_E[id] = shared_from_this() : bssynchronizedclipgeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void bssynchronizedclipgenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("pClipGenerator", line, output))
					{
						pClipGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 4:
				{
					if (readParam("SyncAnimPrefix", line, SyncAnimPrefix)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("bSyncClipIgnoreMarkPlacement", line, bSyncClipIgnoreMarkPlacement)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("fGetToMarkTime", line, fGetToMarkTime)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("fMarkErrorThreshold", line, fMarkErrorThreshold)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("bLeadCharacter", line, bLeadCharacter)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("bReorientSupportChar", line, bReorientSupportChar)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("bApplyMotionFromRoot", line, bApplyMotionFromRoot)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("sAnimationBindingIndex", line, sAnimationBindingIndex)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << synchronizedclipgenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bssynchronizedclipgenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + synchronizedclipgenerator::key + to_string(functionlayer) + ">";
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

			if (pClipGenerator) threadedNextNode(pClipGenerator, filepath, curadd, functionlayer, graphroot);
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
				bssynchronizedclipgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bssynchronizedclipgeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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
				bssynchronizedclipgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bssynchronizedclipgeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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

string bssynchronizedclipgenerator::getClassCode()
{
	return synchronizedclipgenerator::key;
}

void bssynchronizedclipgenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(14);
	output.reserve(14);
	usize base = 2;
	bssynchronizedclipgenerator* ctrpart = static_cast<bssynchronizedclipgenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, synchronizedclipgenerator::classname, synchronizedclipgenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("pClipGenerator", pClipGenerator, ctrpart->pClipGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("SyncAnimPrefix", SyncAnimPrefix, ctrpart->SyncAnimPrefix, output, storeline, base, false, open, isEdited);
	paramMatch("bSyncClipIgnoreMarkPlacement", bSyncClipIgnoreMarkPlacement, ctrpart->bSyncClipIgnoreMarkPlacement, output, storeline, base, false, open, isEdited);
	paramMatch("fGetToMarkTime", fGetToMarkTime, ctrpart->fGetToMarkTime, output, storeline, base, false, open, isEdited);
	paramMatch("fMarkErrorThreshold", fMarkErrorThreshold, ctrpart->fMarkErrorThreshold, output, storeline, base, false, open, isEdited);
	paramMatch("bLeadCharacter", bLeadCharacter, ctrpart->bLeadCharacter, output, storeline, base, false, open, isEdited);
	paramMatch("bReorientSupportChar", bReorientSupportChar, ctrpart->bReorientSupportChar, output, storeline, base, false, open, isEdited);
	paramMatch("bApplyMotionFromRoot", bApplyMotionFromRoot, ctrpart->bApplyMotionFromRoot, output, storeline, base, false, open, isEdited);
	paramMatch("sAnimationBindingIndex", sAnimationBindingIndex, ctrpart->sAnimationBindingIndex, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", synchronizedclipgenerator::classname, output, isEdited);
}

void bssynchronizedclipgenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(14);

	output.push_back(openObject(base, ID, synchronizedclipgenerator::classname, synchronizedclipgenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "pClipGenerator", pClipGenerator));
	output.push_back(autoParam(base, "SyncAnimPrefix", SyncAnimPrefix));
	output.push_back(autoParam(base, "bSyncClipIgnoreMarkPlacement", bSyncClipIgnoreMarkPlacement));
	output.push_back(autoParam(base, "fGetToMarkTime", fGetToMarkTime));
	output.push_back(autoParam(base, "fMarkErrorThreshold", fMarkErrorThreshold));
	output.push_back(autoParam(base, "bLeadCharacter", bLeadCharacter));
	output.push_back(autoParam(base, "bReorientSupportChar", bReorientSupportChar));
	output.push_back(autoParam(base, "bApplyMotionFromRoot", bApplyMotionFromRoot));
	output.push_back(autoParam(base, "sAnimationBindingIndex", sAnimationBindingIndex));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, synchronizedclipgenerator::classname, output, true);
}

void bssynchronizedclipgenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pClipGenerator) hkb_parent[pClipGenerator] = shared_from_this();
}

void bssynchronizedclipgenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pClipGenerator)
	{
		parentRefresh();
		pClipGenerator->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bssynchronizedclipgenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
