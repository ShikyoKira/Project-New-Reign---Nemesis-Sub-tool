#include "hkbexpressiondataarray.h"
#include "Global.h"

using namespace std;

hkbexpressiondataarray::hkbexpressiondataarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "an" + to_string(functionlayer) + ">";

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
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare, true);

		if (compare)
		{
			if (statusChange)
			{
				Dummy(dummyID);
			}

			if (IsForeign[id])
			{
				address = preaddress;
			}
			else if (!statusChange)
			{
				IsNegated = true;
			}
		}
		else
		{
			IsNegated = true;
		}
	}
}

void hkbexpressiondataarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbExpressionDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbExpressionDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbExpressionDataArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbexpressiondataarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbExpressionDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	string line;

	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbExpressionDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(addressChange.find(address));
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbExpressionDataArray(newID: " << id << ") with hkbExpressionDataArray(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
		{
			FunctionLineNew[tempid].push_back(FunctionLineEdited[id][i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbExpressionDataArray(newID: " << id << ") with hkbExpressionDataArray(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = FunctionLineEdited[id];
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbExpressionDataArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbexpressiondataarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbExpressionDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbExpressionDataArray Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbExpressionDataArray(ID: " << id << ") is complete!" << endl;
	}
}

string hkbexpressiondataarray::GetAddress()
{
	return address;
}

bool hkbexpressiondataarray::IsNegate()
{
	return IsNegated;
}

void hkbExpressionDataArrayExport(string id)
{
	// stage 1 reading
	vector<string> storeline1;
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbExpressionDataArray Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	vector<string> storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbExpressionDataArray Output Not Found (ID: " << id << ")" << endl;
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
							if ((storeline2[i].find("<hkparam name=\"expression\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
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
					if (storeline2[i].find("<hkparam name=\"expression\">", 0) != string::npos)
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
		if ((output[j].find("<hkparam name=\"assignmentEventIndex\">", 0) != string::npos) && (output[j].find("<hkparam name=\"assignmentEventIndex\">-1</hkparam>", 0) == string::npos))
		{
			usize eventpos = output[j].find("assignmentEventIndex\">") + 22;
			string eventid = output[j].substr(eventpos, output[j].find("</hkparam>") - eventpos);

			if (eventID[eventid].length() != 0)
			{
				output[j].replace(eventpos, output[j].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
			}
			else
			{
				cout << "ERROR: Invalid event id. Please ensure that event id is valid(ID: " << id << ")" << endl;
				Error = true;
				return;
			}
		}
		else if ((output[j].find("<hkparam name=\"assignmentVariableIndex\">", 0) != string::npos) && (output[j].find("-1", 0) == string::npos))
		{
			usize varpos = output[j].find("assignmentVariableIndex\">") + 25;
			string varID = output[j].substr(varpos, output[j].find("</hkparam>") - varpos);

			if (variableID[varID].length() != 0)
			{
				output[j].replace(varpos, output[j].find("</hkparam>") - varpos, "$variableID[" + variableID[varID] + "]$");
			}
			else
			{
				cout << "ERROR: Invalid variable id. Please ensure that variable id is valid(ID: " << id << ")" << endl;
				Error = true;
				return;
			}
		}
	}
	
	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("<hkparam name=\"expression\">", 0) != string::npos)
				{
					if (output[i + 1].find("CLOSE", 0) != string::npos)
					{
						if (output[i - 1].find("ORIGINAL", 0) == string::npos)
						{
							fwrite << "				<hkobject>" << "\n";
						}

						fwrite << output[i] << "\n";
						fwrite << "<!-- CLOSE -->" << "\n";
						i++;
					}
					else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
					{
						if (output[i - 1].find("OPEN", 0) == string::npos)
						{
							fwrite << "				<hkobject>" << "\n";
						}

						fwrite << output[i] << "\n";
						fwrite << "<!-- ORIGINAL -->" << "\n";
						i++;
					}
					else
					{
						if ((output[i - 1].find("ORIGINAL", 0) == string::npos) && (output[i - 1].find("OPEN", 0) == string::npos) && (output[i - 1].find("<hkobject>", 0) == string::npos))
						{
							fwrite << "				<hkobject>" << "\n";
						}

						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
				{
					if (i != output.size() - 1)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							fwrite << "				</hkobject>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else if (output[i + 1].find("</hkobject>", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
						}
						else if (output[i + 1].find("<hkobject>", 0) != string::npos)
						{
							if (output[i + 2].find("ORIGINAL", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
								fwrite << "<!-- ORIGINAL -->" << "\n";
								i += 2;
							}
							else
							{
								fwrite << output[i] << "\n";
								fwrite << "				</hkobject>" << "\n";
							}
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "				</hkobject>" << "\n";
						}
					}
					else
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}
			}

			fwrite << "			</hkparam>" << "\n";
			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbExpressionDataArray Output Not Found (File: " << filename << ")" << endl;
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