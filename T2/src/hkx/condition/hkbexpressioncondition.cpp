#include "hkbexpressioncondition.h"

using namespace std;

namespace expressioncondition
{
	const string key = "l";
	const string classname = "hkbExpressionCondition";
	const string signature = "0x1c3c1045";
}

string hkbexpressioncondition::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbexpressioncondition>> hkbexpressionconditionList;
safeStringUMap<shared_ptr<hkbexpressioncondition>> hkbexpressionconditionList_E;

void hkbexpressioncondition::regis(string id, bool isEdited)
{
	isEdited ? hkbexpressionconditionList_E[id] = shared_from_this() : hkbexpressionconditionList[id] = shared_from_this();
	isEdited ? hkbconditionList_E[id] = shared_from_this() : hkbconditionList[id] = shared_from_this();
	ID = id;
}

void hkbexpressioncondition::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	for (auto& line : nodelines)
	{
		if (readParam("expression", line, expression)) break;
	}

	if ((Debug) && (!Error))
	{
		cout << expressioncondition::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbexpressioncondition::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + expressioncondition::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			string line;
			int backCounter = preaddress.length() - 1;

			while (isdigit(preaddress[backCounter]))
			{
				line = preaddress[backCounter] + line;
				--backCounter;
			}

			line = line + "l" + to_string(functionlayer);

			// existed
			if (conditionOldFunction(shared_from_this(), address, functionlayer, line, false) != "") // is this new function or old for non generator
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbexpressionconditionList_E.erase(ID);
				hkbconditionList_E.erase(ID);
				editedBehavior.erase(ID);
				previousID = ID;
				ID = addressID[address];
				hkbexpressionconditionList_E[ID] = protect;
				hkbconditionList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, true);
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
				hkbexpressionconditionList_E.erase(ID);
				hkbconditionList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				previousID = ID;
				ID = addressID[address];
				hkbexpressionconditionList_E[ID] = protect;
				hkbconditionList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
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

string hkbexpressioncondition::getClassCode()
{
	return expressioncondition::key;
}

void hkbexpressioncondition::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(3);
	output.reserve(3);
	usize base = 2;
	hkbexpressioncondition* ctrpart = static_cast<hkbexpressioncondition*>(counterpart.get());

	output.push_back(openObject(base, ID, expressioncondition::classname, expressioncondition::signature));		// 1
	paramMatch("expression", expression, ctrpart->expression, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", expressioncondition::classname, output, isEdited);
}

void hkbexpressioncondition::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(3);

	output.push_back(openObject(base, ID, expressioncondition::classname, expressioncondition::signature));		// 1
	output.push_back(autoParam(base, "expression", expression));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, expressioncondition::classname, output, true);
}
