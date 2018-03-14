#include "hkbsensehandlemodifier.h"
#include "Global.h"

using namespace std;

hkbsensehandlemodifier::hkbsensehandlemodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "al" + to_string(functionlayer) + ">";

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

void hkbsensehandlemodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbSenseHandleModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload = line.substr(29, line.find("</hkparam>") - 29);
				if (payload != "null")
				{
					referencingIDs[payload].push_back(id);
				}
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbSenseHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		cout << "ERROR: hkbSenseHandleModifier Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbSenseHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbsensehandlemodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbSenseHandleModifier(ID: " << id << ") has been initialized!" << endl;
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
					referencingIDs[payload].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbSenseHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbSenseHandleModifier(newID: " << id << ") with hkbSenseHandleModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		if (payload != "null")
		{
			referencingIDs[payload].pop_back();
			referencingIDs[payload].push_back(tempid);
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
			cout << "ERROR: hkbSenseHandleModifier Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
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
			cout << "ERROR: hkbSenseHandleModifier Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbSenseHandleModifier(newID: " << id << ") with hkbSenseHandleModifier(oldID: " << tempid << ") is complete!" << endl;
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
			cout << "ERROR: hkbSenseHandleModifier Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbSenseHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbsensehandlemodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbSenseHandleModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload = line.substr(29, line.find("</hkparam>") - 29);
				if (payload != "null")
				{
					if (!exchangeID[payload].empty())
					{
						payload = exchangeID[payload];
					}
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbSenseHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbSenseHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbsensehandlemodifier::GetPayload()
{
	return "#" + boost::regex_replace(string(payload), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbsensehandlemodifier::IsPayloadNull()
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

string hkbsensehandlemodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbsensehandlemodifier::IsBindingNull()
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

string hkbsensehandlemodifier::GetAddress()
{
	return address;
}

bool hkbsensehandlemodifier::IsNegate()
{
	return IsNegated;
}

void hkbSenseHandleModifierExport(string originalfile, string editedfile, string id)
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
		cout << "ERROR: Edit hkbSenseHandleModifier Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
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
		cout << "ERROR: Edit hkbSenseHandleModifier Output Not Found (Edited File: " << editedfile << ")" << endl;
		Error = true;
	}

	//stage 2 identifying edits
	vector<string> output;
	bool IsEdited = false;
	bool IsChanged = false;
	bool open = false;
	int curline = 0;
	int part = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if ((storeline1[curline].find("<hkparam name=\"ranges\" numelements=", 0) == string::npos) && (part == 0)) // pre ranges info
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

			if (storeline1[curline].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos)
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
		else if ((storeline1[curline].find("<hkparam name=\"handleOut\">", 0) == string::npos) && (part == 1))
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkparam name=\"handleOut\">", 0) == string::npos))
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
				if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

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

			if ((open) && (storeline2[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos))
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

			if (storeline2[i].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
			{
				output.push_back("				</hkobject>");
			}
		}
		else if (part == 2) // existing leftover settings
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
			if (storeline2[i].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{

					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

					if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("handleOut", 0) == string::npos))
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
			else if (storeline2[i].find("<hkparam name=\"handleOut\">", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
				output.push_back(storeline2[i]);
				part = 2;
				curline++;
			}
			else
			{
				if (part == 1)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if (storeline2[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos)
					{
						if (IsChanged)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < curline; j++)
							{
								if ((storeline1[j].find("<hkparam name=\"id\">", 0) != string::npos) && (output.back().find("ORIGINAL", 0) == string::npos))
								{
									output.push_back("				<hkobject>");
								}

								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}

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

	NemesisReaderFormat(output, true);

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
					if ((output[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
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
					if (output[i].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos)
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

					if (output[i + 1].find("<hkparam name=\"id\">", 0) != string::npos)
					{
						outputfile << output[i] << "\n";

						if (output[i].find("OPEN", 0) != string::npos)
						{

						}

						// outputfile << "				<hkobject>" << "\n";
						// outputfile << output[i] << "\n";
					}
					else
					{
						outputfile << output[i] << "\n";
					}

					if (output[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos)
					{
						part = 1;
					}
				}
				else
				{
					outputfile << output[i] << "\n";
				}

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("handleOut", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))
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
			cout << "ERROR: Edit hkbSenseHandleModifier Output Not Found (New Edited File: " << editedfile << ")" << endl;
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