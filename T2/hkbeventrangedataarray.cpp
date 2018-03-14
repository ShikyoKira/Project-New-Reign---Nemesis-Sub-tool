#include "hkbeventrangedataarray.h"
#include "Global.h"

using namespace std;

hkbeventrangedataarray::hkbeventrangedataarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "av" + to_string(functionlayer) + ">";

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
}

void hkbeventrangedataarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventRangeDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	vector<string> storeline;
	string line;
	payloadcount = 0;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));
				if (payload[payloadcount] != "null")
				{
					referencingIDs[payload[payloadcount]].push_back(id);
				}
				payloadcount++;
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbEventRangeDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbEventRangeDataArray Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbEventRangeDataArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventrangedataarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventRangeDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;
	payloadcount = 0;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (int i = 0; i < size; i++)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

				if (payload[payloadcount] != "null")
				{
					if (!exchangeID[payload[payloadcount]].empty())
					{
						int tempint = line.find(payload[payloadcount]);
						payload[payloadcount] = exchangeID[payload[payloadcount]];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload[payloadcount]);
					}
					referencingIDs[payload[payloadcount]].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbEventRangeDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
	{
		IsForeign[id] = false;

		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbEventRangeDataArray(newID: " << id << ") with hkbEventRangeDataArray(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID
		
		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
			}
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
			cout << "ERROR: hkbEventRangeDataArray Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbEventRangeDataArray Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbEventRangeDataArray(newID: " << id << ") with hkbEventRangeDataArray(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbEventRangeDataArray Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbEventRangeDataArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventrangedataarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbEventRangeDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);
	payloadcount = 0;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

				if (payload[payloadcount] != "null")
				{
					if (!exchangeID[payload[payloadcount]].empty())
					{
						payload[payloadcount] = exchangeID[payload[payloadcount]];
					}
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbEventRangeDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbEventRangeDataArray(ID: " << id << ") is complete!" << endl;
	}
}

int hkbeventrangedataarray::GetPayloadCount()
{
	return payloadcount;
}

string hkbeventrangedataarray::GetPayload(int child)
{
	return payload[child];
}

bool hkbeventrangedataarray::IsPayloadNull(int child)
{
	if (payload[child].find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbeventrangedataarray::GetAddress()
{
	return address;
}

bool hkbeventrangedataarray::IsNegate()
{
	return IsNegated;
}

void hkbEventRangeDataArrayExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit hkbEventRangeDataArray Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit hkbEventRangeDataArray Output Not Found (Edited File: " << editedfile << ")" << endl;
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
							if ((storeline2[i].find("<hkparam name=\"upperBound\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
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
					if (storeline2[i].find("<hkparam name=\"upperBound\">", 0) != string::npos)
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
							if (storeline2[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
							{
								output.push_back("				</hkobject>");
							}
							output.push_back("<!-- ORIGINAL -->");
							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
								if (storeline1[j].find("<hkparam name=\"eventMode\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}
							}
						}
					}
					else
					{
						if (storeline2[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
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

	for (unsigned int j = 0; j < output.size(); j++)
	{
		if ((output[j].find("<hkparam name=\"id\">", 0) != string::npos) && (output[j].find("<hkparam name=\"id\">-1</hkparam>", 0) == string::npos))
		{
			usize eventpos = output[j].find("id\">") + 4;
			string eventid = output[j].substr(eventpos, output[j].find("</hkparam>"));

			if (eventID[eventid].length() != 0)
			{
				output[j].replace(eventpos, output[j].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
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
				if (output[i].find("<hkparam name=\"upperBound\">", 0) != string::npos)
				{
					if (output[i + 1].find("CLOSE", 0) != string::npos)
					{
						if (output[i - 1].find("ORIGINAL", 0) == string::npos)
						{
							outputfile << "				<hkobject>" << "\n";
						}
						outputfile << output[i] << "\n";
						outputfile << "<!-- CLOSE -->" << "\n";
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
					}
				}
				else if (output[i].find("<hkparam name=\"event\">", 0) != string::npos)
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
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
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
				else if (output[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
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
						else if (output[i + 1].find("<hkobject>", 0) != string::npos)
						{
							if (output[i + 2].find("ORIGINAL", 0) != string::npos)
							{
								outputfile << output[i] << "\n";
								outputfile << "<!-- ORIGINAL -->" << "\n";
								i += 2;
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
			cout << "ERROR: Edit hkbEventRangeDataArray Output Not Found (New Edited File: " << editedfile << ")" << endl;
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