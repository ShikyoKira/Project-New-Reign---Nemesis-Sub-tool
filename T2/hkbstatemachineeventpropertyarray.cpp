#include "hkbstatemachineeventpropertyarray.h"
#include "Global.h"

using namespace std;

hkbstatemachineeventpropertyarray::hkbstatemachineeventpropertyarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "o" + to_string(functionlayer) + ">";

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

void hkbstatemachineeventpropertyarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineEventPropertyArray(ID: " << id << ") has been initialized!" << endl;
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
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));
				if (payload[payloadcount] != "null")
				{
					if (!exchangeID[payload[payloadcount]].empty())
					{
						payload[payloadcount] = exchangeID[payload[payloadcount]];
					}
					referencingIDs[payload[payloadcount]].push_back(id);
				}
				payloadcount++;
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineEventPropertyArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbStateMachineEventPropertyArray Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineEventPropertyArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachineeventpropertyarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineEventPropertyArray(ID: " << id << ") has been initialized!" << endl;
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
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));
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
				payloadcount++;
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineEventPropertyArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbStateMachineEventPropertyArray(newID: " << id << ") with hkbStateMachineEventPropertyArray(oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbStateMachineEventPropertyArray Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbStateMachineEventPropertyArray Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineEventPropertyArray(newID: " << id << ") with hkbStateMachineEventPropertyArray(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbStateMachineEventPropertyArray Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineEventPropertyArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachineeventpropertyarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachineEventPropertyArray(ID: " << id << ") has been initialized!" << endl;
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
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload[payloadcount] != "null")
				{
					if (!exchangeID[payload[payloadcount]].empty())
					{
						payload[payloadcount] = exchangeID[payload[payloadcount]];
					}
				}
				payloadcount++;
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachineEventPropertyArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachineEventPropertyArray(ID: " << id << ") is complete!" << endl;
	}
}

int hkbstatemachineeventpropertyarray::GetPayloadCount()
{
	return payloadcount;
}

string hkbstatemachineeventpropertyarray::GetPayload(int child)
{
	return payload[child];
}

bool hkbstatemachineeventpropertyarray::IsPayloadNull(int child)
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

string hkbstatemachineeventpropertyarray::GetAddress()
{
	return address;
}

bool hkbstatemachineeventpropertyarray::IsNegate()
{
	return IsNegated;
}

void hkbStateMachineEventPropertyArrayExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit hkbStateMachineEventPropertyArray Input Not Found (Original File: " << originalfile << ")" << endl;
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
		cout << "ERROR: Edit hkbStateMachineEventPropertyArray Output Not Found (Edited File: " << editedfile << ")" << endl;
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
				if (output[i].find("<hkparam name=\"id\">", 0) != string::npos && output[i].find("<hkparam name=\"id\">-1</hkparam>", 0) == string::npos)
				{
					outputfile << "				<hkobject>" << "\n";
					usize eventpos = output[i].find("id\">") + 4;
					string eventid = output[i].substr(eventpos, output[i].find("</hkparam>"));

					if (eventID[eventid].length() != 0)
					{
						output[i].replace(eventpos, output[i].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
					}

					outputfile << output[i] << "\n";
				}
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					outputfile << output[i] << "\n";
					outputfile << "				</hkobject>" << "\n";
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
			cout << "ERROR: Edit hkbStateMachineEventPropertyArray Output Not Found (New Edited File: " << editedfile << ")" << endl;
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