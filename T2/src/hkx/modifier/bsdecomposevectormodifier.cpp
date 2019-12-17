#include "bsdecomposevectormodifier.h"

using namespace std;

namespace decomposevectormodifier
{
	const string key = "bg";
	const string classname = "BSDecomposeVectorModifier";
	const string signature = "0x31f6b8b6";
}

string bsdecomposevectormodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsdecomposevectormodifier>> bsdecomposevectormodifierList;
safeStringUMap<shared_ptr<bsdecomposevectormodifier>> bsdecomposevectormodifierList_E;

void bsdecomposevectormodifier::regis(string id, bool isEdited)
{
	isEdited ? bsdecomposevectormodifierList_E[id] = shared_from_this() : bsdecomposevectormodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsdecomposevectormodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("vector", line, vector)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("bSyncClipIxgnoreMarkPlacement", line, x)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("y", line, y)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("z", line, z)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("w", line, w)) ++type;
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
				else if (header == "vector") vector.update(itr->str(2));
				else if (header == "x") x = stod(itr->str(2));
				else if (header == "y") y = stod(itr->str(2));
				else if (header == "z") z = stod(itr->str(2));
				else if (header == "w") w = stod(itr->str(2));

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << decomposevectormodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsdecomposevectormodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + decomposevectormodifier::key + to_string(functionlayer) + ">";
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
				bsdecomposevectormodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsdecomposevectormodifierList_E[ID] = protect;
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
				bsdecomposevectormodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsdecomposevectormodifierList_E[ID] = protect;
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

string bsdecomposevectormodifier::getClassCode()
{
	return decomposevectormodifier::key;
}

void bsdecomposevectormodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(11);
	output.reserve(11);
	usize base = 2;
	bsdecomposevectormodifier* ctrpart = static_cast<bsdecomposevectormodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, decomposevectormodifier::classname, decomposevectormodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("vector", vector, ctrpart->vector, output, storeline, base, false, open, isEdited);
	paramMatch("x", x, ctrpart->x, output, storeline, base, false, open, isEdited);
	paramMatch("y", y, ctrpart->y, output, storeline, base, false, open, isEdited);
	paramMatch("z", z, ctrpart->z, output, storeline, base, false, open, isEdited);
	paramMatch("w", w, ctrpart->w, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", decomposevectormodifier::classname, output, isEdited);
}

void bsdecomposevectormodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(11);

	output.push_back(openObject(base, ID, decomposevectormodifier::classname, decomposevectormodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "vector", vector));
	output.push_back(autoParam(base, "x", x));
	output.push_back(autoParam(base, "y", y));
	output.push_back(autoParam(base, "z", z));
	output.push_back(autoParam(base, "w", w));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, decomposevectormodifier::classname, output, true);
}

void bsdecomposevectormodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void bsdecomposevectormodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
