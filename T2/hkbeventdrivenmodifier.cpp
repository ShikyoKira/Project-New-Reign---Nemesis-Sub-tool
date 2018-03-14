#include "hkbeventdrivenmodifier.h"
#include "Global.h"

using namespace std;

hkbeventdrivenmodifier::hkbeventdrivenmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "aa" + to_string(functionlayer) + ">";

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

void hkbeventdrivenmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventDrivenModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"modifier\">", 0) != string::npos)
			{
				modifier = line.substr(28, line.find("</hkparam>") - 28);
				if (modifier != "null")
				{
					referencingIDs[modifier].push_back(id);
				}
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbEventDrivenModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbEventDrivenModifier Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbEventDrivenModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventdrivenmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventDrivenModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"modifier\">", 0) != string::npos)
			{
				modifier = line.substr(28, line.find("</hkparam>") - 28);
				if (modifier != "null")
				{
					if (!exchangeID[modifier].empty())
					{
						int tempint = line.find(modifier);
						modifier = exchangeID[modifier];
						line.replace(tempint, line.find("</hkparam>") - tempint, modifier);
					}
					referencingIDs[modifier].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbEventDrivenModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;

		string tempid;
		if (!addressChange[address].empty())
		{
			tempid = addressID[addressChange[address]];
			addressChange.erase(addressChange.find(address));
		}
		else
		{
			tempid = addressID[address];
		}
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbEventDrivenModifier(newID: " << id << ") with hkbEventDrivenModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		if (modifier != "null")
		{
			referencingIDs[modifier].pop_back();
			referencingIDs[modifier].push_back(tempid);
		}
		
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
			cout << "ERROR: hkbEventDrivenModifier Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbEventDrivenModifier Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbEventDrivenModifier(newID: " << id << ") with hkbEventDrivenModifier(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbEventDrivenModifier Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbEventDrivenModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventdrivenmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbEventDrivenModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"modifier\">", 0) != string::npos)
			{
				modifier = line.substr(28, line.find("</hkparam>") - 28);
				if (modifier != "null")
				{
					if (!exchangeID[modifier].empty())
					{
						modifier = exchangeID[modifier];
					}
				}
				break;
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbEventDrivenModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbEventDrivenModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbeventdrivenmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbeventdrivenmodifier::IsBindingNull()
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

string hkbeventdrivenmodifier::GetModifier()
{
	return "#" + boost::regex_replace(string(modifier), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbeventdrivenmodifier::GetAddress()
{
	return address;
}

bool hkbeventdrivenmodifier::IsNegate()
{
	return IsNegated;
}

void hkbEventDrivenModifierExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit hkbEventDrivenModifier Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit hkbEventDrivenModifier Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
	}

	for (unsigned int j = 0; j < storeline2.size(); j++) // changing newID to modCode ID
	{
		if (storeline2[j].find("#", 0) != string::npos)
		{
			usize tempint = 0;
			int position = 0;
			usize size = count(storeline2[j].begin(), storeline2[j].end(), '#');

			for (unsigned int i = 0; i < size; i++)
			{
				position = storeline2[j].find("#", tempint);
				tempint = storeline2[j].find("#", position + 1);
				string tempID;

				if (tempint == -1)
				{
					string templine;

					if (storeline2[j].find("signature", 0) != string::npos)
					{
						templine = storeline2[j].substr(0, storeline2[j].find("class"));
					}
					else
					{
						templine = storeline2[j].substr(position, tempint - position - 1);
					}

					tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
				}
				else
				{
					tempID = storeline2[j].substr(position, tempint - position - 1);
				}

				int tempLength = tempID.length();
				string strID = tempID.substr(1, tempLength - 1);
				int intID = stoi(strID);

				if (intID > 10000)
				{
					int position2 = storeline2[j].find(tempID);
					string modID;

					if (!newID[tempID].empty())
					{
						modID = newID[tempID];
					}
					else
					{
						modID = "#" + modcode + "$" + to_string(functioncount);
						newID[tempID] = modID;
						functioncount++;
					}

					storeline2[j].replace(position2, tempLength, modID);
				}
			}

			if (((storeline2[j].find("<hkparam name=\"activateEventId\">", 0) != string::npos) || (storeline2[j].find("<hkparam name=\"deactivateEventId\">", 0) != string::npos)) && (storeline2[j].find("<hkparam name=\"activateEventId\">-1</hkparam>", 0) == string::npos) && (storeline2[j].find("<hkparam name=\"deactivateEventId\">-1</hkparam>", 0) == string::npos))
			{
				usize eventpos = storeline2[j].find("Id\">") + 4;
				string eventid = storeline2[j].substr(eventpos, storeline2[j].find("</hkparam>"));

				if (eventID[eventid].length() != 0)
				{
					storeline2[j].replace(eventpos, storeline2[j].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
				}
				else
				{
					cout << "ERROR: Invalid event id. Please ensure that event id is valid(EditedFile: " << editedfile << ", ID: " << id << ")" << endl;
					Error = true;
					return;
				}
			}
		}
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
			output.close();
		}
		else
		{
			cout << "ERROR: Edit hkbEventDrivenModifier Output Not Found (New Edited File: " << editedfile << ")" << endl;
			Error = true;
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