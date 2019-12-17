#include "hkbdampingmodifier.h"

using namespace std;

namespace dampingmodifier
{
	const string key = "bm";
	const string classname = "hkbDampingModifier";
	const string signature = "0x9a040f03";
}

string hkbdampingmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbdampingmodifier>> hkbdampingmodifierList;
safeStringUMap<shared_ptr<hkbdampingmodifier>> hkbdampingmodifierList_E;

void hkbdampingmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbdampingmodifierList_E[id] = shared_from_this() : hkbdampingmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbdampingmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("kP", line, kP)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("kI", line, kI)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("kD", line, kD)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("enableScalarDamping", line, enableScalarDamping)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("enableVectorDamping", line, enableVectorDamping)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("rawValue", line, rawValue)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("dampedValue", line, dampedValue)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("rawVector", line, rawVector)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("dampedVector", line, dampedVector)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("vecErrorSum", line, vecErrorSum)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("vecPreviousError", line, vecPreviousError)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("errorSum", line, errorSum)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("previousError", line, previousError)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << dampingmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbdampingmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + dampingmodifier::key + to_string(functionlayer) + ">";
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
				hkbdampingmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbdampingmodifierList_E[ID] = protect;
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
				hkbdampingmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbdampingmodifierList_E[ID] = protect;
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

string hkbdampingmodifier::getClassCode()
{
	return dampingmodifier::key;
}

void hkbdampingmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(19);
	output.reserve(19);
	usize base = 2;
	hkbdampingmodifier* ctrpart = static_cast<hkbdampingmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, dampingmodifier::classname, dampingmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("kP", kP, ctrpart->kP, output, storeline, base, false, open, isEdited);
	paramMatch("kI", kI, ctrpart->kI, output, storeline, base, false, open, isEdited);
	paramMatch("kD", kD, ctrpart->kD, output, storeline, base, false, open, isEdited);
	paramMatch("enableScalarDamping", enableScalarDamping, ctrpart->enableScalarDamping, output, storeline, base, false, open, isEdited);
	paramMatch("enableVectorDamping", enableVectorDamping, ctrpart->enableVectorDamping, output, storeline, base, false, open, isEdited);
	paramMatch("rawValue", rawValue, ctrpart->rawValue, output, storeline, base, false, open, isEdited);
	paramMatch("dampedValue", dampedValue, ctrpart->dampedValue, output, storeline, base, false, open, isEdited);
	paramMatch("rawVector", rawVector, ctrpart->rawVector, output, storeline, base, false, open, isEdited);
	paramMatch("dampedVector", dampedVector, ctrpart->dampedVector, output, storeline, base, false, open, isEdited);
	paramMatch("vecErrorSum", vecErrorSum, ctrpart->vecErrorSum, output, storeline, base, false, open, isEdited);
	paramMatch("vecPreviousError", vecPreviousError, ctrpart->vecPreviousError, output, storeline, base, false, open, isEdited);
	paramMatch("errorSum", errorSum, ctrpart->errorSum, output, storeline, base, false, open, isEdited);
	paramMatch("previousError", previousError, ctrpart->previousError, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", dampingmodifier::classname, output, isEdited);
}

void hkbdampingmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(19);

	output.push_back(openObject(base, ID, dampingmodifier::classname, dampingmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "kP", kP));
	output.push_back(autoParam(base, "kI", kI));
	output.push_back(autoParam(base, "kD", kD));
	output.push_back(autoParam(base, "enableScalarDamping", enableScalarDamping));
	output.push_back(autoParam(base, "enableVectorDamping", enableVectorDamping));
	output.push_back(autoParam(base, "rawValue", rawValue));
	output.push_back(autoParam(base, "dampedValue", dampedValue));
	output.push_back(autoParam(base, "rawVector", rawVector));
	output.push_back(autoParam(base, "dampedVector", dampedVector));
	output.push_back(autoParam(base, "vecErrorSum", vecErrorSum));
	output.push_back(autoParam(base, "vecPreviousError", vecPreviousError));
	output.push_back(autoParam(base, "errorSum", errorSum));
	output.push_back(autoParam(base, "previousError", previousError));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, dampingmodifier::classname, output, true);
}

void hkbdampingmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbdampingmodifier::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
