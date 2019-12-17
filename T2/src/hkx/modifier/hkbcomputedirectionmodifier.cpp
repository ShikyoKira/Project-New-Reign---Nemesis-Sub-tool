#include "hkbcomputedirectionmodifier.h"

using namespace std;

namespace computedirectionmodifier
{
	const string key = "br";
	const string classname = "hkbComputeDirectionModifier";
	const string signature = "0xdf358bd3";
}

string hkbcomputedirectionmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbcomputedirectionmodifier>> hkbcomputedirectionmodifierList;
safeStringUMap<shared_ptr<hkbcomputedirectionmodifier>> hkbcomputedirectionmodifierList_E;

void hkbcomputedirectionmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbcomputedirectionmodifierList_E[id] = shared_from_this() : hkbcomputedirectionmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbcomputedirectionmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("pointIn", line, pointIn)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("pointOut", line, pointOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("groundAngleOut", line, groundAngleOut)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("upAngleOut", line, upAngleOut)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("verticalOffset", line, verticalOffset)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("reverseGroundAngle", line, reverseGroundAngle)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("reverseUpAngle", line, reverseUpAngle)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("projectPoint", line, projectPoint)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("normalizePoint", line, normalizePoint)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("computeOnlyOnce", line, computeOnlyOnce)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("computedOutput", line, computedOutput)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << computedirectionmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcomputedirectionmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + computedirectionmodifier::key + to_string(functionlayer) + ">";
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
				hkbcomputedirectionmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbcomputedirectionmodifierList_E[ID] = protect;
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
				hkbcomputedirectionmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbcomputedirectionmodifierList_E[ID] = protect;
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

string hkbcomputedirectionmodifier::getClassCode()
{
	return computedirectionmodifier::key;
}

void hkbcomputedirectionmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(17);
	output.reserve(17);
	usize base = 2;
	hkbcomputedirectionmodifier* ctrpart = static_cast<hkbcomputedirectionmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, computedirectionmodifier::classname, computedirectionmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("pointIn", pointIn, ctrpart->pointIn, output, storeline, base, false, open, isEdited);
	paramMatch("pointOut", pointOut, ctrpart->pointOut, output, storeline, base, false, open, isEdited);
	paramMatch("groundAngleOut", groundAngleOut, ctrpart->groundAngleOut, output, storeline, base, false, open, isEdited);
	paramMatch("upAngleOut", upAngleOut, ctrpart->upAngleOut, output, storeline, base, false, open, isEdited);
	paramMatch("verticalOffset", verticalOffset, ctrpart->verticalOffset, output, storeline, base, false, open, isEdited);
	paramMatch("reverseGroundAngle", reverseGroundAngle, ctrpart->reverseGroundAngle, output, storeline, base, false, open, isEdited);
	paramMatch("reverseUpAngle", reverseUpAngle, ctrpart->reverseUpAngle, output, storeline, base, false, open, isEdited);
	paramMatch("projectPoint", projectPoint, ctrpart->projectPoint, output, storeline, base, false, open, isEdited);
	paramMatch("normalizePoint", normalizePoint, ctrpart->normalizePoint, output, storeline, base, false, open, isEdited);
	paramMatch("computeOnlyOnce", computeOnlyOnce, ctrpart->computeOnlyOnce, output, storeline, base, false, open, isEdited);
	paramMatch("computedOutput", computedOutput, ctrpart->computedOutput, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", computedirectionmodifier::classname, output, isEdited);
}

void hkbcomputedirectionmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(17);

	output.push_back(openObject(base, ID, computedirectionmodifier::classname, computedirectionmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "pointIn", pointIn));
	output.push_back(autoParam(base, "pointOut", pointOut));
	output.push_back(autoParam(base, "groundAngleOut", groundAngleOut));
	output.push_back(autoParam(base, "upAngleOut", upAngleOut));
	output.push_back(autoParam(base, "verticalOffset", verticalOffset));
	output.push_back(autoParam(base, "reverseGroundAngle", reverseGroundAngle));
	output.push_back(autoParam(base, "reverseUpAngle", reverseUpAngle));
	output.push_back(autoParam(base, "projectPoint", projectPoint));
	output.push_back(autoParam(base, "normalizePoint", normalizePoint));
	output.push_back(autoParam(base, "computeOnlyOnce", computeOnlyOnce));
	output.push_back(autoParam(base, "computedOutput", computedOutput));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, computedirectionmodifier::classname, output, true);
}

void hkbcomputedirectionmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbcomputedirectionmodifier::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
