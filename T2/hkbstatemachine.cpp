#include "hkbstatemachine.h"
#include "Global.h"

using namespace std;

hkbstatemachine::hkbstatemachine(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "r" + to_string(functionlayer) + ">region";

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
			Dummy(dummyID);

			if (IsForeign[id])
			{
				if (!region[id].empty())
				{
					address = region[id];
				}
				else
				{
					address = preaddress;
				}
			}
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

void hkbstatemachine::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	vector<string> storeline;
	string line;
	bool pauseline = false;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');
					for (unsigned int i = 0; i < size; i++)
					{
						int position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);
						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
						referencingIDs[tempgenerator].push_back(id);
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
					if (variablebindingset != "null")
					{
						referencingIDs[variablebindingset].push_back(id);
					}
					parent[variablebindingset] = id;
				}

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);
					if (payload != "null")
					{
						referencingIDs[payload].push_back(id);
					}
					parent[payload] = id;
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);
					if (wildcard != "null")
					{
						referencingIDs[wildcard].push_back(id);
					}
					parent[wildcard] = id;
				}

				else if (line.find("<hkparam name=\"name\">", 0) != string::npos)
				{
					name = line.substr(24, line.find("</hkparam>") - 24);
				}
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbStateMachine Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	address = name + "(r" + to_string(regioncount[name]) + ")=>";
	regioncount[name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachine::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineEdited[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					int curgen = 1;
					usize size = count(line.begin(), line.end(), '#');
					for (unsigned int i = 0; i < size; i++)
					{
						usize position = 0;
						usize tempint = 0;

						for (int j = 0; j < curgen; j++)
						{
							position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
						}

						curgen++;
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							usize tempint = line.find(tempgenerator);
							int templength = tempgenerator.length();
							tempgenerator = exchangeID[tempgenerator];
							line.replace(tempint, templength, tempgenerator);
						}

						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
						referencingIDs[tempgenerator].push_back(id);
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
					if (variablebindingset != "null")
					{
						if (!exchangeID[variablebindingset].empty())
						{
							int tempint = line.find(variablebindingset);
							line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[variablebindingset]);
							variablebindingset = exchangeID[variablebindingset];
						}
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);
					if (payload != "null")
					{
						if (!exchangeID[payload].empty())
						{
							int tempint = line.find(payload);
							payload = exchangeID[payload];
							line.replace(tempint, line.find("</hkparam>") - tempint, payload);
						}
						parent[payload] = id;
						referencingIDs[payload].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);
					if (wildcard != "null")
					{
						if (!exchangeID[wildcard].empty())
						{
							usize tempint = line.find(wildcard);
							line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[wildcard]);
							wildcard = exchangeID[wildcard];
						}
						parent[wildcard] = id;
						referencingIDs[wildcard].push_back(id);
					}
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;

		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachine(newID: " << id << ") with hkbStateMachine(oldID: " << tempid << ")" << endl;
		}
		
		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		for (unsigned int i = 0; i < generator.size(); i++)
		{
			referencingIDs[generator[i]].pop_back();
			referencingIDs[generator[i]].push_back(tempid);
		}

		if (payload != "null")
		{
			referencingIDs[payload].pop_back();
			referencingIDs[payload].push_back(tempid);
		}

		if (wildcard != "null")
		{
			referencingIDs[wildcard].pop_back();
			referencingIDs[wildcard].push_back(tempid);
		}

		string inputfile = "temp/" + tempid + ".txt";
		vector<string> storeline;
		storeline.reserve(FileLineCount(inputfile));
		ifstream input(inputfile);
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
			cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		// stage 3
		int curline = 1;
		bool IsNewChild = false;
		vector<string> newstoreline;
		vector<string> newchild;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
			if (!IsNewChild)
			{
				if ((newline[i].find("numelements", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
				}
				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].find("wildcardTransitions", 0) == string::npos))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].find("wildcardTransitions", 0) == string::npos))
					{
						for (unsigned int j = 0; j < newchild.size(); j++)
						{
							newstoreline.push_back(newchild[j]);
						}
						newstoreline.push_back(newline[i]);
						curline++;
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}
				}
				else if ((storeline[curline].find(newline[i], 0) == string::npos) && (storeline[curline].find("</hkparam>", 0) == string::npos) && (storeline[curline].find("</hkobject>", 0) == string::npos))
				{
					usize size = count(storeline[curline].begin(), storeline[curline].end(), '#');
					usize size2 = count(newline[i].begin(), newline[i].end(), '#');

					if (size < size2)
					{
						usize position = 0;
						usize tempint = 0;

						for (unsigned int j = 0; j < size + 1; j++)
						{
							position = newline[i].find("#", tempint);
							tempint = newline[i].find("#", position + 1);
						}
						newstoreline.push_back(newline[i].substr(0, position - 1));
						newchild.push_back("				" + newline[i].substr(position, -1));
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}

					curline++;
				}
				else
				{
					newstoreline.push_back(newline[i]);
					curline++;
				}
			}
		}

		ofstream output("new/" + tempid + ".txt"); // output stored function data
		if (output.is_open())
		{
			for (unsigned int i = 0; i < newstoreline.size(); i++)
			{
				output << newstoreline[i] << "\n";
			}
			output.close();
		}
		else
		{
			cout << "ERROR: hkbStateMachine Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachine(newID: " << id << ") with hkbStateMachine(oldID: " << tempid << ") is complete!" << endl;
		}

		address = region[tempid];
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
			cout << "ERROR: hkbStateMachine Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachine::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);
	bool pauseline = false;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');
					generator.reserve(size);
					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							tempgenerator = exchangeID[tempgenerator];
						}

						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
					}
				}
			}
			else
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

						parent[variablebindingset] = id;
					}
				}

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);

					if (payload != "null")
					{
						if (!exchangeID[payload].empty())
						{
							payload = exchangeID[payload];
						}
						parent[payload] = id;
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						if (!exchangeID[wildcard].empty())
						{
							wildcard = exchangeID[wildcard];
						}
						parent[wildcard] = id;
					}

					break;
				}
			}
		}

		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	if (!region[id].empty())
	{
		address = region[id];
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachine::NextGenerator(int child)
{
	return "#" + boost::regex_replace(string(generator[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachine::GetChildren()
{
	return children;
}

string hkbstatemachine::GetPayload()
{
	return "#" + boost::regex_replace(string(payload), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachine::IsPayloadNull()
{
	if (payload.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbstatemachine::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachine::IsBindingNull()
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

string hkbstatemachine::GetWildcard()
{
	return wildcard;
}

bool hkbstatemachine::IsWildcardNull()
{
	if (wildcard.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbstatemachine::GetAddress()
{
	return address;
}

bool hkbstatemachine::IsNegate()
{
	return IsNegated;
}

void hkbStateMachineExport(string originalfile, string editedfile, string id)
{
	//stage 1 reading
	vector<string> storeline1;
	string line;
	ifstream origfile(originalfile);

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
		cout << "ERROR: Edit hkbStateMachine Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
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
		cout << "ERROR: Edit hkbStateMachine Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
	}

	vector<string> output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"wildcardTransitions\">", 0) == string::npos) // existing variable value
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
		else
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					openpoint = curline;
					if (storeline2[i].find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
					{
						IsChanged = true;
					}
					else
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
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
						IsChanged = false;
					}
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
				curline++;
			}
		}

		if (storeline2[i].find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos) // merging with new element or existing data or itself
		{
			if (open)
			{
				bool IsOpenOut = false;

				if ((openpoint == curline) && (IsChanged))
				{
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
					if ((storeline1[j].find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos) && (IsOpenOut))
					{
						output.push_back("			</hkparam>");
					}

					output.push_back(storeline1[j]);
				}

				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
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

	NemesisReaderFormat(output, true);

	// stage 3 output if it is edited
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("<hkparam name=\"eventToSendWhenStateOrTransitionChanges\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
						outputfile << "				<hkobject>" << "\n";
					}
					else
					{
						outputfile << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
					}
					else
					{
						outputfile << "				<hkobject>" << "\n";
						outputfile << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
						outputfile << "				</hkobject>" << "\n";
						outputfile << "			</hkparam>" << "\n";
					}
					else
					{
						outputfile << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"startStateChooser\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
					}
					else
					{
						outputfile << "				</hkobject>" << "\n";
						outputfile << "			</hkparam>" << "\n";
						outputfile << output[i] << "\n";
					}
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
			cout << "ERROR: Edit hkbStateMachine Output Not Found (New Edited File: " << editedfile << ")" << endl;
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