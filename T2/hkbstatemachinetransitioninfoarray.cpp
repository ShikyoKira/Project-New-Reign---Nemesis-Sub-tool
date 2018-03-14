#include "hkbstatemachinetransitioninfoarray.h"
#include "Global.h"

using namespace std;

hkbstatemachinetransitioninfoarray::hkbstatemachinetransitioninfoarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "q" + to_string(functionlayer) + ">";

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

void hkbstatemachinetransitioninfoarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineTransitionInfoArray(ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	vector<string> storeline;
	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();
		storeline.push_back(FunctionLineOriginal[id][0]);

		for (int i = 1; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("#", 0) != string::npos)
			{
				int tempInt = line.find("#");
				string reference = line.substr(tempInt, line.find("</hkparam>") - tempInt);

				if (line.find("name=\"transition\">", 0) != string::npos)
				{
					transition.push_back(reference);
				}
				else
				{
					condition.push_back(reference);
				}

				if (!IsReferenceExist[reference])
				{
					referencingIDs[reference].push_back(id);
					IsReferenceExist[reference] = true;
				}
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbStateMachineTransitionInfoArray Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineTransitionInfoArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinetransitioninfoarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineTransitionInfoArray(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	unordered_map<string, bool> IsReferenceExist;
	vector<string> newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();
		newline.push_back(FunctionLineEdited[id][0]);

		for (int i = 1; i < size; i++)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("#", 0) != string::npos)
			{
				int tempInt = line.find("#");
				string reference = line.substr(tempInt, line.find("</hkparam>") - tempInt);

				if (!exchangeID[reference].empty())
				{
					int tempint = line.find(reference);
					reference = exchangeID[reference];
					line.replace(tempint, line.find("</hkparam>") - tempint, reference);
				}

				if (line.find("name=\"transition\">", 0) != string::npos)
				{
					transition.push_back(reference);
				}
				else
				{
					condition.push_back(reference);
				}

				if (!IsReferenceExist[reference])
				{
					parent[reference] = id;
					referencingIDs[reference].push_back(id);
					IsReferenceExist[reference] = true;
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
	{
		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineTransitionInfoArray(newID: " << id << ") with hkbStateMachineTransitionInfoArray(oldID: " << tempid << ")" << endl;
		}

		for (unsigned int i = 0; i < transition.size(); i++)
		{
			referencingIDs[transition[i]].pop_back();
			referencingIDs[transition[i]].push_back(tempid);
		}

		for (unsigned int i = 0; i < condition.size(); i++)
		{
			referencingIDs[condition[i]].pop_back();
			referencingIDs[condition[i]].push_back(tempid);
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		// stage 3
		ofstream output("new/" + tempid + ".txt"); // output stored function data
		if (output.is_open())
		{
			output << FunctionLineOriginal[tempid][0] << "\n";
			for (unsigned int i = 1; i < newline.size(); i++)
			{
				output << newline[i] << "\n";
			}
			output.close();
		}
		else
		{
			cout << "ERROR: hkbStateMachineTransitionInfoArray Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}
		
		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineTransitionInfoArray(newID: " << id << ") with hkbStateMachineTransitionInfoArray(oldID: " << tempid << ") is complete!" << endl;
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
		}
		else
		{
			cout << "ERROR: hkbStateMachineTransitionInfoArray Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineTransitionInfoArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinetransitioninfoarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachineTransitionInfoArray(ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;
	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if ((line.find("#", 0) != string::npos) && (line.find("signature", 0) == string::npos))
			{
				int tempInt = line.find("#");
				string reference = line.substr(tempInt, line.find("</hkparam>") - tempInt);
				if (reference != "null")
				{
					if (!exchangeID[reference].empty())
					{
						reference = exchangeID[reference];
					}
					if (!IsReferenceExist[reference])
					{
						if (line.find("name=\"transition\">", 0) != string::npos)
						{
							transition.push_back(reference);
						}
						else
						{
							condition.push_back(reference);
						}
						IsReferenceExist[reference] = true;
					}
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachineTransitionInfoArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachineTransitionInfoArray(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachinetransitioninfoarray::GetTransition(int number)
{
	return "#" + boost::regex_replace(string(transition[number]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachinetransitioninfoarray::GetTransitionCount()
{
	return transition.size();
}

string hkbstatemachinetransitioninfoarray::GetCondition(int number)
{
	return "#" + boost::regex_replace(string(condition[number]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachinetransitioninfoarray::GetConditionCount()
{
	return condition.size();
}

string hkbstatemachinetransitioninfoarray::GetAddress()
{
	return address;
}

bool hkbstatemachinetransitioninfoarray::IsNegate()
{
	return IsNegated;
}

void hkbStateMachineTransitionInfoArrayExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vector<string> output;
	bool newtransition = false;
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
		if (!newtransition) // existing data
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
							if ((storeline2[i].find("<hkparam name=\"triggerInterval\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
							{
								output.push_back("				<hkobject>");
							}
							output.push_back("<!-- ORIGINAL -->");
							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}
					}
					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					if (storeline2[i].find("<hkparam name=\"triggerInterval\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
					}
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}
			output.push_back(storeline2[i]);

			if (curline != storeline1.size() - 1)
			{
				curline++;
			}
			else
			{
				newtransition = true;
			}

			if (i == storeline2.size() - 1) // if close no new element
			{
				if (open)
				{
					if (IsChanged)
					{
						closepoint = curline + 1;
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
					IsChanged = false;
					open = false;
				}
			}
		}
		else // new added data
		{
			if (i != storeline2.size() - 1)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("				<hkobject>");
					IsEdited = true;
					open = true;
				}
				output.push_back(storeline2[i]);
			}
			else
			{
				output.push_back(storeline2[i]);
				if (open)
				{
					if (IsChanged)
					{
						closepoint = curline + 1;
						if (closepoint != openpoint)
						{
							if (storeline2[i].find("<hkparam name=\"flags\">", 0) != string::npos)
							{
								output.push_back("				</hkobject>");
							}
							output.push_back("<!-- ORIGINAL -->");
							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
								if (storeline1[j].find("<hkparam name=\"flags\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}
							}
						}
					}
					else
					{
						if (storeline2[i].find("<hkparam name=\"flags\">", 0) != string::npos)
						{
							output.push_back("				</hkobject>");
						}
					}
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
		}
	}

	for (unsigned int j = 0; j < output.size(); j++) // changing newID to modCode ID
	{
		if (output[j].find("#", 0) != string::npos)
		{
			usize tempint = 0;
			int position = 0;
			usize size = count(output[j].begin(), output[j].end(), '#');

			for (unsigned int i = 0; i < size; i++)
			{
				position = output[j].find("#", tempint);
				tempint = output[j].find("#", position + 1);
				string tempID;

				if (tempint == -1)
				{
					string templine;

					if (output[j].find("signature", 0) != string::npos)
					{
						templine = output[j].substr(0, output[j].find("class"));
					}
					else
					{
						templine = output[j].substr(position, tempint - position - 1);
					}

					tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
				}
				else
				{
					tempID = output[j].substr(position, tempint - position - 1);
				}

				int tempLength = tempID.length();
				string strID = tempID.substr(1, tempLength - 1);
				int intID = stoi(strID);

				if (intID > 10000)
				{
					int position2 = output[j].find(tempID);
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

					output[j].replace(position2, tempLength, modID);
				}
			}
			
			if (((output[j].find("<hkparam name=\"enterEventId\">", 0) != string::npos) || (output[j].find("<hkparam name=\"exitEventId\">", 0) != string::npos) || (output[j].find("<hkparam name=\"eventId\">", 0) != string::npos)) && (storeline2[j].find("<hkparam name=\"enterEventId\">-1</hkparam>", 0) == string::npos) && (storeline2[j].find("<hkparam name=\"exitEventId\">-1</hkparam>", 0) == string::npos) && (storeline2[j].find("<hkparam name=\"eventId\">-1</hkparam>", 0) == string::npos))
			{
				usize eventpos = output[j].find("Id\">") + 4;
				string eventid = output[j].substr(eventpos, output[j].find("</hkparam>"));

				if (eventID[eventid].length() != 0)
				{
					output[j].replace(eventpos, output[j].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
				}
			}
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
				if (output[i].find("<hkparam name=\"triggerInterval\">", 0) != string::npos)
				{
					if (output[i + 1].find("CLOSE", 0) != string::npos)
					{
						if (output[i - 1].find("ORIGINAL", 0) == string::npos)
						{
							outputfile << "				<hkobject>" << "\n";
						}
						outputfile << output[i] << "\n";
						outputfile << "<!-- CLOSE -->" << "\n";
						outputfile << "						<hkobject>" << "\n";
						i++;
					}
					else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
					{
						if (output[i - 1].find("OPEN", 0) == string::npos)
						{
							outputfile << "				<hkobject>" << "\n";
						}
						outputfile << output[i] << "\n";
						outputfile << "<!-- ORIGINAL -->" << "\n";
						i++;
					}
					else
					{
						if ((output[i - 1].find("ORIGINAL", 0) == string::npos) && (output[i - 1].find("OPEN", 0) == string::npos) && (output[i - 1].find("<hkobject>", 0) == string::npos))
						{
							outputfile << "				<hkobject>" << "\n";
						}
						outputfile << output[i] << "\n";
						outputfile << "						<hkobject>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"initiateInterval\">", 0) != string::npos)
				{
					if (output[i + 1].find("CLOSE", 0) != string::npos)
					{
						outputfile << output[i] << "\n";
						outputfile << "<!-- CLOSE -->" << "\n";
						outputfile << "						<hkobject>" << "\n";
						i++;
					}
					else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
					{
						outputfile << output[i] << "\n";
						outputfile << "<!-- ORIGINAL -->" << "\n";
						i++;
					}
					else
					{
						outputfile << output[i] << "\n";
						outputfile << "						<hkobject>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"exitTime\">", 0) != string::npos)
				{
					if (output[i + 1].find("CLOSE", 0) != string::npos)
					{
						outputfile << output[i] << "\n";
						outputfile << "<!-- CLOSE -->" << "\n";
						outputfile << "						</hkobject>" << "\n";
						outputfile << "					</hkparam>" << "\n";
						i++;
					}
					else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
					{
						outputfile << output[i] << "\n";
						outputfile << "<!-- ORIGINAL -->" << "\n";
						i++;
					}
					else
					{
						outputfile << output[i] << "\n";
						outputfile << "						</hkobject>" << "\n";
						outputfile << "					</hkparam>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"flags\">", 0) != string::npos)
				{
					if (i != output.size() - 1)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							outputfile << output[i] << "\n";
							outputfile << "<!-- CLOSE -->" << "\n";
							outputfile << "				</hkobject>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							outputfile << output[i] << "\n";
							outputfile << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else if (output[i + 1].find("</hkobject>", 0) != string::npos)
						{
							outputfile << output[i] << "\n";
						}
						else
						{
							outputfile << output[i] << "\n";
							outputfile << "				</hkobject>" << "\n";
						}
					}
					else
					{
						outputfile << output[i] << "\n";
						outputfile << "				</hkobject>" << "\n";
					}
				}
				else
				{
					outputfile << output[i] << "\n";
				}
			}
			outputfile << "			</hkparam>" << "\n";
			outputfile << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Output Not Found (New Edited File: " << editedfile << ")" << endl;
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