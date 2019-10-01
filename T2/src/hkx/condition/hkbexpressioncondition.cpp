#include "hkbexpressioncondition.h"

using namespace std;

namespace expressioncondition
{
	string key = "l";
	string classname = "hkbExpressionCondition";
	string signature = "0x1c3c1045";
}

hkbexpressioncondition::hkbexpressioncondition(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + expressioncondition::key + to_string(functionlayer) + ">";

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
			addressChange.erase(address);
			address = tempAdd;

			IsForeign[id] = false;
			exchangeID[id] = dummyID;

			if ((Debug) && (!Error))
			{
				cout << "Comparing hkbExpressionCondition (newID: " << id << ") with hkbExpressionCondition (oldID: " << dummyID << ")" << endl;
			}

			ReferenceReplacementExt(id, dummyID); // replacing reference in previous functions that is using newID

			{
				vecstr emptyVS;
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
			addressChange.erase(address);
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
			vecstr emptyVS;
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

safeStringUMap<shared_ptr<hkbexpressioncondition>> hkbexpressionconditionList;
safeStringUMap<shared_ptr<hkbexpressioncondition>> hkbexpressionconditionList_E;

void hkbexpressioncondition::regis(string id, bool isEdited)
{
	isEdited ? hkbexpressionconditionList_E[id] = shared_from_this() : hkbexpressionconditionList[id] = shared_from_this();
	isEdited ? hkbconditionList_E[id] = shared_from_this() : hkbconditionList[id] = shared_from_this();
	ID = id;
}

void hkbexpressioncondition::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	for (auto& line : nodelines)
	{
		if (readParam("expression", line, expression)) break;
	}

	if ((Debug) && (!Error))
	{
		cout << expressioncondition::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbexpressioncondition::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + expressioncondition::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			string line;
			int backCounter = preaddress.length() - 1;

			while (isdigit(preaddress[backCounter]))
			{
				line = preaddress[backCounter] + line;
				--backCounter;
			}

			line = line + "l" + to_string(functionlayer);

			// existed
			if (conditionOldFunction(shared_from_this(), address, functionlayer, line, false) != "") // is this new function or old for non generator
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbexpressionconditionList_E.erase(ID);
				hkbconditionList_E.erase(ID);
				editedBehavior.erase(ID);
				previousID = ID;
				ID = addressID[address];
				hkbexpressionconditionList_E[ID] = protect;
				hkbconditionList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, true);
			}
		}
	}
	else if (!compare)
	{
		CrossReferencing(shared_from_this(), address, functionlayer, compare);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
	{
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare);

		// comparing
		if (compare)
		{
			if (ID != newID)
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto foreign_itr = IsForeign.find(ID);

				if (foreign_itr != IsForeign.end()) IsForeign.erase(foreign_itr);

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbexpressionconditionList_E.erase(ID);
				hkbconditionList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				previousID = ID;
				ID = addressID[address];
				hkbexpressionconditionList_E[ID] = protect;
				hkbconditionList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
			}
			else
			{
				address = preaddress;
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
	{
		if (count(address.begin(), address.end(), '>') == 3)
		{
			if (address.find("(cj", 0) != string::npos || address.find("(i", 0) != string::npos)
			{
				IsOldFunction(filepath, shared_from_this(), address);
			}
		}
	}
}

string hkbexpressioncondition::getClassCode()
{
	return expressioncondition::key;
}

void hkbexpressioncondition::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(3);
	output.reserve(3);
	usize base = 2;
	hkbexpressioncondition* ctrpart = static_cast<hkbexpressioncondition*>(counterpart.get());

	output.push_back(openObject(base, ID, expressioncondition::classname, expressioncondition::signature));		// 1
	paramMatch("expression", expression, ctrpart->expression, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", expressioncondition::classname, output, isEdited);
}

void hkbexpressioncondition::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(3);

	output.push_back(openObject(base, ID, expressioncondition::classname, expressioncondition::signature));		// 1
	output.push_back(autoParam(base, "expression", expression));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, expressioncondition::classname, output, true);
}

void hkbExpressionConditionExport(string id)
{
	// stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	vecstr storeline2 = FunctionLineNew[id];

	// stage 2 identify edits
	vecstr output;
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