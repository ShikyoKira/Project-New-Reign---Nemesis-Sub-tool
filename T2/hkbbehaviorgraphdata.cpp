#include "hkbbehaviorgraphdata.h"
#include "Global.h"

using namespace std;

hkbbehaviorgraphdata::hkbbehaviorgraphdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "f" + to_string(functionlayer) + ">";

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

void hkbbehaviorgraphdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphData(ID: " << id << ") has been initialized!" << endl;
	}

	vector<string> storeline;
	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableInitialValues\">", 0) != string::npos)
			{
				variableinitialvalues = line.substr(41, line.find("</hkparam>") - 41);
				referencingIDs[variableinitialvalues].push_back(id);
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				stringdata = line.substr(30, line.find("</hkparam>") - 30);
				referencingIDs[stringdata].push_back(id);
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbBehaviorGraphData Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraphData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphData(ID: " << id << ") has been initialized!" << endl;
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

			if (line.find("<hkparam name=\"variableInitialValues\">", 0) != string::npos)
			{
				variableinitialvalues = line.substr(41, line.find("</hkparam>") - 41);
				if (!exchangeID[variableinitialvalues].empty())
				{
					int tempint = line.find(variableinitialvalues);
					variableinitialvalues = exchangeID[variableinitialvalues];
					line.replace(tempint, line.find("</hkparam>") - tempint, variableinitialvalues);
				}
				referencingIDs[variableinitialvalues].push_back(id);
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				stringdata = line.substr(30, line.find("</hkparam>") - 30);
				if (!exchangeID[stringdata].empty())
				{
					int tempint = line.find(stringdata);
					stringdata = exchangeID[stringdata];
					line.replace(tempint, line.find("</hkparam>") - tempint, stringdata);
				}
				referencingIDs[stringdata].push_back(id);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbBehaviorGraphData(newID: " << id << ") with hkbBehaviorGraphData(oldID: " << tempid << ")" << endl;
		}

		referencingIDs[variableinitialvalues].pop_back();
		referencingIDs[variableinitialvalues].push_back(tempid);

		referencingIDs[stringdata].pop_back();
		referencingIDs[stringdata].push_back(tempid);

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
			cout << "ERROR: hkbBehaviorGraphData Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbBehaviorGraphData Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraphData(newID: " << id << ") with hkbBehaviorGraphData(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbBehaviorGraphData Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraphData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBehaviorGraphData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.find("<hkparam name=\"variableInitialValues\">", 0) != string::npos)
			{
				variableinitialvalues = line.substr(41, line.find("</hkparam>") - 41);
				if (!exchangeID[variableinitialvalues].empty())
				{
					variableinitialvalues = exchangeID[variableinitialvalues];
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				stringdata = line.substr(30, line.find("</hkparam>") - 30);
				if (!exchangeID[stringdata].empty())
				{
					stringdata = exchangeID[stringdata];
				}
				break;
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbBehaviorGraphData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBehaviorGraphData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbbehaviorgraphdata::GetAddress()
{
	return address;
}

string hkbbehaviorgraphdata::GetVariableInitialValues()
{
	return "#" + boost::regex_replace(string(variableinitialvalues), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbbehaviorgraphdata::GetStringData()
{
	return "#" + boost::regex_replace(string(stringdata), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbbehaviorgraphdata::IsNegate()
{
	return IsNegated;
}

void hkbBehaviorGraphDataExport(string originalfile, string editedfile, string id)
{
	//stage 1 reading
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
		cout << "ERROR: Edit hkbBehaviorGraphData Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit hkbBehaviorGraphData Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vector<string> output;
	bool IsEdited = false;
	bool IsChanged = false;
	bool open = false;
	int curline = 3;
	int part = 0;
	int openpoint;
	int closepoint;

	output.push_back(storeline2[0]);
	output.push_back(storeline2[1]);
	if ((storeline1[2].find(storeline2[2], 0) == string::npos) || (storeline1[2].length() != storeline2[2].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[2]);
		if ((storeline1[2].find("</hkparam>", 0) != string::npos) && (storeline2[3].find("numelements=", 0) == string::npos))
		{
			IsChanged = true;
			openpoint = 2;
			open = true;
		}
		else
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back(storeline1[2]);
			output.push_back("<!-- CLOSE -->");
			open = false;
		}
		IsEdited = true;
	}
	else
	{
		output.push_back(storeline2[2]);
	}

	for (unsigned int i = 3; i < storeline2.size(); i++)
	{
		if ((storeline1[curline].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) == string::npos) && (part == 0)) // existing variableInfos value
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("numelements=", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
			}
			else
			{
				if (open)
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");
					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}
					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back(storeline2[i]);
			}
			curline++;

			if ((open) && (storeline2[i + 1].find("numelements=", 0) != string::npos))
			{
				closepoint = curline;
				output.push_back("<!-- ORIGINAL -->");
				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
				output.push_back("<!-- CLOSE -->");
				IsChanged = false;
				open = false;
			}
		}
		else if ((storeline1[curline].find("<hkparam name=\"eventInfos\" numelements=", 0) == string::npos) && (part == 1)) // existing characterPropertyInfos value
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("numelements=", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
			}
			else
			{
				if (open)
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");
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

			if ((open) && (storeline2[i + 1].find("numelements=", 0) != string::npos))
			{
				closepoint = curline;
				output.push_back("<!-- ORIGINAL -->");
				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
				output.push_back("<!-- CLOSE -->");
				IsChanged = false;
				open = false;
			}
		}
		else if ((storeline1[curline].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) == string::npos) && (part == 2)) // existing eventInfos value
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("numelements=", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
			}
			else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (open)
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");
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
			else
			{
				output.push_back(storeline2[i]);
			}
			curline++;

			if ((open) && (storeline2[i + 1].find("numelements=", 0) != string::npos))
			{
				closepoint = curline;
				output.push_back("<!-- ORIGINAL -->");
				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
				output.push_back("<!-- CLOSE -->");
				IsChanged = false;
				open = false;
			}
		}
		else if (part == 3) // existing leftover settings
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
		}
		else // added variable value
		{
			if (storeline2[i].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("numelements=", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
				part = 1;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("numelements=", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
				part = 2;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}
				output.push_back(storeline2[i]);
				part = 3;
				curline++;
			}
			else
			{
				if (part == 0)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos)
					{
						output.push_back("				</hkobject>");
						if (IsChanged)
						{
							if (openpoint != curline)
							{
								output.push_back("<!-- ORIGINAL -->");
								for (int j = openpoint; j < curline; j++)
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
				else if (part == 1)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
					{
						output.push_back("				</hkobject>");
						if (IsChanged)
						{
							if (openpoint != curline)
							{
								output.push_back("<!-- ORIGINAL -->");
								for (int j = openpoint; j < curline; j++)
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
				else if (part == 2)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos)
					{
						if (IsChanged)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < curline; j++)
							{
								output.push_back(storeline1[j]);
							}
							IsChanged = false;
						}
						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}
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

	// stage 3 output if it is edited
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	bool closeOri = false;
	bool closeEdit = false;

	if (IsEdited)
	{
		ofstream outputfile(filename);
		if (outputfile.is_open())
		{
			part = 0;
			for (unsigned int i = 0; i < output.size(); i++)
			{ 
				if (i < output.size() - 1)
				{
					if ((output[i + 1].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"variableInfos\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
				}

				if (part == 0)
				{
					if ((output[i].find("<hkparam name=\"variableInfos\" numelements=", 0) != string::npos) || (output[i].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos))
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

					if ((output[i].find("<hkparam name=\"role\">", 0) != string::npos) && (output[i].find("</hkparam>", 0) == string::npos))
					{
						outputfile << "				<hkobject>" << "\n";
						outputfile << output[i] << "\n";
						outputfile << "						<hkobject>" << "\n";
					}
					else if (output[i].find("<hkparam name=\"flags\">", 0) != string::npos)
					{
						outputfile << output[i] << "\n";

						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							outputfile << "<!-- CLOSE -->" << "\n";
							i++;
						}

						if (output[i + 1].find("ORIGINAL", 0) == string::npos)
						{
							outputfile << "						</hkobject>" << "\n";
							outputfile << "					</hkparam>" << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"type\">", 0) != string::npos)
					{
						outputfile << output[i] << "\n";

						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							outputfile << "<!-- CLOSE -->" << "\n";
							i++;
						}
						
						if ((output[i + 1].find("</hkobject>", 0) == string::npos) && (output[i + 1].find("ORIGINAL", 0) == string::npos))
						{
							outputfile << "				</hkobject>" << "\n";
						}
					}
					else
					{
						outputfile << output[i] << "\n";
					}

					if (output[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
					{
						part = 1;
					}
				}
				else if (part == 1)
				{
					if (output[i].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
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

					if (output[i].find("<hkparam name=\"flags\">", 0) != string::npos)
					{
						outputfile << "				<hkobject>" << "\n";
						outputfile << output[i] << "\n";
						outputfile << "				</hkobject>" << "\n";
					}
					else
					{
						outputfile << output[i] << "\n";
					}

					if (output[i + 1].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos)
					{
						part = 2;
					}
				}
				else
				{
					outputfile << output[i] << "\n";
				}

				if (i < output.size() - 1) 
				{
					if (output[i + 1].find("numelements=", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkparam name=\"type\">", 0) != string::npos) || (output[i].find("<hkparam name=\"flags\">", 0) != string::npos))
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
			}
			outputfile << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbBehaviorGraphData Output Not Found (New Edited File: " << editedfile << ")" << endl;
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