#include "hkbstringcondition.h"
#include "Global.h"

using namespace std;

hkbstringcondition::hkbstringcondition(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "ck" + to_string(functionlayer) + ">";

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
			addressChange.erase(addressChange.find(address));
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

	line = line + "ck" + to_string(functionlayer);
	
	// stage 2
	if (conditionOldFunction(id, address, functionlayer, line, true) != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(addressChange.find(address));
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
			vector<string> emptyVS;
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

void hkbStringConditionExport(string id)
{
	// stage 1 reading
	vector<string> storeline1 = FunctionLineTemp[id];

	vector<string> storeline2 = FunctionLineNew[id];
	// stage 2 identify edits
	vector<string> output;
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