#include "hkbexpressioncondition.h"
#include "Global.h"

using namespace std;

hkbexpressioncondition::hkbexpressioncondition(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "l" + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id, functionlayer);
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

		if (addressChange.find(address) != addressChange.end())
		{
			string tempAdd = addressChange[address];
			addressChange.erase(addressChange.find(address));
			address = tempAdd;

			IsForeign[id] = false;
			exchangeID[id] = dummyID;

			if ((Debug) && (!Error))
			{
				cout << "Comparing hkbExpressionCondition (newID: " << id << ") with hkbExpressionCondition (oldID: " << dummyID << ")" << endl;
			}

			ReferenceReplacementExt(id, dummyID); // replacing reference in previous functions that is using newID

			{
				vector<string> emptyVS;
				FunctionLineNew[dummyID] = emptyVS;
			}

			FunctionLineNew[dummyID].push_back(FunctionLineTemp[dummyID][0]);

			for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
			{
				FunctionLineNew[dummyID].push_back(FunctionLineEdited[id][i]);
			}

			if ((Debug) && (!Error))
			{
				cout << "Comparing hkbExpressionCondition (newID: " << id << ") with hkbExpressionCondition (oldID: " << dummyID << ") is complete!" << endl;
			}

			RecordID(id, address, true);

			if ((Debug) && (!Error))
			{
				cout << "hkbExpressionCondition (ID: " << id << ") is complete!" << endl;
			}

			Dummy(dummyID);
		}
		else
		{
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
	else
	{
		return;
	}
}

void hkbexpressioncondition::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbExpressionCondition(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbExpressionCondition Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbExpressionCondition (ID: " << id << ") is complete!" << endl;
	}
}

void hkbexpressioncondition::Compare(string filepath, string id, int functionlayer)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbExpressionCondition (ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	string line;
	string tempid;
	bool pass = false;
	int backCounter = tempaddress.length() - 1;

	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbExpressionCondition Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	while (isdigit(tempaddress[backCounter]))
	{
		line = tempaddress[backCounter] + line;
		--backCounter;
	}

	line = line + "l" + to_string(functionlayer);

	// stage 2
	if (conditionOldFunction(id, address, functionlayer, line, false) != "") // is this new function or old for non generator
	{
		if (addressChange.find(address) != addressChange.end())
		{
			string tempAdd = addressChange[address];
			addressChange.erase(addressChange.find(address));
			tempid = addressID[tempAdd];
		}
		else
		{
			tempid = addressID[address];
		}

		pass = true;

		if (pass != pass)
		{
			string functionlayer = boost::regex_replace(string(address.substr(address.find_last_of("l"))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
			int num = elements[parent[id]];
			string preadd = tempaddress.substr(0, tempaddress.length() - boost::regex_replace(string(tempaddress.substr(tempaddress.find_last_of(">") + 1)), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")).length());
			string expression = FunctionLineEdited[id][1];

			map<string, string> debugAddress;

			for (auto& add : addressID)
			{
				debugAddress[add.first] = add.second;
			}

			for (int i = 0; i < 1000; ++i)
			{
				string tempadd = preadd + to_string(i) + "l" + functionlayer + ">";

				if (addressID[tempadd].empty())
				{
					num = max(num, i);
					break;
				}
			}

			if (!addressID[address].empty())
			{
				if (FunctionLineOriginal[addressID[address]][1] == expression)
				{
					pass = true;
				}
			}

			if (!pass)
			{
				for (int i = 0; i < num; ++i)
				{
					string tempadd = preadd + to_string(i) + "l" + functionlayer + ">";

					if (!addressID[tempadd].empty())
					{
						if (FunctionLineOriginal[addressID[tempadd]][1] == expression)
						{
							tempid = addressID[tempadd];

							if (IsExist[tempid])
							{
								string unchangedID;
								pass = true;

								for (auto& ID : exchangeID)
								{
									if (ID.second == tempid)
									{
										unchangedID = ID.first;
										break;
									}
								}

								if (unchangedID.length() == 0)
								{
									cout << "ERROR: hkbExpressionCondition missing wrong reference (Adjusted ID: " << tempid << ", Original ID: " << id << ")" << endl;
									Error = true;
									return;
								}

								if (tempid != unchangedID)
								{
									int wrongReferenceInt = stoi(tempid.substr(1, tempid.length() - 1));
									int rightReferenceInt = stoi(unchangedID.substr(1, unchangedID.length() - 1));

									if (!IsExist[unchangedID])
									{
										FunctionLineNew[unchangedID] = FunctionLineEdited[unchangedID];
										IsExist[unchangedID] = true;
										idcount.insert(rightReferenceInt);
									}

									FunctionLineNew[tempid].clear();
									IsExist.erase(IsExist.find(tempid));
									idcount.erase(wrongReferenceInt);
								}

								ReferenceReplacementExt(tempid, unchangedID);
							}

							break;
						}
					}
				}
			}
		}

		if (!IsExist[tempid])
		{
			// pass = true;
		}
	}

	if (pass)
	{
		exchangeID[id] = tempid;
		IsForeign[id] = false;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbExpressionCondition (newID: " << id << ") with hkbExpressionCondition (oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

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
			cout << "Comparing hkbExpressionCondition (newID: " << id << ") with hkbExpressionCondition (oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbExpressionCondition (ID: " << id << ") is complete!" << endl;
	}
}

void hkbexpressioncondition::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbExpressionCondition (ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbExpressionCondition Inputfile (ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbExpressionCondition (ID: " << id << ") is complete!" << endl;
	}
}

string hkbexpressioncondition::GetAddress()
{
	return address;
}

bool hkbexpressioncondition::IsNegate()
{
	return IsNegated;
}

void hkbExpressionConditionExport(string id)
{
	// stage 1 reading
	vector<string> storeline1 = FunctionLineTemp[id];

	vector<string> storeline2 = FunctionLineNew[id];

	// stage 2 identify edits
	vector<string> output;
	bool IsEdited = false;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[1]);
		output.push_back("<!-- ORIGINAL -->");
		output.push_back(storeline1[1]);
		output.push_back("<!-- CLOSE -->");
		IsEdited = true;
	}

	output.push_back(storeline2[2]);

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
			cout << "ERROR: Edit hkbExpressionCondition Output Not Found (File: " << filename << ")" << endl;
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