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
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare);

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

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
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
		}
	}
	else
	{
		cout << "ERROR: hkbHandIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
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

		for (usize i = 0; i < size; ++i)
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

					parent[variablebindingset] = id;
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
			cout << "Comparing hkbHandIkControlsModifier(newID: " << id << ") with hkbHandIkControlsModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		{
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbHandIkControlsModifier(newID: " << id << ") with hkbHandIkControlsModifier(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
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

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

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

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbHandIkControlsModifier Inputfile(ID: " << id << ")" << endl;
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

void hkbHandIkControlsModifierExport(string id)
{
	//stage 1 reading
	vector<string> storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10) && (line.find("<hkparam name=\"controlData\">", 0) == string::npos))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbHandIkControlsModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vector<string> storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10) && (line.find("<hkparam name=\"controlData\">", 0) == string::npos))
			{
				storeline2.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbHandIkControlsModifier Output Not Found (ID: " << id << ")" << endl;
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
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	bool closeOri = false;
	bool closeEdit = false;

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

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

				fwrite << output[i] << "\n";
				
				if (i < output.size() - 2)
				{
					if ((output[i + 1].find("ORIGINAL", 0) != string::npos) && (!closeEdit) && (output[i + 2].find("<hkparam name=\"hands\" numelements=", 0) != string::npos) && (output[i + 2].find("</hkparam>", 0) != string::npos))
					{
						fwrite << "			</hkparam>" << "\n";
						closeEdit = true;
					}
				}

				if (i == output.size() - 1)
				{
					if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))  // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbHandIkControlsModifier Output Not Found (File: " << filename << ")" << endl;
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