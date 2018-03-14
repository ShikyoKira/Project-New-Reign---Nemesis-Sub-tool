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
		string dummyID = CrossReferencing(id, address, functionlayer, compare);
		if (compare)
		{
			IsNegated = true;
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

	vector<string> storeline;
	string line;
	payloadcount = 0;
	payload.reserve(2);

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (int i = 0; i < size; i++)
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
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));
				if (payload[payloadcount] != "null")
				{
					referencingIDs[payload[payloadcount]].push_back(id);
				}
				payloadcount++;
			}

			storeline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	ofstream output("temp/" + id + ".txt");
	if (output.is_open())
	{
		for (unsigned int i = 0; i < storeline.size(); i++)
		{
			output << storeline[i] << "\n";
		}
		output.close();
	}
	else
	{
		cout << "ERROR: BSCyclicBlendTransitionGenerator Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

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
	payloadcount = 0;
	payload.reserve(2);

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (int i = 0; i < size; i++)
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
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));
				if (payload[payloadcount] != "null")
				{
					if (!exchangeID[payload[payloadcount]].empty())
					{
						int tempint = line.find(payload[payloadcount]);
						payload[payloadcount] = exchangeID[payload[payloadcount]];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload[payloadcount]);
					}
					referencingIDs[payload[payloadcount]].push_back(id);
				}
				payloadcount++;
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
		if (!addressChange[address].empty())
		{
			tempid = addressID[addressChange[address]];
			addressChange.erase(addressChange.find(address));
		}
		else
		{
			tempid = addressID[address];
		}
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSCyclicBlendTransitionGenerator(newID: " << id << ") with BSCyclicBlendTransitionGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);

		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
			}
		}

		string inputfile = "temp/" + tempid + ".txt";
		vector<string> storeline;
		storeline.reserve(FileLineCount(inputfile));
		ifstream input(inputfile); // read old function
		if (input.is_open())
		{
			while (getline(input, line))
			{
				storeline.push_back(line);
			}
			input.close();
		}
		else
		{
			cout << "ERROR: BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		// stage 3
		bool IsNewChild = false;
		vector<string> newstoreline;
		vector<string> newchild;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
				newstoreline.push_back(newline[i]);
		}

		ofstream output("new/" + tempid + ".txt"); // output stored function data
		if (output.is_open())
		{
			for (unsigned int i = 0; i < newstoreline.size(); i++)
			{
				output << newstoreline[i] << "\n";
			}
			output.close();
		}
		else
		{
			cout << "ERROR: BSCyclicBlendTransitionGenerator Outputfile(File: " << filepath << ", newID: " << id << ", oldID: " << tempid << ")" << endl;
			Error = true;
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSCyclicBlendTransitionGenerator(newID: " << id << ") with BSCyclicBlendTransitionGenerator(oldID: " << tempid << ") is complete!" << endl;
		}
	}

	else
	{
		IsForeign[id] = true;

		ofstream output("new/" + id + ".txt"); // output stored function data
		if (output.is_open())
		{
			for (unsigned int i = 0; i < newline.size(); i++)
			{
				output << newline[i] << "\n";
			}
			output.close();
		}
		else
		{
			cout << "ERROR: BSCyclicBlendTransitionGenerator Outputfile(File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

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

	string line;
	string filepath = "new/" + id + ".txt";
	ifstream file(filepath);
	payloadcount = 0;
	payload.reserve(2);

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);
				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						variablebindingset = exchangeID[variablebindingset];
					}
				}
			}
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);
				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));
				if (payload[payloadcount] != "null")
				{
					if (!exchangeID[payload[payloadcount]].empty())
					{
						payload[payloadcount] = exchangeID[payload[payloadcount]];
					}
				}
				payloadcount++;
			}
		}
		file.close();
	}
	else
	{
		cout << "ERROR: Dummy BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
	return payloadcount;
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

void BSCyclicBlendTransitionGeneratorExport(string originalfile, string editedfile, string id)
{
	//stage 1 reading
	vector<string> storeline1;
	string line;
	ifstream origfile(originalfile);

	if (origfile.is_open())
	{
		while (getline(origfile, line))
		{
			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
		origfile.close();
	}
	else
	{
		cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Input Not Found (Original File: " << originalfile << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vector<string> storeline2;
	ifstream editfile(editedfile);

	if (editfile.is_open())
	{
		while (getline(editfile, line))
		{
			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
		editfile.close();
	}
	else
	{
		cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Output Not Found (Edited File: " << editedfile << ")" << endl;
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
	string filename = "cache/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	if (IsEdited)
	{
		ofstream outputfile(filename);
		if (outputfile.is_open())
		{
			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("<hkparam name=\"EventToFreezeBlendValue\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
						outputfile << "				<hkobject>" << "\n";
					}
					else
					{
						outputfile << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
					}
					else
					{
						outputfile << "				<hkobject>" << "\n";
						outputfile << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
						outputfile << "				</hkobject>" << "\n";
						outputfile << "			</hkparam>" << "\n";
					}
					else
					{
						outputfile << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"EventToCrossBlend\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
					}
					else
					{
						outputfile << "				</hkobject>" << "\n";
						outputfile << "			</hkparam>" << "\n";
						outputfile << output[i] << "\n";
					}

					if (output[i + 1].find("OPEN", 0) != string::npos)
					{
						outputfile << "				<hkobject>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"fBlendParameter\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						outputfile << output[i] << "\n";
					}
					else
					{
						outputfile << "				</hkobject>" << "\n";
						outputfile << "			</hkparam>" << "\n";
						outputfile << output[i] << "\n";
					}
				}
				else
				{
					outputfile << output[i] << "\n";
				}
			}
			outputfile << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Output Not Found (New Edited File: " << editedfile << ")" << endl;
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