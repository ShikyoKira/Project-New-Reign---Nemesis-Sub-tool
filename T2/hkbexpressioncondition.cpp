#include "hkbexpressioncondition.h"
#include "Global.h"

using namespace std;

hkbexpressioncondition::hkbexpressioncondition(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "l" + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id);
		}
		else
		{
			nonCompare(filepath, id);
		}
	}
	else if (!Error)
	{
		string dummyID = CrossReferencing(id, address, functionlayer, compare, true);
		if (compare)
		{
			IsNegated = true;
		}
		else
		{
			IsNegated = true;

			address = region[id];
		}
	}
	else
	{
		return;
	}
}

void hkbexpressioncondition::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbExpressionCondition(ID: " << id << ") has been initialized!" << endl;
	}

	vector<string> storeline;
	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbExpressionCondition Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	ofstream output("temp/" + id + ".txt");
	if (output.is_open())
	{
		for (unsigned int i = 0; i < storeline.size(); i++)
		{
			output << storeline[i] << "\n";
		}
		output.close();
	}
	else
	{
		cout << "ERROR: hkbExpressionCondition Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbExpressionCondition(ID: " << id << ") is complete!" << endl;
	}
}

void hkbexpressioncondition::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbExpressionCondition(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineEdited[id][i];
			
			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbExpressionCondition Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
	{
		IsForeign[id] = false;

		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbExpressionCondition(newID: " << id << ") with hkbExpressionCondition(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		string inputfile = "temp/" + tempid + ".txt";
		vector<string> storeline;
		storeline.reserve(FileLineCount(inputfile));
		ifstream input(inputfile); // read old function
		if (input.is_open())
		{
			while (getline(input, line))
			{
				storeline.push_back(line);
			}
			input.close();
		}
		else
		{
			cout << "ERROR: hkbExpressionCondition Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		// stage 3
		ofstream output("new/" + tempid + ".txt"); // output stored function data
		if (output.is_open())
		{
			output << storeline[0] << "\n";
			for (unsigned int i = 1; i < newline.size(); i++)
			{
				output << newline[i] << "\n";
			}
			output.close();
		}
		else
		{
			cout << "ERROR: hkbExpressionCondition Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbExpressionCondition(newID: " << id << ") with hkbExpressionCondition(oldID: " << tempid << ") is complete!" << endl;
		}
	}

	else
	{
		IsForeign[id] = true;

		ofstream output("new/" + id + ".txt"); // output stored function data
		if (output.is_open())
		{
			for (unsigned int i = 0; i < newline.size(); i++)
			{
				output << newline[i] << "\n";
			}
			output.close();
		}
		else
		{
			cout << "ERROR: hkbExpressionCondition Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbExpressionCondition(ID: " << id << ") is complete!" << endl;
	}
}

void hkbexpressioncondition::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbExpressionCondition(ID: " << id << ") has been initialized!" << endl;
	}

	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbExpressionCondition Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbExpressionCondition(ID: " << id << ") is complete!" << endl;
	}
}

string hkbexpressioncondition::GetAddress()
{
	return address;
}

bool hkbexpressioncondition::IsNegate()
{
	return IsNegated;
}

void hkbExpressionConditionExport(string originalfile, string editedfile, string id)
{
	// stage 1 reading
	vector<string> storeline1;
	ifstream origfile(originalfile);
	string line;

	if (origfile.is_open())
	{
		while (getline(origfile, line))
		{
			storeline1.push_back(line);
		}
		origfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbExpressionCondition Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
		return;
	}

	vector<string> storeline2;
	ifstream editfile(editedfile);

	if (editfile.is_open())
	{
		while (getline(editfile, line))
		{
			storeline2.push_back(line);
		}
		editfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbExpressionCondition Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
		return;
	}

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
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	if (IsEdited)
	{
		ofstream outputfile(filename);
		if (outputfile.is_open())
		{
			for (unsigned int i = 0; i < output.size(); i++)
			{
				outputfile << output[i] << "\n";
			}
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbExpressionCondition Output Not Found (New Edited File: " << editedfile << ")" << endl;
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