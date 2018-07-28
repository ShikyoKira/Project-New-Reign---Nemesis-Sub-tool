#include "hkbpoweredragdollcontrolmodifier.h"
#include "Global.h"

using namespace std;

hkbpoweredragdollcontrolmodifier::hkbpoweredragdollcontrolmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "bc" + to_string(functionlayer) + ">";

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

void hkbpoweredragdollcontrolmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbPoweredRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"bones\">", 0) != string::npos)
			{
				bone = line.substr(25, line.find("</hkparam>") - 25);

				if (bone != "null")
				{
					referencingIDs[bone].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweight = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweight != "null")
				{
					referencingIDs[boneweight].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbPoweredRagdollControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbPoweredRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbpoweredragdollcontrolmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbPoweredRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"bones\">", 0) != string::npos)
			{
				bone = line.substr(25, line.find("</hkparam>") - 25);

				if (bone != "null")
				{
					if (!exchangeID[bone].empty())
					{
						int tempint = line.find(bone);
						bone = exchangeID[bone];
						line.replace(tempint, line.find("</hkparam>") - tempint, bone);
					}

					parent[bone] = id;
					referencingIDs[bone].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweight = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweight != "null")
				{
					if (!exchangeID[boneweight].empty())
					{
						int tempint = line.find(boneweight);
						boneweight = exchangeID[boneweight];
						line.replace(tempint, line.find("</hkparam>") - tempint, boneweight);
					}

					parent[boneweight] = id;
					referencingIDs[boneweight].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbPoweredRagdollControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbPoweredRagdollControlsModifier(newID: " << id << ") with hkbPoweredRagdollControlsModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		referencingIDs[bone].push_back(tempid);
		parent[bone] = tempid;

		referencingIDs[boneweight].push_back(tempid);
		parent[boneweight] = tempid;
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
			cout << "Comparing hkbPoweredRagdollControlsModifier(newID: " << id << ") with hkbPoweredRagdollControlsModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbPoweredRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbpoweredragdollcontrolmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbPoweredRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"bones\">", 0) != string::npos)
			{
				bone = line.substr(25, line.find("</hkparam>") - 25);

				if (bone != "null")
				{
					if (!exchangeID[bone].empty())
					{
						bone = exchangeID[bone];
					}

					parent[bone] = id;
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweight = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweight != "null")
				{
					if (!exchangeID[boneweight].empty())
					{
						boneweight = exchangeID[boneweight];
					}

					parent[boneweight] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbPoweredRagdollControlsModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbPoweredRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbpoweredragdollcontrolmodifier::GetBone()
{
	return "#" + boost::regex_replace(string(bone), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbpoweredragdollcontrolmodifier::IsBoneNull()
{
	if (bone.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbpoweredragdollcontrolmodifier::GetBoneWeight()
{
	return "#" + boost::regex_replace(string(boneweight), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbpoweredragdollcontrolmodifier::IsBoneWeightNull()
{
	if (boneweight.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbpoweredragdollcontrolmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbpoweredragdollcontrolmodifier::IsBindingNull()
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

string hkbpoweredragdollcontrolmodifier::GetAddress()
{
	return address;
}

bool hkbpoweredragdollcontrolmodifier::IsNegate()
{
	return IsNegated;
}

void hkbPoweredRagdollControlsModifierExport(string id)
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
		cout << "ERROR: Edit hkbPoweredRagdollControlsModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbPoweredRagdollControlsModifier Output Not Found (File: " << filename << ")" << endl;
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