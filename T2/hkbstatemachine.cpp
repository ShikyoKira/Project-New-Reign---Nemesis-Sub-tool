#include "hkbstatemachine.h"
#include "Global.h"
#include "generatorlines.h"
#include "src\stateid.h"

using namespace std;

string GetClass(string id, bool compare); // get class
bool matchScoring(vector<string>& ori, vector<string>& edit, string id);

hkbstatemachine::hkbstatemachine(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "r" + to_string(functionlayer) + ">region";

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
		bool statusChanged = false;

		if (IsForeign[id])
		{
			statusChanged = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare);

		if (compare)
		{
			Dummy(dummyID);

			if (IsForeign[id])
			{
				if (!region[id].empty())
				{
					address = region[id];
				}
				else
				{
					address = preaddress;
				}
			}
			else if (stateChange && statusChanged)
			{
				string temp = preaddress + "r" + to_string(functionlayer) + ">region";
				string lastSMAdd = boost::regex_replace(string(temp), boost::regex("(^.*[(]r[0-9]+[)]=>[^>]+>)[^>]+.*"), string("\\1"));

				if (lastSMAdd != temp)
				{
					string parentID = parent[addressID[lastSMAdd]];

					if (parentID.length() == 0)
					{
						cout << "ERROR: hkbStateMachine missing parent ID (ID: " << id << ")" << endl;
						Error = true;
						return;
					}

					if (isSMIDExist[parentID])
					{
						previousSM = StateMachineID[parentID];
						previousSMExist = true;
					}
				}
				else
				{
					string parentID = parent[id];

					while (parentID.length() > 0 && GetClass(parentID, true) != "hkbStateMachine")
					{
						if (IsForeign[parentID])
						{
							cout << "ERROR: hkbStateMachine foreign parent ID (ID: " << parentID << ")" << endl;
							Error = true;
							return;
						}

						parentID = parent[parentID];
					}

					if (isSMIDExist[parentID])
					{
						previousSM = StateMachineID[parentID];
						previousSMExist = true;
					}
				}

				StateMachineID[dummyID] = make_shared<hkbstatemachine>(*this);
				isSMIDExist[dummyID] = true;
			}
		}
		else
		{
			IsNegated = true;
			address = region[id];
		}
	}
	else
	{
		return;
	}
}

void hkbstatemachine::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachine (ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;
	bool pauseline = false;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);
						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
						referencingIDs[tempgenerator].push_back(id);
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

					if (variablebindingset != "null")
					{
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);

					if (payload != "null")
					{
						parent[payload] = id;
						referencingIDs[payload].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;

					if (children > 0)
					{
						isStateIDExist[id] = true;
					}
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						parent[wildcard] = id;
						referencingIDs[wildcard].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"name\">", 0) != string::npos)
				{
					name = line.substr(24, line.find("</hkparam>") - 24);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID
	address = name + "(r" + to_string(regioncount[name]) + ")=>";
	regioncount[name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachine::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineEdited[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					int curgen = 1;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = 0;
						usize tempint = 0;

						for (int j = 0; j < curgen; j++)
						{
							position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
						}

						curgen++;
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							usize tempint = line.find(tempgenerator);
							int templength = tempgenerator.length();
							tempgenerator = exchangeID[tempgenerator];
							line.replace(tempint, templength, tempgenerator);
						}

						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
						referencingIDs[tempgenerator].push_back(id);
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

					if (variablebindingset != "null")
					{
						if (!exchangeID[variablebindingset].empty())
						{
							int tempint = line.find(variablebindingset);
							line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[variablebindingset]);
							variablebindingset = exchangeID[variablebindingset];
						}

						parent[variablebindingset] = id;
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

						parent[payload] = id;
						referencingIDs[payload].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						if (!exchangeID[wildcard].empty())
						{
							usize tempint = line.find(wildcard);
							line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[wildcard]);
							wildcard = exchangeID[wildcard];
						}

						parent[wildcard] = id;
						referencingIDs[wildcard].push_back(id);
					}
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		elements[tempid] = children;
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachine(newID: " << id << ") with hkbStateMachine(oldID: " << tempid << ")" << endl;
		}
		
		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < generator.size(); i++)
		{
			referencingIDs[generator[i]].pop_back();
			referencingIDs[generator[i]].push_back(tempid);
			parent[generator[i]] = tempid;
		}

		if (payload != "null")
		{
			referencingIDs[payload].pop_back();
			referencingIDs[payload].push_back(tempid);
			parent[payload] = tempid;
		}

		if (wildcard != "null")
		{
			referencingIDs[wildcard].pop_back();
			referencingIDs[wildcard].push_back(tempid);
			parent[wildcard] = tempid;
		}

		vector<string> storeline = FunctionLineTemp[tempid];

		// stage 3
		int curline = 1;
		bool IsNewChild = false;
		vector<string> newstoreline;
		vector<string> newchild;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
			if (!IsNewChild)
			{
				if ((newline[i].find("numelements", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
				}

				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].find("wildcardTransitions", 0) == string::npos))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].find("wildcardTransitions", 0) == string::npos))
					{
						for (unsigned int j = 0; j < newchild.size(); j++)
						{
							newstoreline.push_back(newchild[j]);
						}

						newstoreline.push_back(newline[i]);
						curline++;
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}
				}
				else if ((storeline[curline].find(newline[i], 0) == string::npos) && (storeline[curline].find("</hkparam>", 0) == string::npos) && (storeline[curline].find("</hkobject>", 0) == string::npos))
				{
					usize size = count(storeline[curline].begin(), storeline[curline].end(), '#');
					usize size2 = count(newline[i].begin(), newline[i].end(), '#');

					if (size < size2)
					{
						usize position = 0;
						usize tempint = 0;

						for (unsigned int j = 0; j < size + 1; j++)
						{
							position = newline[i].find("#", tempint);
							tempint = newline[i].find("#", position + 1);
						}

						newstoreline.push_back(newline[i].substr(0, position - 1));
						newchild.push_back("				" + newline[i].substr(position, -1));
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}

					curline++;
				}
				else
				{
					newstoreline.push_back(newline[i]);
					curline++;
				}
			}
		}

		FunctionLineNew[tempid] = newstoreline;

		if (stateChange)
		{
			string lastSMAdd = boost::regex_replace(string(address), boost::regex("(^.*[(]r[0-9]+[)]=>[^>]+>)[^>]+.*"), string("\\1"));

			if (lastSMAdd != address)
			{
				string parentID = parent[addressID[lastSMAdd]];

				if (parentID.length() == 0)
				{
					cout << "ERROR: hkbStateMachine missing parent ID (ID: " << id << ")" << endl;
					Error = true;
					return;
				}

				if (isSMIDExist[parentID])
				{
					previousSM = StateMachineID[parentID];
					previousSMExist = true;
				}
			}

			StateMachineID[tempid] = make_shared<hkbstatemachine>(*this);
			isSMIDExist[tempid] = true;
		}

		address = region[tempid];

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachine(newID: " << id << ") with hkbStateMachine(oldID: " << tempid << ") is complete!" << endl;
		}

	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
		elements[id] = children;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachine::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
	}

	bool pauseline = false;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');
					generator.reserve(size);

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							tempgenerator = exchangeID[tempgenerator];
						}

						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
					}
				}
			}
			else
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

						parent[variablebindingset] = id;
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

						parent[payload] = id;
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						if (!exchangeID[wildcard].empty())
						{
							wildcard = exchangeID[wildcard];
						}

						parent[wildcard] = id;
					}

					break;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachine Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachine::NextGenerator(int child)
{
	return "#" + boost::regex_replace(string(generator[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachine::GetChildren()
{
	return children;
}

string hkbstatemachine::GetPayload()
{
	return "#" + boost::regex_replace(string(payload), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachine::IsPayloadNull()
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

string hkbstatemachine::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachine::IsBindingNull()
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

string hkbstatemachine::GetWildcard()
{
	return wildcard;
}

bool hkbstatemachine::IsWildcardNull()
{
	if (wildcard.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbstatemachine::GetAddress()
{
	return address;
}

bool hkbstatemachine::IsNegate()
{
	return IsNegated;
}

string hkbstatemachine::GetRealID()
{
	return realID;
}

void hkbStateMachineExport(string id)
{
	//stage 1 reading
	vector<string> storeline1;
	vector<string> storeline2;

	if (!generatorLines(storeline1, storeline2, id, "hkbStateMachine"))
	{
		return;
	}

	vector<string> oriline;
	vector<string> output;
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	size_t size = max(storeline1.size(), storeline2.size());

	for (unsigned int i = 0; i < size; ++i)
	{
		if (storeline1[curline].find("<hkparam name=\"wildcardTransitions\">", 0) == string::npos)
		{
			if (storeline1[curline] != storeline2[i])
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				oriline.push_back(storeline1[curline]);
				output.push_back(storeline2[i]);
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), oriline.begin(), oriline.end());
					output.push_back("<!-- CLOSE -->");
					oriline.clear();
					open = false;
				}

				output.push_back(storeline1[curline]);
			}

			++curline;
		}
		else if (storeline2[i].find("<hkparam name=\"wildcardTransitions\">", 0) == string::npos)
		{
			if (open && oriline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), oriline.begin(), oriline.end());
				output.push_back("<!-- CLOSE -->");
				oriline.clear();
				open = false;
			}

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
			if (open)
			{
				if (oriline.size() > 0)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), oriline.begin(), oriline.end());
					oriline.clear();
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			output.push_back("			</hkparam>");

			if (storeline1[curline] != storeline2[i])
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back(storeline2[i]);
				output.push_back("<!-- ORIGINAL -->");
				output.push_back(storeline1[curline]);
				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
			}
			else
			{
				output.push_back(storeline1[curline]);
			}
		}
	}

	NemesisReaderFormat(output, true);

	// stage 2 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("<hkparam name=\"eventToSendWhenStateOrTransitionChanges\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
						fwrite << "				<hkobject>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << "				<hkobject>" << "\n";
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"startStateChooser\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit hkbStateMachine Output Not Found (File: " << filename << ")" << endl;
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
