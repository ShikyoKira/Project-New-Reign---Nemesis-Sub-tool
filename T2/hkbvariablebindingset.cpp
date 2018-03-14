#include "hkbvariablebindingset.h"
#include "Global.h"

using namespace std;

hkbvariablebindingset::hkbvariablebindingset(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "t" + to_string(functionlayer) + ">";

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
		}
	}
	else
	{
		return;
	}
}

void hkbvariablebindingset::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableBindingSet(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbVariableBindingSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbVariableBindingSet Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbVariableBindingSet(ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablebindingset::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableBindingSet(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbVariableBindingSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
	{
		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbVariableBindingSet(newID: " << id << ") with hkbVariableBindingSet(oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbVariableBindingSet Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbVariableBindingSet Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbVariableBindingSet(newID: " << id << ") with hkbVariableBindingSet(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbVariableBindingSet Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbVariableBindingSet(ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablebindingset::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbVariableBindingSet(ID: " << id << ") has been initialized!" << endl;
	}

	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbVariableBindingSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbVariableBindingSet(ID: " << id << ") is complete!" << endl;
	}
}

string hkbvariablebindingset::GetAddress()
{
	return address;
}

bool hkbvariablebindingset::IsNegate()
{
	return IsNegated;
}

void hkbVariableBindingSetExport(string originalfile, string editedfile, string id)
{
	// stage 1 reading
	vector<string> storeline1;
	ifstream origfile(originalfile);
	string line;

	if (origfile.is_open())
	{
		while (getline(origfile, line))
		{
			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
		origfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbVariableBindingSet Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
		return;
	}

	vector<string> storeline2;
	ifstream editfile(editedfile);

	if (editfile.is_open())
	{
		while (getline(editfile, line))
		{
			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
		editfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbVariableBindingSet Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vector<string> output;
	bool IsChanged = false;
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
		IsChanged = true;
		IsEdited = true;
		open = true;
	}

	output.push_back(storeline2[1]);

	for (unsigned int i = 2; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"indexOfBindingToEnable\">", 0) == string::npos) // existing variable value
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
			{
				if (open)
				{
					if (IsChanged)
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

						IsChanged = false;
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
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}
			curline++;
		}
		else // new element
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					if (storeline2[i].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos)
					{
						IsChanged = true;
					}
					IsEdited = true;
					open = true;
				}

			}
			else 
			{
				if (open)
				{
					if (IsChanged)
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
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
		}

		if (storeline2[i].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos) // merging with new element or existing data or itself
		{
			if (open)
			{
				bool IsOpenOut = false;

				if ((openpoint == curline) && (IsChanged))
				{
					output.pop_back();
					output.push_back("			</hkparam>");
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back("			</hkparam>");
					output.push_back(storeline2[i]);
					IsOpenOut = true;
				}

				closepoint = curline + 1;
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < closepoint; j++)
				{
					if ((storeline1[j].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos) && (IsOpenOut))
					{
						output.push_back("			</hkparam>");
					}

					output.push_back(storeline1[j]);
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}
			else
			{
				output.push_back("			</hkparam>");
				output.push_back(storeline2[i]);
			}
		}
		else
		{
			output.push_back(storeline2[i]);
		}
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
				if (output[i].find("<hkparam name=\"memberPath\">", 0) != string::npos)
				{
					outputfile << "				<hkobject>" << "\n";
					outputfile << output[i] << "\n";
				}
				else if (output[i].find("<hkparam name=\"bindingType\">", 0) != string::npos)
				{
					outputfile << output[i] << "\n";
					outputfile << "				</hkobject>" << "\n";
				}
				else if (output[i].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos)
				{
					outputfile << output[i] << "\n";
				}
				else if (output[i].find("<hkparam name=\"variableIndex\">", 0) != string::npos && output[i].find("<hkparam name=\"variableIndex\">-1</hkparam>", 0) != string::npos)
				{
					usize varpos = output[i].find("variableIndex") + 15;
					string varID = output[i].substr(varpos, output[i].find("</hkparam>"));

					if (eventID[varID].length() != 0)
					{
						output[i].replace(varpos, output[i].find("</hkparam>") - varpos, "$variableID[" + variableID[varID] + "]$");
					}

					outputfile << output[i] << "\n";
				}
				else
				{
					outputfile << output[i] << "\n";
				}
			}

			outputfile << "		</hkobject>" << "\n";
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