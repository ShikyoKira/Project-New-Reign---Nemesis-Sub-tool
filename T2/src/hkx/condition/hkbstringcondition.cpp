#include "hkbstringcondition.h"
#include "Global.h"

using namespace std;

namespace stringcondition
{
	string key = "ck";
	string classname = "hkbStringCondition";
	string signature = "0x5ab50487";
}

hkbstringcondition::hkbstringcondition(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + stringcondition::key + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id, functionlayer);
		}
		else
		{
			nonCompare(filepath, id);
		}
	}
	else if (!Error)
	{
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare, true);

		if (addressChange.find(address) != addressChange.end())
		{
			string tempAdd = addressChange[address];
			addressChange.erase(address);
			address = tempAdd;

			IsForeign[id] = false;
			exchangeID[id] = dummyID;

			if ((Debug) && (!Error))
			{
				cout << "Comparing hkbStringCondition (newID: " << id << ") with hkbStringCondition (oldID: " << dummyID << ")" << endl;
			}

			FunctionLineNew[dummyID].push_back(FunctionLineTemp[dummyID][0]);

			for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
			{
				FunctionLineNew[dummyID].push_back(FunctionLineEdited[id][i]);
			}

			if ((Debug) && (!Error))
			{
				cout << "Comparing hkbStringCondition (newID: " << id << ") with hkbStringCondition (oldID: " << dummyID << ") is complete!" << endl;
			}

			RecordID(id, address, true);

			if ((Debug) && (!Error))
			{
				cout << "hkbStringCondition (ID: " << id << ") is complete!" << endl;
			}

			Dummy(dummyID);
		}
		else
		{
			if (compare)
			{
				if (statusChange)
				{
					Dummy(dummyID);
				}

				if (IsForeign[id])
				{
					address = preaddress;
				}
				else if (!statusChange)
				{
					IsNegated = true;
				}
			}
			else
			{
				IsNegated = true;
			}
		}
	}
	else
	{
		return;
	}
}

void hkbstringcondition::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStringCondition (ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineOriginal[id].empty())
	{
		cout << "ERROR: hkbStringCondition Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStringCondition (ID: " << id << ") is complete!" << endl;
	}
}

void hkbstringcondition::Compare(string filepath, string id, int functionlayer)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStringCondition (ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbStringCondition Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}
	
	string line;
	int backCounter = tempaddress.length() - 1;

	while (isdigit(tempaddress[backCounter]))
	{
		line = tempaddress[backCounter] + line;
		--backCounter;
	}

	line = line + stringcondition::key + to_string(functionlayer);
	
	// stage 2
	if (conditionOldFunction(id, address, functionlayer, line, true) != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStringCondition (newID: " << id << ") with hkbStringCondition (oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
		{
			FunctionLineNew[tempid].push_back(FunctionLineEdited[id][i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStringCondition (newID: " << id << ") with hkbStringCondition (oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = FunctionLineEdited[id];
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStringCondition (ID: " << id << ") is complete!" << endl;
	}
}

void hkbstringcondition::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStringCondition (ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbStringCondition Inputfile (ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStringCondition (ID: " << id << ") is complete!" << endl;
	}
}

string hkbstringcondition::GetAddress()
{
	return address;
}

bool hkbstringcondition::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbstringcondition>> hkbstringconditionList;
safeStringUMap<shared_ptr<hkbstringcondition>> hkbstringconditionList_E;

void hkbstringcondition::regis(string id, bool isEdited)
{
	isEdited ? hkbstringconditionList_E[id] = shared_from_this() : hkbstringconditionList[id] = shared_from_this();
	isEdited ? hkbconditionList_E[id] = shared_from_this() : hkbconditionList[id] = shared_from_this();
	ID = id;
}

void hkbstringcondition::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	for (auto& line : nodelines)
	{
		if (readParam("conditionString", line, conditionString)) break;
	}

	if ((Debug) && (!Error))
	{
		cout << stringcondition::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstringcondition::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + stringcondition::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			// existed
			string line;
			int backCounter = preaddress.length() - 1;

			while (isdigit(preaddress[backCounter]))
			{
				line = preaddress[backCounter] + line;
				--backCounter;
			}

			line = line + stringcondition::key + to_string(functionlayer);

			if (conditionOldFunction(shared_from_this(), address, functionlayer, line, true) != "") // is this new function or old for non generator
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbstringconditionList_E.erase(ID);
				hkbconditionList_E.erase(ID);
				editedBehavior.erase(ID);
				previousID = ID;
				ID = addressID[address];
				hkbstringconditionList_E[ID] = protect;
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
				hkbstringconditionList_E.erase(ID);
				hkbconditionList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				previousID = ID;
				ID = addressID[address];
				hkbstringconditionList_E[ID] = protect;
				hkbconditionList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
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

string hkbstringcondition::getClassCode()
{
	return stringcondition::key;
}

void hkbstringcondition::match(std::shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(3);
	output.reserve(3);
	usize base = 2;
	hkbstringcondition* ctrpart = static_cast<hkbstringcondition*>(counterpart.get());

	output.push_back(openObject(base, ID, stringcondition::classname, stringcondition::signature));		// 1
	paramMatch("conditionString", conditionString, ctrpart->conditionString, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", stringcondition::classname, output, isEdited);
}

void hkbstringcondition::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(3);

	output.push_back(openObject(base, ID, stringcondition::classname, stringcondition::signature));		// 1
	output.push_back(autoParam(base, "conditionString", conditionString));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, stringcondition::classname, output, true);
}

void hkbStringConditionExport(string id)
{
	// stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	vecstr storeline2 = FunctionLineNew[id];
	// stage 2 identify edits
	vecstr output;
	bool IsEdited = false;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[1]);
		output.push_back("<!-- ORIGINAL -->");
		output.push_back(storeline1[1]);
		output.push_back("<!-- CLOSE -->");
		IsEdited = true;
	}

	output.push_back(storeline2[2]);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				fwrite << output[i] << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbStringCondition Output Not Found (File: " << filename << ")" << endl;
			Error = true;
			return;
		}
	}
	else
	{
		if (IsFileExist(filename))
		{
			if (remove(filename.c_str()) != 0)
			{
				perror("Error deleting file");
				Error = true;
			}
		}
	}
}