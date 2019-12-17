#include "hkbgetupmodifier.h"

using namespace std;

namespace getupmodifier
{
	const string key = "bo";
	const string classname = "hkbGetUpModifier";
	const string signature = "0x61cb7ac0";
}

string hkbgetupmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbgetupmodifier>> hkbgetupmodifierList;
safeStringUMap<shared_ptr<hkbgetupmodifier>> hkbgetupmodifierList_E;

void hkbgetupmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbgetupmodifierList_E[id] = shared_from_this() : hkbgetupmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbgetupmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("groundNormal", line, groundNormal)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("duration", line, duration)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("alignWithGroundDuration", line, alignWithGroundDuration)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("rootBoneIndex", line, rootBoneIndex)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("otherBoneIndex", line, otherBoneIndex)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("anotherBoneIndex", line, anotherBoneIndex)) ++type;
				}
			}
		}
		if (line.find("<hkparam name=\"") != string::npos)
		{
			for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
				itr != boost::sregex_iterator(); ++itr)
			{
				string header = itr->str(1);

				if (header == "variableBindingSet") variableBindingSet = isEdited ? hkbvariablebindingsetList_E[itr->str(2)] : hkbvariablebindingsetList[itr->str(2)];
				else if (header == "userData") userData = stoi(itr->str(2));
				else if (header == "name") name = itr->str(2);
				else if (header == "enable") enable = itr->str(2) == "true" ? true : false;
				else if (header == "groundNormal") groundNormal.update(itr->str(2));
				else if (header == "duration") duration = stod(itr->str(2));
				else if (header == "alignWithGroundDuration") alignWithGroundDuration = stod(itr->str(2));
				else if (header == "rootBoneIndex") rootBoneIndex = stoi(itr->str(2));
				else if (header == "otherBoneIndex") otherBoneIndex = stoi(itr->str(2));
				else if (header == "anotherBoneIndex") anotherBoneIndex = stoi(itr->str(2));

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << getupmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbgetupmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + getupmodifier::key + to_string(functionlayer) + ">";
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
				hkbgetupmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbgetupmodifierList_E[ID] = protect;
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
				hkbgetupmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbgetupmodifierList_E[ID] = protect;
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

string hkbgetupmodifier::getClassCode()
{
	return getupmodifier::key;
}

void hkbgetupmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(12);
	output.reserve(12);
	usize base = 2;
	hkbgetupmodifier* ctrpart = static_cast<hkbgetupmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, getupmodifier::classname, getupmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("groundNormal", groundNormal, ctrpart->groundNormal, output, storeline, base, false, open, isEdited);
	paramMatch("duration", duration, ctrpart->duration, output, storeline, base, false, open, isEdited);
	paramMatch("alignWithGroundDuration", alignWithGroundDuration, ctrpart->alignWithGroundDuration, output, storeline, base, false, open, isEdited);
	paramMatch("rootBoneIndex", rootBoneIndex, ctrpart->rootBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("otherBoneIndex", otherBoneIndex, ctrpart->otherBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("anotherBoneIndex", anotherBoneIndex, ctrpart->anotherBoneIndex, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", getupmodifier::classname, output, isEdited);
}

void hkbgetupmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(12);

	output.push_back(openObject(base, ID, getupmodifier::classname, getupmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "groundNormal", groundNormal));
	output.push_back(autoParam(base, "duration", duration));
	output.push_back(autoParam(base, "alignWithGroundDuration", alignWithGroundDuration));
	output.push_back(autoParam(base, "rootBoneIndex", rootBoneIndex));
	output.push_back(autoParam(base, "otherBoneIndex", otherBoneIndex));
	output.push_back(autoParam(base, "anotherBoneIndex", anotherBoneIndex));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, getupmodifier::classname, output, true);
}

void hkbgetupmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbgetupmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
