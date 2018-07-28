#include "bsoffsetanimationgenerator.h"
#include "Global.h"

using namespace std;

bsoffsetanimationgenerator::bsoffsetanimationgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "ci" + to_string(functionlayer) + ">";

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

		string dummyID = CrossReferencing(id, address, functionlayer, compare, true);

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

void bsoffsetanimationgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSOffsetAnimationGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"pOffsetClipGenerator\">", 0) != string::npos)
			{
				clipgenerator = line.substr(40, line.find("</hkparam>") - 40);
				referencingIDs[clipgenerator].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: BSOffsetAnimationGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSOffsetAnimationGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsoffsetanimationgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSOffsetAnimationGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					generator = exchangeID[generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, generator);
				}

				if (generator != "null")
				{
					parent[generator] = id;
					referencingIDs[generator].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"pOffsetClipGenerator\">", 0) != string::npos)
			{
				clipgenerator = line.substr(40, line.find("</hkparam>") - 40);

				if (!exchangeID[clipgenerator].empty())
				{
					int tempint = line.find(clipgenerator);
					clipgenerator = exchangeID[clipgenerator];
					line.replace(tempint, line.find("</hkparam>") - tempint, clipgenerator);
				}

				if (clipgenerator != "null")
				{
					parent[clipgenerator] = id;
					referencingIDs[clipgenerator].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSOffsetAnimationGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
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
			cout << "Comparing BSOffsetAnimationGenerator(newID: " << id << ") with BSOffsetAnimationGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (clipgenerator != "null")
		{
			referencingIDs[clipgenerator].pop_back();
			referencingIDs[clipgenerator].push_back(tempid);
			parent[clipgenerator] = tempid;
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

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
			cout << "Comparing BSOffsetAnimationGenerator(newID: " << id << ") with BSOffsetAnimationGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSOffsetAnimationGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsoffsetanimationgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSOffsetAnimationGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}

				if (generator != "null")
				{
					parent[generator] = id;
				}
			}
			else if (line.find("<hkparam name=\"pOffsetClipGenerator\">", 0) != string::npos)
			{
				clipgenerator = line.substr(40, line.find("</hkparam>") - 40);

				if (!exchangeID[clipgenerator].empty())
				{
					clipgenerator = exchangeID[clipgenerator];
				}

				if (clipgenerator != "null")
				{
					parent[clipgenerator] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSOffsetAnimationGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSOffsetAnimationGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bsoffsetanimationgenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string bsoffsetanimationgenerator::GetClipGenerator()
{
	return "#" + boost::regex_replace(string(clipgenerator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsoffsetanimationgenerator::IsClipNull()
{
	if (clipgenerator.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bsoffsetanimationgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsoffsetanimationgenerator::IsBindingNull()
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

string bsoffsetanimationgenerator::GetAddress()
{
	return address;
}

bool bsoffsetanimationgenerator::IsNegate()
{
	return IsNegated;
}

void BSOffsetAnimationGeneratorExport(string id)
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
		cout << "ERROR: Edit BSOffsetAnimationGenerator Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit BSOffsetAnimationGenerator Output Not Found (File: " << filename << ")" << endl;
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