#include "hkbbehaviorgraphstringdata.h"
#include "Global.h"

using namespace std;

hkbbehaviorgraphstringdata::hkbbehaviorgraphstringdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "b" + to_string(functionlayer) + ">";

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

void hkbbehaviorgraphstringdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphStringData(ID: " << id << ") has been initialized!" << endl;
	}

	vector<string> storeline;

	if (!FunctionLineOriginal[id].empty())
	{
		ofstream output("temp/" + id + ".txt");

		if (output.is_open())
		{
			for (unsigned int i = 0; i < FunctionLineOriginal[id].size(); i++)
			{
				output << FunctionLineOriginal[id][i] << "\n";
			}

			output.close();
		}
		else
		{
			cout << "ERROR: hkbBehaviorGraphStringData Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraphStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphstringdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphStringData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (int i = 0; i < size; i++)
		{
			newline.push_back(FunctionLineEdited[id][i]);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
	{
		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraphStringData(newID: " << id << ") with hkbBehaviorGraphStringData(oldID: " << tempid << ")" << endl;
		}

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
			cout << "ERROR: hkbBehaviorGraphStringData Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		// stage 3
		int curline = 1;
		vector<string> newstoreline;

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
			cout << "ERROR: hkbBehaviorGraphStringData Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraphStringData(newID: " << id << ") with hkbBehaviorGraphStringData(oldID: " << tempid << ") is complete!" << endl;
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
			output.close();
		}
		else
		{
			cout << "ERROR: hkbBehaviorGraphStringData Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraphStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphstringdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBehaviorGraphStringData(ID: " << id << ") has been initialized!" << endl;
	}

	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);

	if (file.is_open())
	{
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy hkbBehaviorGraphStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBehaviorGraphStringData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbbehaviorgraphstringdata::GetAddress()
{
	return address;
}

bool hkbbehaviorgraphstringdata::IsNegate()
{
	return IsNegated;
}

void hkbBehaviorGraphStringDataExport(string originalfile, string editedfile, string id)
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
	int curline = 2;
	int part = 0;
	int openpoint;
	int closepoint;

	output.push_back(storeline2[0]);
	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[1]);
		if ((storeline1[1].find("</hkparam>", 0) != string::npos) && (storeline2[2].find("<hkcstring>", 0) != string::npos))
		{
			IsChanged = true;
			openpoint = 1;
			open = true;
		}
		else
		{
			IsChanged = false;
			output.push_back("<!-- ORIGINAL -->");
			output.push_back(storeline1[1]);
			output.push_back("<!-- CLOSE -->");
			open = false;
		}
		IsEdited = true;
	}
	else
	{
		output.push_back(storeline2[1]);
	}

	for (unsigned int i = 2; i < storeline2.size(); i++)
	{
		if ((storeline1[curline].find("<hkparam name=\"attributeNames\" numelements=", 0) == string::npos) && (part == 0)) // existing attributeNames value
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back(storeline2[i]);
				if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkcstring>", 0) != string::npos))
				{
					IsChanged = true;
					openpoint = curline;
					open = true;
				}
				else
				{
					IsChanged = false;
					output.push_back("<!-- ORIGINAL -->");
					output.push_back(storeline1[curline]);
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
				IsEdited = true;
			}
			else
			{
				output.push_back(storeline2[i]);
			}
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"variableNames\" numelements=", 0) == string::npos) && (part == 1)) // existing variableNames value
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back(storeline2[i]);
				if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkcstring>", 0) != string::npos))
				{
					IsChanged = true;
					openpoint = curline;
					open = true;
				}
				else
				{
					IsChanged = false;
					output.push_back("<!-- ORIGINAL -->");
					output.push_back(storeline1[curline]);
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
				IsEdited = true;
			}
			else
			{
				output.push_back(storeline2[i]);
			}
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) == string::npos) && (part == 2)) // existing characterPropertyNames value
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
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
			else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					for (int j = openpoint; j < curline; j++)
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
		}
		else if ((curline != storeline1.size()) && (part == 3)) // existing characterPropertyNames value
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
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
			else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					for (int j = openpoint; j < curline; j++)
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

			if (curline + 1 != storeline1.size())
			{
				curline++;
			}
		}
		else // added variable value
		{
			if (storeline2[i].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkcstring>", 0) != string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						IsChanged = false;
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
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
			else if (storeline2[i].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkcstring>", 0) != string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						IsChanged = false;
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
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
			else if (storeline2[i].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkcstring>", 0) != string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						IsChanged = false;
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						open = false;
					}
					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
				part = 3;

				if (curline + 1 != storeline1.size())
				{
					curline++;
				}
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

					if (storeline2[i + 1].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos)
					{
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

					if (storeline2[i + 1].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos)
					{
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

					if (storeline2[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos)
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
			closepoint = curline + 1;
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
			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (i < output.size() - 1)
				{
					if ((output[i + 1].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"eventNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((i + 1 == output.size()) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							outputfile << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							outputfile << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
				}

				if (output[i].find("\" numelements=\"", 0) != string::npos)
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

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("numelements=", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkcstring>", 0) != string::npos))
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
				else if (i + 1 == output.size())
				{
					if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkcstring>", 0) != string::npos))
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