#include "hkbkeyframebonesmodifier.h"
#include "Global.h"

using namespace std;

hkbkeyframebonesmodifier::hkbkeyframebonesmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "bp" + to_string(functionlayer) + ">";

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

void hkbkeyframebonesmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbKeyframeBonesModifier(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	string classname = "<hkobject name=\"" + id;
	bool record = false;

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
			else if (line.find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				keyframedbonelist = line.substr(38, line.find("</hkparam>") - 38);

				if (keyframedbonelist != "null")
				{
					referencingIDs[keyframedbonelist].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbKeyframeBonesModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbKeyframeBonesModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbkeyframebonesmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbKeyframeBonesModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				keyframedbonelist = line.substr(38, line.find("</hkparam>") - 38);

				if (keyframedbonelist != "null")
				{
					if (!exchangeID[keyframedbonelist].empty())
					{
						int tempint = line.find(keyframedbonelist);
						keyframedbonelist = exchangeID[keyframedbonelist];
						line.replace(tempint, line.find("</hkparam>") - tempint, keyframedbonelist);
					}

					parent[keyframedbonelist] = id;
					referencingIDs[keyframedbonelist].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbKeyframeBonesModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbKeyframeBonesModifier(newID: " << id << ") with hkbKeyframeBonesModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (keyframedbonelist != "null")
		{
			referencingIDs[keyframedbonelist].pop_back();
			referencingIDs[keyframedbonelist].push_back(tempid);
			parent[keyframedbonelist] = tempid;
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
			cout << "Comparing hkbKeyframeBonesModifier(newID: " << id << ") with hkbKeyframeBonesModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbKeyframeBonesModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbkeyframebonesmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbKeyframeBonesModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
			{
				keyframedbonelist = line.substr(38, line.find("</hkparam>") - 38);

				if (keyframedbonelist != "null")
				{
					if (!exchangeID[keyframedbonelist].empty())
					{
						keyframedbonelist = exchangeID[keyframedbonelist];
					}

					parent[keyframedbonelist] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbKeyframeBonesModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbKeyframeBonesModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbkeyframebonesmodifier::GetKeyframedBonesList()
{
	return "#" + boost::regex_replace(string(keyframedbonelist), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbkeyframebonesmodifier::IsKeyframedBonesListNull()
{
	if (keyframedbonelist.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbkeyframebonesmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbkeyframebonesmodifier::IsBindingNull()
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

string hkbkeyframebonesmodifier::GetAddress()
{
	return address;
}

bool hkbkeyframebonesmodifier::IsNegate()
{
	return IsNegated;
}

void hkbKeyframeBonesModifierExport(string id)
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

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbKeyframeBonesModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
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
		cout << "ERROR: Edit hkbKeyframeBonesModifier Output Not Found (ID: " << id << ")" << endl;
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
		if ((storeline1[curline].find("<hkparam name=\"keyframeInfo\" numelements=", 0) == string::npos) && (part == 0)) // pre keyframeInfo info
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

			if (storeline1[curline].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos)
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
		else if ((storeline1[curline].find("<hkparam name=\"keyframedBonesList\">", 0) == string::npos) && (part == 1))
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) == string::npos))
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
				if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
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

			if ((open) && (storeline2[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos))
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

			if (storeline2[i].find("<hkparam name=\"isValid\">", 0) != string::npos)
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
			if (storeline2[i].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

					if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"isValid\">", 0) != string::npos)
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
			else if (storeline2[i].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
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

					if (storeline2[i].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"isValid\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if (storeline2[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
					{
						if (IsChanged)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < curline; j++)
							{
								if ((storeline1[j].find("<hkparam name=\"keyframedPosition\">", 0) != string::npos) && (output.back().find("ORIGINAL", 0) == string::npos))
								{
									output.push_back("				<hkobject>");
								}

								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"isValid\">", 0) != string::npos)
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

	NemesisReaderFormat(output);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	bool closeOri = false;
	bool closeEdit = false;

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);
			part = 0;

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (i < output.size() - 1)
				{
					if ((output[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
				}

				if (part == 0)
				{
					if (output[i].find("<hkparam name=\"keyframeInfo\" numelements=", 0) != string::npos)
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

					if (output[i + 1].find("<hkparam name=\"keyframedBonesList\">", 0) != string::npos)
					{
						part = 1;
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("keyframedBonesList", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))
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
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbKeyframeBonesModifier Output Not Found (File: " << filename << ")" << endl;
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