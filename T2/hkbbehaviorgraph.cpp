#include "hkbbehaviorgraph.h"
#include "Global.h"

using namespace std;

hkbbehaviorgraph::hkbbehaviorgraph(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "g" + to_string(functionlayer) + ">";

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

void hkbbehaviorgraph::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraph(" << id << ") has been initialized!" << endl;
	}

	vector<string> storeline;
	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
				if (variablebindingset != "null")
				{
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"rootGenerator\">", 0) != string::npos)
			{
				generator = line.substr(33, line.find("</hkparam>") - 33);
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"data\">", 0) != string::npos)
			{
				data = line.substr(24, line.find("</hkparam>") - 24);
				referencingIDs[data].push_back(id);
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraph Inputfile(" << id << ", " << filepath << ")" << endl;
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
		cout << "ERROR: hkbBehaviorGraph Outputfile(" << id << ", " << filepath << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraph(" << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraph::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraph(ID: " << id << ") has been initialized!" << endl;
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

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						int tempint = line.find(variablebindingset);
						variablebindingset = exchangeID[variablebindingset];
						line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
					}
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"rootGenerator\">", 0) != string::npos)
			{
				generator = line.substr(33, line.find("</hkparam>") - 33);
				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					generator = exchangeID[generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, generator);
				}
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"data\">", 0) != string::npos)
			{
				data = line.substr(24, line.find("</hkparam>") - 24);
				if (!exchangeID[data].empty())
				{
					int tempint = line.find(data);
					data = exchangeID[data];
					line.replace(tempint, line.find("</hkparam>") - tempint, data);
				}
				referencingIDs[data].push_back(id);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraph Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;

		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraph(newID: " << id << ") with hkbBehaviorGraph(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);

		referencingIDs[data].pop_back();
		referencingIDs[data].push_back(tempid);

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
			cout << "ERROR: hkbBehaviorGraph Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbBehaviorGraph Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraph(newID: " << id << ") with hkbBehaviorGraph(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbBehaviorGraph Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraph(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraph::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBehaviorGraph(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						variablebindingset = exchangeID[variablebindingset];
					}
				}
			}
			else if (line.find("<hkparam name=\"rootGenerator\">", 0) != string::npos)
			{
				generator = line.substr(33, line.find("</hkparam>") - 33);
				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}
			}
			else if (line.find("<hkparam name=\"data\">", 0) != string::npos)
			{
				data = line.substr(24, line.find("</hkparam>") - 24);
				if (!exchangeID[data].empty())
				{
					data = exchangeID[data];
				}
				break;
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbBehaviorGraph Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBehaviorGraph(ID: " << id << ") is complete!" << endl;
	}
}

string hkbbehaviorgraph::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbbehaviorgraph::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbbehaviorgraph::IsBindingNull()
{
	if (variablebindingset.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbbehaviorgraph::GetData()
{
	return data;
}

string hkbbehaviorgraph::GetAddress()
{
	return address;
}

bool hkbbehaviorgraph::IsNegate()
{
	return IsNegated;
}

void hkbBehaviorGraphExport(string originalfile, string editedfile, string id)
{
	//stage 1 reading
	vector<string> storeline1;
	string line;
	ifstream origfile(originalfile);

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
		cout << "ERROR: Edit hkbBehaviorGraph Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vector<string> storeline2;
	ifstream editfile(editedfile);
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	if (editfile.is_open())
	{
		while (getline(editfile, line))
		{
			if ((line.find(storeline1[curline], 0) != string::npos) && (line.length() == storeline1[curline].length()))
			{
				if (open)
				{
					closepoint = curline;
					if (closepoint != openpoint)
					{
						storeline2.push_back("<!-- ORIGINAL -->");
						for (int j = openpoint; j < closepoint; j++)
						{
							storeline2.push_back(storeline1[j]);
						}
					}
					storeline2.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					storeline2.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					open = true;
				}
				IsEdited = true;
			}
			storeline2.push_back(line);
			curline++;
		}
		editfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbBehaviorGraph Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
	}

	// stage 3 output if it is edited
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	if (IsEdited)
	{
		ofstream output(filename);
		if (output.is_open())
		{
			for (unsigned int i = 0; i < storeline2.size(); i++)
			{
				output << storeline2[i] << "\n";
			}
		}
		else
		{
			cout << "ERROR: Edit hkbBehaviorGraph Output Not Found (New Edited File: " << editedfile << ")" << endl;
			Error = true;
		}
		output.close();
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