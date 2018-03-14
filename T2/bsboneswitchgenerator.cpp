#include "bsboneswitchgenerator.h"
#include "Global.h"

using namespace std;

bsboneswitchgenerator::bsboneswitchgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "e" + to_string(functionlayer) + ">";

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

void bsboneswitchgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSBoneSwitchGenerator(ID: " << id << ") has been initialized!" << endl;
	}

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
						string tempbonedata = line.substr(position, tempint - position - 1);
						bonedata.push_back(tempbonedata);
						parent[tempbonedata] = id;
						referencingIDs[tempbonedata].push_back(id);
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
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
				{
					generator = line.substr(37, line.find("</hkparam>") - 37);
					parent[generator] = id;
					referencingIDs[generator].push_back(id);
				}
				else if (line.find("<hkparam name=\"ChildrenA\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(42, line.length() - 44));
					bonedata.reserve(children);
					pauseline = true;

				}
			}
			
			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSBoneSwitchGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: BSBoneSwitchGenerator Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSBoneSwitchGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsboneswitchgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSBoneSwitchGenerator(ID: " << id << ") has been initialized!" << endl;
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
						int position = 0;
						usize tempint = 0;

						for (int j = 0; j < curgen; j++)
						{
							position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
						}

						curgen++;
						string tempbonedata = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempbonedata].empty())
						{
							int tempint = line.find(tempbonedata);
							int templength = tempbonedata.length();
							tempbonedata = exchangeID[tempbonedata];
							line.replace(tempint, templength, tempbonedata);
						}

						bonedata.push_back(tempbonedata);
						parent[tempbonedata] = id;
						referencingIDs[tempbonedata].push_back(id);
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
							variablebindingset = exchangeID[variablebindingset];
							line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
						}
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
				{
					generator = line.substr(37, line.find("</hkparam>") - 37);
					if (!exchangeID[generator].empty())
					{
						int tempint = line.find(generator);
						generator = exchangeID[generator];
						line.replace(tempint, line.find("</hkparam>") - tempint, generator);
					}
					parent[generator] = id;
					referencingIDs[generator].push_back(id);
				}
				else if (line.find("<hkparam name=\"ChildrenA\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(42, line.length() - 44));
					bonedata.reserve(children);
					pauseline = true;
				}
			}
			
			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSBoneSwitchGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSBoneSwitchGenerator(newID: " << id << ") with BSBoneSwitchGenerator(oldID: " << tempid << ")" << endl;
		}

		// replace referencing IDs
		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		for (unsigned int i = 0; i < bonedata.size(); i++)
		{
			referencingIDs[bonedata[i]].pop_back();
			referencingIDs[bonedata[i]].push_back(tempid);
		}

		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);

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
			cout << "ERROR: BSBoneSwitchGenerator Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: BSBoneSwitchGenerator Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSBoneSwitchGenerator(newID: " << id << ") with BSBoneSwitchGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		}
		else
		{
			cout << "ERROR: BSBoneSwitchGenerator Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}
		output.close();

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "BSBoneSwitchGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsboneswitchgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSBoneSwitchGenerator(ID: " << id << ") has been initialized!" << endl;
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
						string tempbonedata = line.substr(position, tempint - position - 1);
						if (!exchangeID[tempbonedata].empty())
						{
							tempbonedata = exchangeID[tempbonedata];
						}
						bonedata.push_back(tempbonedata);
						parent[tempbonedata] = id;
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
				else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
				{
					generator = line.substr(37, line.find("</hkparam>") - 37);
					if (!exchangeID[generator].empty())
					{
						generator = exchangeID[generator];
					}
					parent[generator] = id;
				}
				else if (line.find("<hkparam name=\"ChildrenA\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(42, line.length() - 44));
					bonedata.reserve(children);
					pauseline = true;
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy BSBoneSwitchGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSBoneSwitchGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bsboneswitchgenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string bsboneswitchgenerator::GetBoneData(int child)
{
	return "#" + boost::regex_replace(string(bonedata[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int bsboneswitchgenerator::GetChildren()
{
	return children;
}

string bsboneswitchgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsboneswitchgenerator::IsBindingNull()
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

string bsboneswitchgenerator::GetAddress()
{
	return address;
}

bool bsboneswitchgenerator::IsNegate()
{
	return IsNegated;
}

void BSBoneSwitchGeneratorExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit BSBoneSwitchGenerator Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit BSBoneSwitchGenerator Output Not Found (Edited File: " << editedfile << ")" << endl;
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
		if (i < storeline1.size()) // existing variable value
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
			output.push_back(storeline2[i]);
			curline++;
		}
		else // new data
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = curline;
				IsEdited = true;
				open = true;
			}

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
			outputfile << "			</hkparam>" << "\n";
			outputfile << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit BSBoneSwitchGenerator Output Not Found (New Edited File: " << editedfile << ")" << endl;
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