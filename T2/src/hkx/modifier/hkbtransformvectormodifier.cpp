#include "hkbtransformvectormodifier.h"

using namespace std;

namespace transformvectormodifier
{
	const string key = "cp";
	const string classname = "hkbTransformVectorModifier";
	const string signature = "0xf93e0e24";
}

string hkbtransformvectormodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbtransformvectormodifier>> hkbtransformvectormodifierList;
safeStringUMap<shared_ptr<hkbtransformvectormodifier>> hkbtransformvectormodifierList_E;

void hkbtransformvectormodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbtransformvectormodifierList_E[id] = shared_from_this() : hkbtransformvectormodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbtransformvectormodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("rotation", line, rotation)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("translation", line, translation)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("vectorIn", line, vectorIn)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("vectorOut", line, vectorOut)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("rotateOnly", line, rotateOnly)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("inverse", line, inverse)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("computeOnActivate", line, computeOnActivate)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("computeOnModify", line, computeOnModify)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << transformvectormodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbtransformvectormodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + transformvectormodifier::key + to_string(functionlayer) + ">";
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
				hkbtransformvectormodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbtransformvectormodifierList_E[ID] = protect;
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
				hkbtransformvectormodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbtransformvectormodifierList_E[ID] = protect;
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

string hkbtransformvectormodifier::getClassCode()
{
	return transformvectormodifier::key;
}

void hkbtransformvectormodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(14);
	usize base = 2;
	hkbtransformvectormodifier* ctrpart = static_cast<hkbtransformvectormodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, transformvectormodifier::classname, transformvectormodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("rotation", rotation, ctrpart->rotation, output, storeline, base, false, open, isEdited);
	paramMatch("translation", translation, ctrpart->translation, output, storeline, base, false, open, isEdited);
	paramMatch("vectorIn", vectorIn, ctrpart->vectorIn, output, storeline, base, false, open, isEdited);
	paramMatch("vectorOut", vectorOut, ctrpart->vectorOut, output, storeline, base, false, open, isEdited);
	paramMatch("rotateOnly", rotateOnly, ctrpart->rotateOnly, output, storeline, base, false, open, isEdited);
	paramMatch("inverse", inverse, ctrpart->inverse, output, storeline, base, false, open, isEdited);
	paramMatch("computeOnActivate", computeOnActivate, ctrpart->computeOnActivate, output, storeline, base, false, open, isEdited);
	paramMatch("computeOnModify", computeOnModify, ctrpart->computeOnModify, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", transformvectormodifier::classname, output, isEdited);
}

void hkbtransformvectormodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(14);

	output.push_back(openObject(base, ID, transformvectormodifier::classname, transformvectormodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "rotation", rotation));
	output.push_back(autoParam(base, "translation", translation));
	output.push_back(autoParam(base, "vectorIn", vectorIn));
	output.push_back(autoParam(base, "vectorOut", vectorOut));
	output.push_back(autoParam(base, "rotateOnly", rotateOnly));
	output.push_back(autoParam(base, "inverse", inverse));
	output.push_back(autoParam(base, "computeOnActivate", computeOnActivate));
	output.push_back(autoParam(base, "computeOnModify", computeOnModify));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, transformvectormodifier::classname, output, true);
}

void hkbtransformvectormodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbtransformvectormodifier::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
