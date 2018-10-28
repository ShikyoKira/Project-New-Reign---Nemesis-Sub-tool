#include "hkbfootikdriverinfo.h"
#include "Global.h"

using namespace std;

hkbfootikdriverinfo::hkbfootikdriverinfo(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "cd" + to_string(functionlayer) + ">";

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

void hkbfootikdriverinfo::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkDriverInfo(ID: " << id << ") has been initialized!" << endl;
	}

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbFootIkDriverInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkDriverInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikdriverinfo::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkDriverInfo(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbFootIkDriverInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbFootIkDriverInfo(newID: " << id << ") with hkbFootIkDriverInfo(oldID: " << tempid << ")" << endl;
		}

		{
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
		{
			FunctionLineNew[tempid].push_back(FunctionLineEdited[id][i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbFootIkDriverInfo(newID: " << id << ") with hkbFootIkDriverInfo(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = FunctionLineEdited[id];
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkDriverInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikdriverinfo::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbFootIkDriverInfo(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbFootIkDriverInfo Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbFootIkDriverInfo(ID: " << id << ") is complete!" << endl;
	}
}

string hkbfootikdriverinfo::GetAddress()
{
	return address;
}

bool hkbfootikdriverinfo::IsNegate()
{
	return IsNegated;
}

void hkbFootIkDriverInfoExport(string id)
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
		cout << "ERROR: Edit hkbFootIkDriverInfo Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
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

		storeline2.shrink_to_fit();
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkDriverInfo Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vector<string> output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	bool IsEditedClose = false;
	bool IsEditedOpen = false;
	int openpoint;
	int closepoint;
	int curline = 0;
	int part = 0;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"raycastDistanceUp\">", 0) == string::npos && part == 0) // existing variable value
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
			{
				if (open)
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
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = i;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}

			curline++;
			output.push_back(storeline2[i]);

			if (curline != storeline1.size() && storeline1[curline].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos && storeline1[curline].find(storeline2[i + 1], 0) == string::npos)
			{
				output.push_back("				</hkobject>");
			}
		}
		else if (part == 1) // existing leftover settings
		{
			if (storeline2[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos)
			{
				output.push_back("				</hkobject>");
			}

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
			}

			curline++;
			output.push_back(storeline2[i]);			
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos) // existing variable value
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}
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
				}

				output.push_back(storeline2[i]);
				part = 1;
				curline++;
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

					if (storeline2[i].find("<hkparam name=\"kneeAxisLS\">", 0) != string::npos) // existing variable value
					{
						output.push_back("				<hkobject>");
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos)
					{
						output.push_back("				</hkobject>");

						if (IsChanged)
						{
							if (openpoint != curline)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
								{
									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"ankleIndex\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}
								}
							}

							IsChanged = false;
						}

						output.push_back("<!-- CLOSE -->");
						output.push_back("			</hkparam>");
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

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (((output[i].find("OPEN", 0) != string::npos) && (output[i].find("MOD_CODE", 0) != string::npos)))
				{
					if (output[i + 1].find("<hkparam name=\"kneeAxisLS\">", 0) != string::npos && output[i - 1].find("<hkobject6>", 0) == string::npos)
					{
						if (i > 10)
						{
							fwrite << "				</hkobject>" << "\n";
						}

						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i + 1].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos && output[i - 1].find("</hkparam>", 0) == string::npos)
					{
						fwrite << "			</hkparam>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"kneeAxisLS\">", 0) != string::npos)
				{
					if ((output[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos) || (output[i - 1].find("<!-- CLOSE -->", 0) != string::npos))
					{
						if (output[i - 2].find("<hkparam name=\"legs\" numelements=") == string::npos)
						{
							fwrite << "				</hkobject>" << "\n";
						}

						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i - 1].find("<hkparam name=\"legs\" numelements=") != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i - 1].find("</hkobject>") != string::npos && output[i - 2].find("<hkparam name=\"ankleIndex\">") != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
				}
				
				if (output[i].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos)
				{
					if ((output[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos))
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}
					else if ((output[i - 1].find("</hkobject>", 0) != string::npos))
					{
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<!-- CLOSE -->", 0) != string::npos)
				{
					if (output[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos && i + 1 != output.size() && output[i + 1].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos)
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
			cout << "ERROR: Edit hkbFootIkDriverInfo Output Not Found (File: " << filename << ")" << endl;
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