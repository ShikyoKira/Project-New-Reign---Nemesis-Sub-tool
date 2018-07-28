#include "hkrootlevelcontainer.h"
#include "Global.h"

using namespace std;

hkrootlevelcontainer::hkrootlevelcontainer(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "n" + to_string(functionlayer) + ">";

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

void hkrootlevelcontainer::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkRootLevelContainer(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	
	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("variant", 0) != string::npos)
			{
				generator = line.substr(29, line.find("</hkparam>") - 29);
				referencingIDs[generator].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: hkRootLevelContainer Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;		
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkRootLevelContainer(ID: " << id << ") is complete!" << endl;
	}
}

void hkrootlevelcontainer::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkRootLevelContainer(ID: " << id << ") has been initialized!" << endl;
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

			if (line.find("variant", 0) != string::npos)
			{
				generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[generator].empty())
				{
					usize tempint = line.find(generator);
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
		cout << "ERROR: hkRootLevelContainer Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	IsForeign[id] = false;

	string tempid = addressID[address];
	exchangeID[id] = tempid;

	if ((Debug) && (!Error))
	{
		cout << "Comparing hkRootLevelContainer(newID: " << id << ") with hkRootLevelContainer(oldID: " << tempid << ")" << endl;
	}

	if (generator != "null")
	{
		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);
		parent[generator] = tempid;
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
		cout << "Comparing hkRootLevelContainer(newID: " << id << ") with hkRootLevelContainer(oldID: " << tempid << ") is complete!" << endl;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkRootLevelContainer(ID: " << id << ") is complete!" << endl;
	}
}

void hkrootlevelcontainer::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkRootLevelContainer(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("variant", 0) != string::npos)
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
		cout << "ERROR: Dummy hkRootLevelContainer Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkRootLevelContainer(ID: " << id << ") is complete!" << endl;
	}
}

string hkrootlevelcontainer::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkrootlevelcontainer::GetAddress()
{
	return address;
}

bool hkrootlevelcontainer::IsNegate()
{
	return IsNegated;
}

void hkRootLevelContainerExport(string id)
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
		cout << "ERROR: Edit hkRootLevelContainer Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

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
			cout << "ERROR: Edit hkRootLevelContainer Output Not Found (File: " << filename << ")" << endl;
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