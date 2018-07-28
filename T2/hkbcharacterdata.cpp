#include "hkbcharacterdata.h"
#include "Global.h"

using namespace std;

hkbcharacterdata::hkbcharacterdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "ce" + to_string(functionlayer) + ">";

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

void hkbcharacterdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		for (unsigned int i = 0; i < FunctionLineOriginal[id].size(); i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				size_t pos = line.find("characterPropertyValues\">") + 25;
				charPropertyID = line.substr(pos, line.find("</hkparam>") - pos);

				if (charPropertyID != "null")
				{
					referencingIDs[charPropertyID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"footIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("footIkDriverInfo\">") + 18;
				footIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (footIKID != "null")
				{
					referencingIDs[footIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"handIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("handIkDriverInfo\">") + 18;
				handIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (handIKID != "null")
				{
					referencingIDs[handIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				size_t pos = line.find("stringData\">") + 12;
				stringData = line.substr(pos, line.find("</hkparam>") - pos);

				if (stringData != "null")
				{
					referencingIDs[stringData].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"mirroredSkeletonInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("mirroredSkeletonInfo\">") + 22;
				skeleton = line.substr(pos, line.find("</hkparam>") - pos);

				if (skeleton != "null")
				{
					referencingIDs[skeleton].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbCharacterData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbCharacterData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> storeline2;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		for (unsigned int i = 0; i < FunctionLineEdited[id].size(); i++)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				usize pos = line.find("characterPropertyValues\">") + 25;
				charPropertyID = line.substr(pos, line.find("</hkparam>") - pos);

				if (charPropertyID != "null")
				{
					if (!exchangeID[charPropertyID].empty())
					{
						usize tempint = line.find(charPropertyID);
						charPropertyID = exchangeID[charPropertyID];
						line.replace(tempint, line.find("</hkparam>") - tempint, charPropertyID);
					}

					parent[charPropertyID] = id;
					referencingIDs[charPropertyID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"footIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("footIkDriverInfo\">") + 18;
				footIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (footIKID != "null")
				{
					if (!exchangeID[footIKID].empty())
					{
						usize tempint = line.find(footIKID);
						footIKID = exchangeID[footIKID];
						line.replace(tempint, line.find("</hkparam>") - tempint, footIKID);
					}

					parent[footIKID] = id;
					referencingIDs[footIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"handIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("handIkDriverInfo\">") + 18;
				handIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (handIKID != "null")
				{
					if (!exchangeID[handIKID].empty())
					{
						usize tempint = line.find(handIKID);
						handIKID = exchangeID[handIKID];
						line.replace(tempint, line.find("</hkparam>") - tempint, handIKID);
					}

					parent[handIKID] = id;
					referencingIDs[handIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				size_t pos = line.find("stringData\">") + 12;
				stringData = line.substr(pos, line.find("</hkparam>") - pos);

				if (stringData != "null")
				{
					if (!exchangeID[stringData].empty())
					{
						usize tempint = line.find(stringData);
						stringData = exchangeID[stringData];
						line.replace(tempint, line.find("</hkparam>") - tempint, stringData);
					}

					parent[stringData] = id;
					referencingIDs[stringData].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"mirroredSkeletonInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("mirroredSkeletonInfo\">") + 22;
				skeleton = line.substr(pos, line.find("</hkparam>") - pos);

				if (skeleton != "null")
				{
					if (!exchangeID[skeleton].empty())
					{
						usize tempint = line.find(skeleton);
						skeleton = exchangeID[skeleton];
						line.replace(tempint, line.find("</hkparam>") - tempint, skeleton);
					}

					parent[skeleton] = id;
					referencingIDs[skeleton].push_back(id);
				}
			}

			storeline2.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbCharacterData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			address = addressChange[address];
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbCharacterData(newID: " << id << ") with hkbCharacterData(oldID: " << tempid << ")" << endl;
		}

		if (charPropertyID != "null")
		{
			referencingIDs[charPropertyID].pop_back();
			referencingIDs[charPropertyID].push_back(tempid);
			parent[charPropertyID] = tempid;
		}

		if (footIKID != "null")
		{
			referencingIDs[footIKID].pop_back();
			referencingIDs[footIKID].push_back(tempid);
			parent[footIKID] = tempid;
		}

		if (handIKID != "null")
		{
			referencingIDs[handIKID].pop_back();
			referencingIDs[handIKID].push_back(tempid);
			parent[handIKID] = tempid;
		}

		if (stringData != "null")
		{
			referencingIDs[stringData].pop_back();
			referencingIDs[stringData].push_back(tempid);
			parent[stringData] = tempid;
		}

		if (skeleton != "null")
		{
			referencingIDs[skeleton].pop_back();
			referencingIDs[skeleton].push_back(tempid);
			parent[skeleton] = tempid;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < storeline2.size(); i++)
		{
			FunctionLineNew[tempid].push_back(storeline2[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbCharacterData(newID: " << id << ") with hkbCharacterData(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = storeline2;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbCharacterData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbCharacterData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				size_t pos = line.find("characterPropertyValues\">") + 25;
				charPropertyID = line.substr(pos, line.find("</hkparam>") - pos);

				if (charPropertyID != "null")
				{
					if (!exchangeID[charPropertyID].empty())
					{
						charPropertyID = exchangeID[charPropertyID];
					}

					parent[charPropertyID] = id;
				}
			}
			else if (line.find("<hkparam name=\"footIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("footIkDriverInfo\">") + 18;
				footIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (footIKID != "null")
				{
					if (!exchangeID[footIKID].empty())
					{
						footIKID = exchangeID[footIKID];
					}

					parent[footIKID] = id;
				}
			}
			else if (line.find("<hkparam name=\"handIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("handIkDriverInfo\">") + 18;
				handIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (handIKID != "null")
				{
					if (!exchangeID[handIKID].empty())
					{
						handIKID = exchangeID[handIKID];
					}

					parent[handIKID] = id;
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				size_t pos = line.find("stringData\">") + 12;
				stringData = line.substr(pos, line.find("</hkparam>") - pos);

				if (stringData != "null")
				{
					if (!exchangeID[stringData].empty())
					{
						stringData = exchangeID[stringData];
					}

					parent[stringData] = id;
				}
			}
			else if (line.find("<hkparam name=\"mirroredSkeletonInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("mirroredSkeletonInfo\">") + 22;
				skeleton = line.substr(pos, line.find("</hkparam>") - pos);

				if (skeleton != "null")
				{
					if (!exchangeID[skeleton].empty())
					{
						skeleton = exchangeID[skeleton];
					}

					parent[skeleton] = id;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbCharacterData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbCharacterData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbcharacterdata::GetAddress()
{
	return address;
}

bool hkbcharacterdata::IsNegate()
{
	return IsNegated;
}

string hkbcharacterdata::GetCharPropertyValues()
{
	return charPropertyID;
}

string hkbcharacterdata::GetFootIK()
{
	return footIKID;
}

string hkbcharacterdata::GetHandIK()
{
	return handIKID;
}

string hkbcharacterdata::GetSkeletonInfo()
{
	return skeleton;
}

string hkbcharacterdata::GetStringData()
{
	return stringData;
}

bool hkbcharacterdata::HasCharProperty()
{
	if (charPropertyID.find("null", 0) != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool hkbcharacterdata::HasFootIK()
{
	if (footIKID.find("null", 0) != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool hkbcharacterdata::HasHandIK()
{
	if (handIKID.find("null", 0) != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void hkbCharacterDataExport(string id)
{
	//stage 1 reading
	vector<string> storeline1 = FunctionLineTemp[id];

	vector<string> storeline2 = FunctionLineNew[id];

	// stage 2 identify edits
	vector<string> output;
	bool open = false;
	bool IsEdited = false;
	int curline = 2;
	int openpoint;
	int closepoint;
	int nextpoint;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		openpoint = curline - 1;
		IsEdited = true;
		open = true;
	}

	output.push_back(storeline2[1]);

	for (unsigned int i = 2; i < storeline2.size(); i++)		// wordVariableValues data
	{
		if (storeline1[curline].find("\t\t\t<hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkparam>", 0) != string::npos)
		{
			if (open)
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

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			output.push_back(storeline1[curline]);
			curline++;
		}
		else if (storeline1[curline].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) == string::npos)		// existing variable value
		{
			if (storeline1[curline].find(storeline2[i], 0) != string::npos)
			{
				if (open)
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

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}
			}

			curline++;
			output.push_back(storeline2[i]);
		}
		else // added variable value
		{
			if (open)
			{
				closepoint = curline - 1;
				output.pop_back();

				if (closepoint != openpoint)
				{
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			nextpoint = i;
			break;
		}
	}

	bool skip = false;

	for (unsigned int i = nextpoint; i < storeline2.size(); i++)		// characterPropertyInfos data
	{
		if (storeline1[curline].find("<hkparam name=\"numBonesPerLod\" numelements=", 0) == string::npos)		// existing variable value
		{
			if (storeline1[curline + 1].find("<hkparam name=\"numBonesPerLod\" numelements=", 0) == string::npos && (storeline1[curline].find("\t\t\t<hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkparam>", 0) != string::npos || (storeline1[curline].find("\t\t\t<hkparam name=\"role\">", 0) != string::npos && storeline1[curline].find("\t\t\t<hkparam name=\"role\">ROLE_DEFAULT</hkparam>", 0) == string::npos)))
			{
				if (open)
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

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back(storeline1[curline]);
				curline++;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					if (unsigned int(nextpoint) + 5 > i && storeline1[curline].find("</hkparam>", 0) != string::npos)
					{
						skip = true;
					}
				}
				else
				{
					if (open)
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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}

				curline++;
				output.push_back(storeline2[i]);
			}
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"numBonesPerLod\" numelements=", 0) != string::npos)
			{
				if (open)
				{
					if (skip)
					{
						closepoint = curline;
					}
					else
					{
						closepoint = curline - 1;
						output.pop_back();
					}

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					output.push_back("<!-- CLOSE -->");

					if (!skip)
					{
						output.push_back("			</hkparam>");
					}

					open = false;
				}

				nextpoint = i;
				break;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						openpoint = curline;
						open = true;
					}
				}
				else
				{
					if (open)
					{
						closepoint = curline - 1;
						output.pop_back();

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}

			output.push_back(storeline2[i]);
		}
	}

	for (unsigned int i = nextpoint; i < storeline2.size(); i++)		// numBonesPerLod data
	{
		if (storeline1[curline].find("<hkparam name=\"characterPropertyValues\">", 0) == string::npos)		// existing variable value
		{
			if (storeline1[curline + 1].find("<hkparam name=\"characterPropertyValues\">", 0) == string::npos && (storeline1[curline].find("\t\t\t<hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkparam>", 0) != string::npos || (storeline1[curline].find("\t\t\t<hkparam name=\"role\">", 0) != string::npos && storeline1[curline].find("\t\t\t<hkparam name=\"role\">ROLE_DEFAULT</hkparam>", 0) == string::npos)))
			{
				if (open)
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

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back(storeline1[curline]);
				curline++;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					if (unsigned int(nextpoint) + 5 > i && storeline1[curline].find("</hkparam>", 0) != string::npos)
					{
						skip = true;
					}
				}
				else
				{
					if (open)
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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}

				curline++;
				output.push_back(storeline2[i]);
			}
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				if (open)
				{
					if (skip)
					{
						closepoint = curline;
					}
					else
					{
						closepoint = curline - 1;
						output.pop_back();
					}

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					output.push_back("<!-- CLOSE -->");

					if (!skip)
					{
						output.push_back("			</hkparam>");
					}

					open = false;
				}

				nextpoint = i;
				break;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						openpoint = curline;
						open = true;
					}
				}
				else
				{
					if (open)
					{
						closepoint = curline - 1;
						output.pop_back();

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}

			output.push_back(storeline2[i]);
		}
	}

	for (unsigned int i = nextpoint; i < storeline2.size(); i++)		 // leftover data
	{
		if (storeline1[curline].find(storeline2[i], 0) != string::npos)
		{
			if (open)
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

				output.push_back("<!-- CLOSE -->");
				open = false;
			}
		}
		else
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = curline;
				IsEdited = true;
				open = true;
			}
		}

		curline++;
		output.push_back(storeline2[i]);
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
				fwrite << output[i] << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbCharacterData Output Not Found (File: " << filename << ")" << endl;
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
