#include "hkbvariablevalueset.h"
#include "Global.h"

using namespace std;

hkbvariablevalueset::hkbvariablevalueset(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "u" + to_string(functionlayer) + ">";

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
		string dummyID = CrossReferencing(id, address, functionlayer, compare);
		if (compare)
		{
			IsNegated = true;
		}
		else
		{
			IsNegated = true;
		}
	}
	else
	{
		return;
	}
}

void hkbvariablevalueset::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableValueSet(ID: " << id << ") has been initialized!" << endl;
	}

	vector<string> storeline;
	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		// output stored data line
		ofstream output("temp/" + id + ".txt");

		if (output.is_open())
		{
			for (unsigned int i = 0; i < FunctionLineOriginal[id].size(); i++)
			{
				output << FunctionLineOriginal[id][i] << "\n";
			}

			output.close();
		}
		else
		{
			cout << "ERROR: hkbVariableValueSet Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}
	}
	else
	{
		cout << "ERROR: hkbVariableValueSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}
	
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbVariableValueSet(ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablevalueset::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableValueSet(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbVariableValueSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2	
	if (addressID[address] != "") // is this new function or old for non generator
	{
		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbVariableValueSet(newID: " << id << ") with hkbVariableValueSet(oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbVariableValueSet Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbVariableValueSet Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}
		
		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbVariableValueSet(newID: " << id << ") with hkbVariableValueSet(oldID: " << tempid << ") is complete!" << endl;
		}
	}

	else
	{
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
			cout << "ERROR: hkbVariableValueSet Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);
	
	if ((Debug) && (!Error))
	{
		cout << "hkbVariableValueSet(ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablevalueset::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbVariableValueSet(ID: " << id << ") has been initialized!" << endl;
	}

	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbVariableValueSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbVariableValueSet(ID: " << id << ") is complete!" << endl;
	}
}

string hkbvariablevalueset::GetAddress()
{
	return address;
}

bool hkbvariablevalueset::IsNegate()
{
	return IsNegated;
}

void hkbVariableValueSetExport(string originalfile, string editedfile, string id)
{
	//stage 1 reading
	vector<string> storeline1;
	ifstream origfile(originalfile);
	string line;
	bool skip = false;

	if (origfile.is_open())
	{
		while (getline(origfile, line))
		{
			if (!skip)
			{
				storeline1.push_back(line);
			}
			else
			{
				skip = false;
			}

			if (line.find("name=\"value\">", 0) != string::npos)
			{
				storeline1.pop_back();
				storeline1.pop_back();
				storeline1.push_back(line);
				skip = true;
			}
		}
		origfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbVariableValueSet Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
		return;
	}

	skip = false;
	vector<string> storeline2;
	ifstream editfile(editedfile);

	if (editfile.is_open())
	{
		while (getline(editfile, line))
		{
			if (!skip)
			{
				storeline2.push_back(line);
			}
			else
			{
				skip = false;
			}

			if (line.find("name=\"value\">", 0) != string::npos)
			{
				storeline2.pop_back();
				storeline2.pop_back();
				storeline2.push_back(line);
				skip = true;
			}
		}
		editfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbVariableValueSet Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
		return;
	}
	
	// stage 2 identify edits
	vector<string> output;
	bool open = false;
	bool IsEdited = false;
	int curline = 2;
	int openpoint;
	int closepoint;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		openpoint = curline - 1;
		IsEdited = true;
		open = true;
	}

	output.push_back(storeline2[1]);

	for (unsigned int i = 2; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"quadVariableValues\" numelements=", 0) == string::npos) // existing variable value
		{
			if (storeline1[curline].find(storeline2[i], 0) != string::npos)
			{
				if (open)
				{
					closepoint = curline;

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}
			}

			curline++;
		}
		else // added variable value
		{
			if (storeline2[i].find("<hkparam name=\"quadVariableValues\" numelements=", 0) != string::npos)
			{
				if (open)
				{
					closepoint = curline - 1;
					output.pop_back();

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					output.push_back("<!-- CLOSE -->");
					output.push_back("			</hkparam>");
					open = false;
				}

				openpoint = i;
				break;
			}
			else
			{

				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}
		}

		output.push_back(storeline2[i]);
	}

	for (unsigned int i = openpoint; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"variantVariableValues\" numelements=", 0) == string::npos) // existing variable value
		{
			if (storeline1[curline].find(storeline2[i], 0) == string::npos)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			curline++;
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"variantVariableValues\" numelements=", 0) != string::npos)
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;
					open = false;

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
				}

				curline++;
			}
		}

		output.push_back(storeline2[i]);
	}

	// stage 3 output if it is edited
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("name=\"value\">", 0) != string::npos)
				{
					outputfile << "				<hkobject>" << "\n";
					outputfile << output[i] << "\n";
					outputfile << "				</hkobject>" << "\n";
				}
				else
				{
					outputfile << output[i] << "\n";
				}
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbVariableValueSet Output Not Found (New Edited File: " << editedfile << ")" << endl;
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