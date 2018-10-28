#include "bsmodifyoncemodifier.h"
#include "Global.h"

using namespace std;

bsmodifyoncemodifier::bsmodifyoncemodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "ay" + to_string(functionlayer) + ">";

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

void bsmodifyoncemodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSModifyOnceModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOnActivateModifier\">", 0) != string::npos)
			{
				modifier1 = line.substr(39, line.find("</hkparam>") - 39);

				if (modifier1 != "null")
				{
					referencingIDs[modifier1].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"pOnDeactivateModifier\">", 0) != string::npos)
			{
				modifier2 = line.substr(41, line.find("</hkparam>") - 41);

				if (modifier2 != "null")
				{
					referencingIDs[modifier2].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: BSModifyOnceModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSModifyOnceModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bsmodifyoncemodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSModifyOnceModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOnActivateModifier\">", 0) != string::npos)
			{
				modifier1 = line.substr(39, line.find("</hkparam>") - 39);

				if (modifier1 != "null")
				{
					if (!exchangeID[modifier1].empty())
					{
						int tempint = line.find(modifier1);
						modifier1 = exchangeID[modifier1];
						line.replace(tempint, line.find("</hkparam>") - tempint, modifier1);
					}

					parent[modifier1] = id;
					referencingIDs[modifier1].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"pOnDeactivateModifier\">", 0) != string::npos)
			{
				modifier2 = line.substr(41, line.find("</hkparam>") - 41);

				if (modifier2 != "null")
				{
					if (!exchangeID[modifier2].empty())
					{
						int tempint = line.find(modifier2);
						modifier2 = exchangeID[modifier2];
						line.replace(tempint, line.find("</hkparam>") - tempint, modifier2);
					}

					parent[modifier2] = id;
					referencingIDs[modifier2].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSModifyOnceModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSModifyOnceModifier(newID: " << id << ") with BSModifyOnceModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (modifier1 != "null")
		{
			referencingIDs[modifier1].pop_back();
			referencingIDs[modifier1].push_back(tempid);
			parent[modifier1] = tempid;
		}

		if (modifier2 != "null")
		{
			referencingIDs[modifier2].pop_back();
			referencingIDs[modifier2].push_back(tempid);
			parent[modifier2] = tempid;
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
			cout << "Comparing BSModifyOnceModifier(newID: " << id << ") with BSModifyOnceModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSModifyOnceModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bsmodifyoncemodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSModifyOnceModifier(ID: " << id << ") has been initialized!" << endl;
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
			}
			else if (line.find("<hkparam name=\"pOnActivateModifier\">", 0) != string::npos)
			{
				modifier1 = line.substr(39, line.find("</hkparam>") - 39);

				if (modifier1 != "null")
				{
					if (!exchangeID[modifier1].empty())
					{
						modifier1 = exchangeID[modifier1];
					}

					parent[modifier1] = id;
				}
			}
			else if (line.find("<hkparam name=\"pOnDeactivateModifier\">", 0) != string::npos)
			{
				modifier2 = line.substr(41, line.find("</hkparam>") - 41);

				if (modifier2 != "null")
				{
					if (!exchangeID[modifier2].empty())
					{
						modifier2 = exchangeID[modifier2];
					}

					parent[modifier2] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSModifyOnceModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSModifyOnceModifier(ID: " << id << ") is complete!" << endl;
	}
}

string bsmodifyoncemodifier::GetModifier(int number)
{
	if (number == 0)
	{
		return "#" + boost::regex_replace(string(modifier1), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
	}
	else
	{
		return "#" + boost::regex_replace(string(modifier2), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
	}
}

bool bsmodifyoncemodifier::IsModifierNull(int number)
{
	string tempmodifier;

	if (number == 0)
	{
		tempmodifier = modifier1;
	}
	else
	{
		tempmodifier = modifier2;
	}

	if (tempmodifier.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bsmodifyoncemodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsmodifyoncemodifier::IsBindingNull()
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

string bsmodifyoncemodifier::GetAddress()
{
	return address;
}

bool bsmodifyoncemodifier::IsNegate()
{
	return IsNegated;
}

void BSModifyOnceModifierExport(string id)
{
	//stage 1 reading
	vector<string> storeline1 = FunctionLineTemp[id];

	//stage 2 reading and identifying edits
	vector<string> storeline2;
	storeline2.reserve(FunctionLineNew[id].size());
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find(storeline1[curline], 0) != string::npos) && (line.length() == storeline1[curline].length()))
			{
				if (open)
				{
					closepoint = curline;

					if (closepoint != openpoint)
					{
						storeline2.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							storeline2.push_back(storeline1[j]);
						}
					}

					storeline2.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					storeline2.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					open = true;
				}

				IsEdited = true;
			}

			storeline2.push_back(line);
			curline++;
		}
	}
	else
	{
		cout << "ERROR: Edit BSModifyOnceModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(storeline2, true);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream output(filename);

		if (output.is_open())
		{
			FunctionWriter fwrite(&output);

			for (unsigned int i = 0; i < storeline2.size(); i++)
			{
				fwrite << storeline2[i] << "\n";
			}

			output.close();
		}
		else
		{
			cout << "ERROR: Edit BSModifyOnceModifier Output Not Found (File: " << filename << ")" << endl;
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