#include "hkbmanualselectorgenerator.h"
#include "Global.h"

using namespace std;

hkbmanualselectorgenerator::hkbmanualselectorgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "m" + to_string(functionlayer) + ">region";

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
	else
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
}

void hkbmanualselectorgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
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
				if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');
					for (unsigned int i = 0; i < size; i++)
					{
						int position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);
						generator.push_back(tempgenerator);
						if (!IsReferenceExist[tempgenerator])
						{
							referencingIDs[tempgenerator].push_back(id);
							IsReferenceExist[tempgenerator] = true;
						}
						parent[tempgenerator] = id;
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
				else if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
					if (variablebindingset != "null")
					{
						referencingIDs[variablebindingset].push_back(id);
					}
					parent[variablebindingset] = id;
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
		cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbManualSelectorGenerator Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	address = name + "(m" + to_string(regioncount[name]) + ")=>";
	regioncount[name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	unordered_map<string, bool> IsReferenceExist;
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
						int position = 0;
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
							int tempint = line.find(tempgenerator);
							int templength = tempgenerator.length();
							tempgenerator = exchangeID[tempgenerator];
							line.replace(tempint, templength, tempgenerator);
						}

						parent[tempgenerator] = id;
						generator.push_back(tempgenerator);

						if (!IsReferenceExist[tempgenerator])
						{
							referencingIDs[tempgenerator].push_back(id);
							IsReferenceExist[tempgenerator] = true;
						}
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
				else if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
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
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		elements[tempid] = children;
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbManualSelectorGenerator(newID: " << id << ") with hkbManualSelectorGenerator(oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
				if ((newline[i].find("<hkparam name=\"generators\" numelements=", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
				}
				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].length() < 15))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].length() < 15))
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
				else if (storeline[curline].find(newline[i], 0) == string::npos)
				{
					usize size = count(storeline[curline].begin(), storeline[curline].end(), '#');
					usize size2 = count(newline[i].begin(), newline[i].end(), '#');

					if (size < size2)
					{
						int position = 0;
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
			cout << "ERROR: hkbManualSelectorGenerator Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}
		
		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbManualSelectorGenerator(newID: " << id << ") with hkbManualSelectorGenerator(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbManualSelectorGenerator Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
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
					break;
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
						if (!exchangeID[tempgenerator].empty())
						{
							tempgenerator = exchangeID[tempgenerator];
						}
						parent[tempgenerator] = id;
						generator.push_back(tempgenerator);
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

				else if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string hkbmanualselectorgenerator::NextGenerator(int child)
{
	return "#" + boost::regex_replace(string(generator[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbmanualselectorgenerator::GetChildren()
{
	return children;
}

string hkbmanualselectorgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbmanualselectorgenerator::IsBindingNull()
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

string hkbmanualselectorgenerator::GetClass(string id, string inputfile)
{
	string line;
	string classname = "<hkobject name=\"" + id;
	ifstream input(inputfile);

	if (input.is_open())
	{
		while (getline(input, line))
		{
			if (line.find(classname, 0) != string::npos)
			{
				int tempint = line.find("class") + 7;
				return line.substr(tempint, line.find("signature") - tempint - 2);
			}
		}
	}
	else
	{
		cout << "ERROR: GetClass Inputfile(" << id << ", " << inputfile << ")" << endl;
	}
	input.close();

	cout << "ERROR: GetClass(" << id << ", " << inputfile << ")" << endl;
	Error = true;

	return "null";
}

string hkbmanualselectorgenerator::GetAddress()
{
	return address;
}

bool hkbmanualselectorgenerator::IsNegate()
{
	return IsNegated;
}

void hkbManualSelectorGeneratorExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit hkbManualSelectorGenerator Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit hkbManualSelectorGenerator Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
	}

	vector<string> output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	bool IsOpenOut = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos) // existing variable value
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
								if ((storeline1[j].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos) && ((j != openpoint) || (IsOpenOut)))
								{
									output.push_back("			</hkparam>");
									IsOpenOut = false;
								}
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
			if (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos)
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
									if ((storeline1[j].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos) && ((j != openpoint) || (IsOpenOut)))
									{
										output.push_back("			</hkparam>");
										IsOpenOut = false;
									}
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
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsOpenOut = true;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}
		}

		if (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos) // merging with new element or existing data or itself
		{
			if (open)
			{
				if (output.back().find("OPEN", 0) != string::npos)
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
				}
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

	if (open) // close unclosed edits
	{
		if (IsChanged)
		{
			closepoint = curline;
			if (closepoint != openpoint)
			{
				output.push_back("<!-- ORIGINAL -->");
				for (int j = openpoint; j < closepoint; j++)
				{
					if ((storeline1[j].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos) && ((j != openpoint) || (IsOpenOut)))
					{
						output.push_back("			</hkparam>");
						IsOpenOut = false;
					}
					output.push_back(storeline1[j]);
				}
			}
			IsChanged = false;
		}
		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	NemesisReaderFormat(output);

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
			outputfile << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit hkbManualSelectorGenerator Output Not Found (New Edited File: " << editedfile << ")" << endl;
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