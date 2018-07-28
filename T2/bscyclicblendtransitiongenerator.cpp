#include "bscyclicblendtransitiongenerator.h"
#include "Global.h"

using namespace std;

bscyclicblendtransitiongenerator::bscyclicblendtransitiongenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "c" + to_string(functionlayer) + ">";

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

void bscyclicblendtransitiongenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSCyclicBlendTransitionGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	payload.reserve(2);

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
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (generator != "null")
				{
					referencingIDs[generator].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSCyclicBlendTransitionGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bscyclicblendtransitiongenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSCyclicBlendTransitionGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;
	payload.reserve(2);

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
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[generator]);
					generator = exchangeID[generator];
				}

				if (generator != "null")
				{
					parent[generator] = id;
					referencingIDs[generator].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						int tempint = line.find(payload.back());
						payload.back() = exchangeID[payload.back()];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload.back());
					}

					parent[payload.back()] = id;
					referencingIDs[payload.back()].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSCyclicBlendTransitionGenerator(newID: " << id << ") with BSCyclicBlendTransitionGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
		}

		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
				parent[payload[i]] = tempid;
			}
		}

		// stage 3
		bool IsNewChild = false;
		vector<string> newstoreline;
		vector<string> newchild;

		newstoreline.push_back(FunctionLineTemp[tempid][0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
				newstoreline.push_back(newline[i]);
		}

		FunctionLineNew[tempid] = newstoreline;

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSCyclicBlendTransitionGenerator(newID: " << id << ") with BSCyclicBlendTransitionGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSCyclicBlendTransitionGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bscyclicblendtransitiongenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSCyclicBlendTransitionGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	payload.reserve(2);
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
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						payload.back() = exchangeID[payload.back()];
					}

					parent[payload.back()] = id;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSCyclicBlendTransitionGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSCyclicBlendTransitionGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bscyclicblendtransitiongenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int bscyclicblendtransitiongenerator::GetPayloadCount()
{
	return int(payload.size());
}

string bscyclicblendtransitiongenerator::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bscyclicblendtransitiongenerator::IsPayloadNull(int child)
{
	if (payload[child].find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bscyclicblendtransitiongenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bscyclicblendtransitiongenerator::IsBindingNull()
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

string bscyclicblendtransitiongenerator::GetAddress()
{
	return address;
}

bool bscyclicblendtransitiongenerator::IsNegate()
{
	return IsNegated;
}

void BSCyclicBlendTransitionGeneratorExport(string id)
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
		cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Input Not Found (ID: " << id << ")" << endl;
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
	}
	else
	{
		cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vector<string> output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	int openpoint;
	int closepoint;
	int curline = 0;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if ((storeline1[i].find(storeline2[i], 0) != string::npos) && (storeline1[i].length() == storeline2[i].length()))
		{
			if (open)
			{
				if (IsChanged)
				{
					closepoint = i;

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

	NemesisReaderFormat(output, true);

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
				if (output[i].find("<hkparam name=\"EventToFreezeBlendValue\">", 0) != string::npos)
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
				else if (output[i].find("<hkparam name=\"EventToCrossBlend\">", 0) != string::npos)
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

					if (output[i + 1].find("OPEN", 0) != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"fBlendParameter\">", 0) != string::npos)
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
			cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Output Not Found (File: " << filename << ")" << endl;
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