#include "hkbcharacterstringdata.h"
#include "Global.h"

using namespace std;

hkbcharacterstringdata::hkbcharacterstringdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "cc" + to_string(functionlayer) + ">";

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
}

void hkbcharacterstringdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterStringData(ID: " << id << ") has been initialized!" << endl;
	}

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbCharacterStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbCharacterStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterstringdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterStringData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		newline = FunctionLineEdited[id];
	}
	else
	{
		cout << "ERROR: hkbCharacterStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
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
			cout << "Comparing hkbCharacterStringData(newID: " << id << ") with hkbCharacterStringData(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

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
				if ((newline[i].find("<hkparam name=\"generators\" numelements=", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
				}

				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].length() < 15))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].length() < 15))
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
				else if (storeline[curline].find(newline[i], 0) == string::npos)
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

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbCharacterStringData(newID: " << id << ") with hkbCharacterStringData(oldID: " << tempid << ") is complete!" << endl;
		}

		address = region[tempid];
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
		cout << "hkbCharacterStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterstringdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbCharacterStringData(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbCharacterStringData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbCharacterStringData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbcharacterstringdata::GetAddress()
{
	return address;
}

bool hkbcharacterstringdata::IsNegate()
{
	return IsNegated;
}

void hkbCharacterStringDataExport(string id)
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
		cout << "ERROR: Edit hkbCharacterStringData Input Not Found (ID: " << id << ")" << endl;
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
		cout << "ERROR: Edit hkbCharacterStringData Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vector<string> output;
	bool IsEdited = false;
	bool IsChanged = false;
	bool IsOpenOut = false;
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
		if ((storeline1[curline].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) == string::npos) && (part == 0)) // existing deformableSkinNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"animationNames\" numelements=", 0) == string::npos) && (part == 1)) // existing rigidSkinNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"animationFilenames\" numelements=", 0) == string::npos) && (part == 2)) // existing animationNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) == string::npos) && (part == 3)) // existing animationFilenames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) == string::npos) && (part == 4)) // existing characterPropertyNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"lodNames\" numelements=", 0) == string::npos) && (part == 5)) // existing retargetingSkeletonMapperFilenames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) == string::npos) && (part == 6)) // existing lodNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) == string::npos) && (part == 7)) // existing mirroredSyncPointSubstringsA value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"name\">", 0) == string::npos) && (part == 8)) // existing mirroredSyncPointSubstringsB value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((curline != storeline1.size()) && (part == 9))
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
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
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}

			output.push_back(storeline2[i]);

			if (curline + 1 != storeline1.size())
			{
				curline++;
			}
		}
		else // added variable value
		{
			if (storeline2[i].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 1;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"animationNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 2;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"animationFilenames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 3;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 4;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 5;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"lodNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 6;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 7;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 8;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"name\">", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
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
				}
				else
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}
				}

				output.push_back(storeline2[i]);
				part = 9;
				curline++;
			}
			else
			{
				if (part == 0)
				{
					postProcess("rigidSkinNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 1)
				{
					postProcess("animationNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 2)
				{
					postProcess("animationFilenames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 3)
				{
					postProcess("characterPropertyNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 4)
				{
					postProcess("retargetingSkeletonMapperFilenames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 5)
				{
					postProcess("lodNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 6)
				{
					postProcess("mirroredSyncPointSubstringsA", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 7)
				{
					postProcess("mirroredSyncPointSubstringsB", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 8)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"name\">", 0) != string::npos)
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
			}
		}

		if (i == 1921)
		{
			open = open;
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
				if (i < output.size() - 1)
				{
					if ((output[i + 1].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"deformableSkinNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationFilenames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationFilenames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"lodNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"lodNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((i + 1 == output.size()) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
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

				fwrite << output[i] << "\n";

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("numelements=", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkcstring>", 0) != string::npos))
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
				else if (i + 1 == output.size())
				{
					if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkcstring>", 0) != string::npos))
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
			cout << "ERROR: Edit hkbCharacterStringData Output Not Found (File: " << filename << ")" << endl;
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

inline void process(vector<string> storeline1, vector<string> storeline2, int curline, int i, bool& IsChanged, int& openpoint, bool& open, bool& IsEdited, vector<string>& output)
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
}

inline void postProcess(string elementName, vector<string> storeline1, vector<string> storeline2, int curline, int i, bool& IsChanged, int openpoint, bool& open, bool& IsEdited, vector<string>& output)
{
	if (!open)
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		IsEdited = true;
		open = true;
	}

	output.push_back(storeline2[i]);

	if (storeline2[i + 1].find("<hkparam name=\"" + elementName + "\" numelements=", 0) != string::npos)
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