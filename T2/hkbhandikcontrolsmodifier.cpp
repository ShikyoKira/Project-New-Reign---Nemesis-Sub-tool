#include "hkbhandikcontrolsmodifier.h"
#include "Global.h"

using namespace std;

hkbhandikcontrolsmodifier::hkbhandikcontrolsmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "by" + to_string(functionlayer) + ">";

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

void hkbhandikcontrolsmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbHandIkControlsModifier(ID: " << id << ") has been initialized!" << endl;
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

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbHandIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbHandIkControlsModifier Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbHandIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbhandikcontrolsmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbHandIkControlsModifier(ID: " << id << ") has been initialized!" << endl;
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

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbHandIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbHandIkControlsModifier(newID: " << id << ") with hkbHandIkControlsModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
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
			cout << "ERROR: hkbHandIkControlsModifier Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbHandIkControlsModifier Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbHandIkControlsModifier(newID: " << id << ") with hkbHandIkControlsModifier(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbHandIkControlsModifier Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbHandIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbhandikcontrolsmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbHandIkControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
				break;
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbHandIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbHandIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbhandikcontrolsmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbhandikcontrolsmodifier::IsBindingNull()
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

string hkbhandikcontrolsmodifier::GetAddress()
{
	return address;
}

bool hkbhandikcontrolsmodifier::IsNegate()
{
	return IsNegated;
}

void hkbHandIkControlsModifierExport(string originalfile, string editedfile, string id)
{
	//stage 1 reading
	vector<string> storeline1;
	string line;
	ifstream origfile(originalfile);

	if (origfile.is_open())
	{
		while (getline(origfile, line))
		{
			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10) && (line.find("<hkparam name=\"controlData\">", 0) == string::npos))
			{
				storeline1.push_back(line);
			}
		}
		origfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbHandIkControlsModifier Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
	}

	vector<string> storeline2;
	ifstream editfile(editedfile);

	if (editfile.is_open())
	{
		while (getline(editfile, line))
		{
			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10) && (line.find("<hkparam name=\"controlData\">", 0) == string::npos))
			{
				storeline2.push_back(line);
			}
		}
		editfile.close();
	}
	else
	{
		cout << "ERROR: Edit hkbHandIkControlsModifier Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
	}

	//stage 2 identifying edits
	vector<string> output;
	bool IsEdited = false;
	bool IsChanged = false;
	bool open = false;
	int curline = 0;
	int openpoint;
	int closepoint;
	int storeI = 0;

	for (unsigned int i = storeI; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"hands\" numelements=", 0) == string::npos) // pre hands info
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}

				output.push_back(storeline2[i]);
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;
					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}
					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
				output.push_back(storeline2[i]);
			}

			curline++;

			if (storeline1[curline].find("<hkparam name=\"hands\" numelements=", 0) != string::npos)
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;
					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}
					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
		}
		else
		{
			storeI = i;
			break;
		}
	}

	for (unsigned int i = storeI; i < storeline2.size(); i++)
	{
		if (curline != storeline1.size()) // existing hands data
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (storeline2[i].find("<hkparam name=\"targetPosition\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
					output.push_back("					<hkparam name=\"controlData\">");
					output.push_back("						<hkobject>");
				}
				else if (storeline2[i - 1].find("<hkparam name=\"fixUp\">", 0) != string::npos)
				{
					output.push_back("						</hkobject>");
					output.push_back("					</hkparam>");
				}
				else if ((storeline2[i - 1].find("<hkparam name=\"enable\">", 0) != string::npos) && (curline > 6))
				{
					output.push_back("				</hkobject>");
				}

				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}

				output.push_back(storeline2[i]);
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;
					for (int j = openpoint; j < closepoint; j++)
					{
						if ((storeline1[j].find("<hkparam name=\"targetPosition\">", 0) != string::npos) && (j != openpoint))
						{
							output.push_back("				<hkobject>");
							output.push_back("					<hkparam name=\"controlData\">");
							output.push_back("						<hkobject>");
						}
						else if ((storeline1[j - 1].find("<hkparam name=\"fixUp\">", 0) != string::npos) && (j != openpoint))
						{
							output.push_back("						</hkobject>");
							output.push_back("					</hkparam>");
						}
						else if ((storeline1[j - 1].find("<hkparam name=\"enable\">", 0) != string::npos) && (j > 6) && (j != openpoint))
						{
							output.push_back("				</hkobject>");
						}

						output.push_back(storeline1[j]);
					}
					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}

				if (storeline2[i].find("<hkparam name=\"targetPosition\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
					output.push_back("					<hkparam name=\"controlData\">");
					output.push_back("						<hkobject>");
				}
				else if (storeline2[i - 1].find("<hkparam name=\"fixUp\">", 0) != string::npos)
				{
					output.push_back("						</hkobject>");
					output.push_back("					</hkparam>");
				}
				else if ((storeline2[i - 1].find("<hkparam name=\"enable\">", 0) != string::npos) && (i > 6))
				{
					output.push_back("				</hkobject>");
				}

				output.push_back(storeline2[i]);
			}

			curline++;

			if (i == storeline2.size() - 1)
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;
					for (int j = openpoint; j < closepoint; j++)
					{
						if ((storeline1[j].find("<hkparam name=\"targetPosition\">", 0) != string::npos) && (j != openpoint))
						{
							output.push_back("				<hkobject>");
							output.push_back("					<hkparam name=\"controlData\">");
							output.push_back("						<hkobject>");
						}
						else if ((storeline1[j - 1].find("<hkparam name=\"fixUp\">", 0) != string::npos) && (j != openpoint))
						{
							output.push_back("						</hkobject>");
							output.push_back("					</hkparam>");
						}
						else if ((storeline1[j - 1].find("<hkparam name=\"enable\">", 0) != string::npos) && (j != openpoint))
						{
							output.push_back("				</hkobject>");
						}

						output.push_back(storeline1[j]);
					}
					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}

				if (storeline2[i].find("<hkparam name=\"enable\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}
			}

		}
		else // new data
		{
			if (storeline2[i - 1].find("<hkparam name=\"fixUp\">", 0) != string::npos)
			{
				output.push_back("						</hkobject>");
				output.push_back("					</hkparam>");
			}
			else if (storeline2[i - 1].find("<hkparam name=\"enable\">", 0) != string::npos)
			{
				output.push_back("				</hkobject>");
			}

			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = curline;
				IsEdited = true;
				open = true;
			}

			if (storeline2[i].find("<hkparam name=\"targetPosition\">", 0) != string::npos)
			{
				output.push_back("				<hkobject>");
				output.push_back("					<hkparam name=\"controlData\">");
				output.push_back("						<hkobject>");
			}

			output.push_back(storeline2[i]);

			if (i == storeline2.size() - 1)
			{
				if (open)
				{
					if (storeline2[i].find("<hkparam name=\"enable\">", 0) != string::npos)
					{
						output.push_back("				</hkobject>");
					}

					closepoint = curline;
					if ((IsChanged) && (closepoint != openpoint))
					{
						output.push_back("<!-- ORIGINAL -->");
						for (int j = openpoint; j < closepoint; j++)
						{
							if ((storeline1[j].find("<hkparam name=\"targetPosition\">", 0) != string::npos) && (j != openpoint))
							{
								output.push_back("				<hkobject>");
								output.push_back("					<hkparam name=\"controlData\">");
								output.push_back("						<hkobject>");
							}

							output.push_back(storeline1[j]);

							if (storeline1[j].find("<hkparam name=\"fixUp\">", 0) != string::npos)
							{
								output.push_back("						</hkobject>");
								output.push_back("					</hkparam>");
							}
							else if (storeline1[j].find("<hkparam name=\"enable\">", 0) != string::npos)
							{
								output.push_back("				</hkobject>");
							}
						}
					}
					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
		}
	}
	
	if (open) // close unclosed edits
	{
		if (output.back().find("<hkparam name=\"enable\">", 0) != string::npos)
		{
			output.push_back("				</hkobject>");
		}

		if (IsChanged)
		{
			closepoint = curline;
			if (closepoint != openpoint)
			{
				output.push_back("<!-- ORIGINAL -->");
				for (int j = openpoint; j < closepoint; j++)
				{
					if ((storeline1[j].find("<hkparam name=\"targetPosition\">", 0) != string::npos) && (j != openpoint))
					{
						output.push_back("				<hkobject>");
						output.push_back("					<hkparam name=\"controlData\">");
						output.push_back("						<hkobject>");
					}

					output.push_back(storeline1[j]);

					if (storeline1[j].find("<hkparam name=\"fixUp\">", 0) != string::npos)
					{
						output.push_back("						</hkobject>");
						output.push_back("					</hkparam>");
					}
					else if (storeline1[j].find("<hkparam name=\"enable\">", 0) != string::npos)
					{
						output.push_back("				</hkobject>");
					}

				}
			}
			IsChanged = false;
		}
		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	NemesisReaderFormat(output);

	// stage 3 output if it is edited
	int part = 0;
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	bool closeOri = false;
	bool closeEdit = false;

	if (IsEdited)
	{
		ofstream outputfile(filename);
		if (outputfile.is_open())
		{
			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("<hkparam name=\"hands\" numelements=", 0) != string::npos)
				{
					if (output[i - 1].find("OPEN", 0) != string::npos)
					{
						if (output[i].find("</hkparam>", 0) != string::npos)
						{
							closeEdit = true;
						}
						else
						{
							closeEdit = false;
						}
					}
					else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
					{
						if (output[i].find("</hkparam>", 0) != string::npos)
						{
							closeOri = true;
						}
						else
						{
							closeOri = false;
						}
					}
					else
					{
						if (output[i].find("</hkparam>", 0) != string::npos)
						{
							closeEdit = true;
							closeOri = true;
						}
						else
						{
							closeEdit = false;
							closeOri = false;
						}
					}
				}

				outputfile << output[i] << "\n";
				
				if (i < output.size() - 2)
				{
					if ((output[i + 1].find("ORIGINAL", 0) != string::npos) && (!closeEdit) && (output[i + 2].find("<hkparam name=\"hands\" numelements=", 0) != string::npos) && (output[i + 2].find("</hkparam>", 0) != string::npos))
					{
						outputfile << "			</hkparam>" << "\n";
						closeEdit = true;
					}
				}

				if (i == output.size() - 1)
				{
					if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))  // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
				}
			}
			outputfile << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbHandIkControlsModifier Output Not Found (New Edited File: " << editedfile << ")" << endl;
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