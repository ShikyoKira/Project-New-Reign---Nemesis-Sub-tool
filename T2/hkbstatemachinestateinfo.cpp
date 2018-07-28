#include "hkbstatemachinestateinfo.h"
#include "Global.h"

using namespace std;

hkbstatemachinestateinfo::hkbstatemachinestateinfo(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "p" + to_string(functionlayer) + ">";

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

void hkbstatemachinestateinfo::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineStateInfo(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("enterNotifyEvents", 0) != string::npos)
			{
				enternotifyevent = line.substr(37, line.find("</hkparam>") - 37);

				if (enternotifyevent != "null")
				{
					referencingIDs[enternotifyevent].push_back(id);
				}
			}
			else if (line.find("exitNotifyEvents", 0) != string::npos)
			{
				exitnotifyevent = line.substr(36, line.find("</hkparam>") - 36);

				if (exitnotifyevent != "null")
				{
					referencingIDs[exitnotifyevent].push_back(id);
				}
			}
			else if (line.find("transitions", 0) != string::npos)
			{
				transitions = line.substr(31, line.find("</hkparam>") - 31);

				if (transitions != "null")
				{
					referencingIDs[transitions].push_back(id);
				}
			}
			else if (line.find("generator", 0) != string::npos)
			{
				generator = line.substr(29, line.find("</hkparam>") - 29);
				referencingIDs[generator].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineStateInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineStateInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinestateinfo::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineStateInfo(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("enterNotifyEvents", 0) != string::npos)
			{
				enternotifyevent = line.substr(37, line.find("</hkparam>") - 37);

				if (enternotifyevent != "null")
				{
					if (!exchangeID[enternotifyevent].empty())
					{
						int tempint = line.find(enternotifyevent);
						enternotifyevent = exchangeID[enternotifyevent];
						line.replace(tempint, line.find("</hkparam>") - tempint, enternotifyevent);
					}

					parent[enternotifyevent] = id;
					referencingIDs[enternotifyevent].push_back(id);
				}
			}
			else if (line.find("exitNotifyEvents", 0) != string::npos)
			{
				exitnotifyevent = line.substr(36, line.find("</hkparam>") - 36);

				if (exitnotifyevent != "null")
				{
					if (!exchangeID[exitnotifyevent].empty())
					{
						int tempint = line.find(exitnotifyevent);
						exitnotifyevent = exchangeID[exitnotifyevent];
						line.replace(tempint, line.find("</hkparam>") - tempint, exitnotifyevent);
					}

					parent[exitnotifyevent] = id;
					referencingIDs[exitnotifyevent].push_back(id);
				}
			}
			else if (line.find("transitions", 0) != string::npos)
			{
				transitions = line.substr(31, line.find("</hkparam>") - 31);

				if (transitions != "null")
				{
					if (!exchangeID[transitions].empty())
					{
						int tempint = line.find(transitions);
						transitions = exchangeID[transitions];
						line.replace(tempint, line.find("</hkparam>") - tempint, transitions);
					}

					parent[transitions] = id;
					referencingIDs[transitions].push_back(id);
				}
			}
			else if (line.find("generator", 0) != string::npos)
			{
				generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					generator = exchangeID[generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, generator);
				}

				parent[generator] = id;
				referencingIDs[generator].push_back(id);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineStateInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbStateMachineStateInfo(newID: " << id << ") with hkbStateMachineStateInfo(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (enternotifyevent != "null")
		{
			referencingIDs[enternotifyevent].pop_back();
			referencingIDs[enternotifyevent].push_back(tempid);
			parent[enternotifyevent] = tempid;
		}

		if (exitnotifyevent != "null")
		{
			referencingIDs[exitnotifyevent].pop_back();
			referencingIDs[exitnotifyevent].push_back(tempid);
			parent[exitnotifyevent] = tempid;
		}

		if (transitions != "null")
		{
			referencingIDs[transitions].pop_back();
			referencingIDs[transitions].push_back(tempid);
			parent[transitions] = tempid;
		}
		
		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);
		parent[generator] = tempid;

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
			cout << "Comparing hkbStateMachineStateInfo(newID: " << id << ") with hkbStateMachineStateInfo(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbStateMachineStateInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinestateinfo::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachineStateInfo(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("enterNotifyEvents", 0) != string::npos)
			{
				enternotifyevent = line.substr(37, line.find("</hkparam>") - 37);

				if (enternotifyevent != "null")
				{
					if (!exchangeID[enternotifyevent].empty())
					{
						enternotifyevent = exchangeID[enternotifyevent];
					}

					parent[enternotifyevent] = id;
				}
			}
			else if (line.find("exitNotifyEvents", 0) != string::npos)
			{
				exitnotifyevent = line.substr(36, line.find("</hkparam>") - 36);

				if (exitnotifyevent != "null")
				{
					if (!exchangeID[exitnotifyevent].empty())
					{
						exitnotifyevent = exchangeID[exitnotifyevent];
					}

					parent[exitnotifyevent] = id;
				}
			}
			else if (line.find("transitions", 0) != string::npos)
			{
				transitions = line.substr(31, line.find("</hkparam>") - 31);

				if (transitions != "null")
				{
					if (!exchangeID[transitions].empty())
					{
						transitions = exchangeID[transitions];
					}

					parent[transitions] = id;
				}
			}
			else if (line.find("generator", 0) != string::npos)
			{
				generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}

				parent[generator] = id;
				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachineStateInfo Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachineStateInfo(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachinestateinfo::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbstatemachinestateinfo::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachinestateinfo::IsBindingNull()
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

string hkbstatemachinestateinfo::GetTransitions()
{
	return transitions;
}

bool hkbstatemachinestateinfo::IsTransitionsNull()
{
	if (transitions.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbstatemachinestateinfo::GetEnterNotifyEvent()
{
	return enternotifyevent;
}

bool hkbstatemachinestateinfo::IsEnterNotifyEventNull()
{
	if (enternotifyevent.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbstatemachinestateinfo::GetExitNotifyEvent()
{
	return exitnotifyevent;
}

bool hkbstatemachinestateinfo::IsExitNotifyEventNull()
{
	if (exitnotifyevent.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbstatemachinestateinfo::GetAddress()
{
	return address;
}

bool hkbstatemachinestateinfo::IsNegate()
{
	return IsNegated;
}

void hkbStateMachineStateInfoExport(string id)
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

			if (line[line.size() - 1] == '\n')
			{
				line.pop_back();
			}

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
		cout << "ERROR: Edit hkbStateMachineStateInfo Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(storeline2);

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
			cout << "ERROR: Edit hkbStateMachineStateInfo Output Not Found (File: " << filename << ")" << endl;
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