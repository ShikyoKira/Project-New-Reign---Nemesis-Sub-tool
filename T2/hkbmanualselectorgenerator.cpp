#include "hkbmanualselectorgenerator.h"
#include "Global.h"

using namespace std;

hkbmanualselectorgenerator::hkbmanualselectorgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "m" + to_string(functionlayer) + ">region";

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
			Dummy(dummyID);

			if (IsForeign[id])
			{
				if (!region[id].empty())
				{
					address = region[id];
				}
				else
				{
					address = preaddress;
				}
			}
		}
		else
		{
			IsNegated = true;
			address = region[id];
		}
	}
}

void hkbmanualselectorgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;
	bool pauseline = false;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (pauseline)
			{
				if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);
						generator.push_back(tempgenerator);

						if (!IsReferenceExist[tempgenerator])
						{
							referencingIDs[tempgenerator].push_back(id);
							IsReferenceExist[tempgenerator] = true;
						}

						parent[tempgenerator] = id;
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;
				}
				else if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

					if (variablebindingset != "null")
					{
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"name\">", 0) != string::npos)
				{
					name = line.substr(24, line.find("</hkparam>") - 24);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	address = name + "(m" + to_string(regioncount[name]) + ")=>";
	regioncount[name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	unordered_map<string, bool> IsReferenceExist;
	vector<string> newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; i++)
		{
			line = FunctionLineEdited[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					int curgen = 1;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = 0;
						usize tempint = 0;

						for (int j = 0; j < curgen; j++)
						{
							position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
						}

						curgen++;
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							int tempint = line.find(tempgenerator);
							int templength = tempgenerator.length();
							tempgenerator = exchangeID[tempgenerator];
							line.replace(tempint, templength, tempgenerator);
						}

						parent[tempgenerator] = id;
						generator.push_back(tempgenerator);

						if (!IsReferenceExist[tempgenerator])
						{
							referencingIDs[tempgenerator].push_back(id);
							IsReferenceExist[tempgenerator] = true;
						}
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
				else if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
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
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		elements[tempid] = children;
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbManualSelectorGenerator(newID: " << id << ") with hkbManualSelectorGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < generator.size(); i++)
		{
			referencingIDs[generator[i]].pop_back();
			referencingIDs[generator[i]].push_back(tempid);
			parent[generator[i]] = tempid;
		}

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
					int numelement1 = stoi(boost::regex_replace(string(newline[i]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));
					int numelement2 = stoi(boost::regex_replace(string(storeline[curline]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));

					if (numelement1 != numelement2)
					{
						IsNewChild = true;
					}
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
			cout << "Comparing hkbManualSelectorGenerator(newID: " << id << ") with hkbManualSelectorGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	bool pauseline = false;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					break;
				}
				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							tempgenerator = exchangeID[tempgenerator];
						}

						parent[tempgenerator] = id;
						generator.push_back(tempgenerator);
					}
				}
			}

			else
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

						parent[variablebindingset] = id;
					}
				}

				else if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbManualSelectorGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string hkbmanualselectorgenerator::NextGenerator(int child)
{
	return "#" + boost::regex_replace(string(generator[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbmanualselectorgenerator::GetChildren()
{
	return children;
}

string hkbmanualselectorgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbmanualselectorgenerator::IsBindingNull()
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

string hkbmanualselectorgenerator::GetClass(string id, string inputfile)
{
	string line;
	char charline[2000];
	string classname = "<hkobject name=\"" + id;
	FILE* input;
	fopen_s(&input, inputfile.c_str(), "r");

	if (input)
	{
		while (fgets(charline, 2000, input))
		{
			if (charline[strlen(charline) - 1] == '\n')
			{
				charline[strlen(charline) - 1] = '\0';
			}

			line = charline;

			if (line.find(classname, 0) != string::npos)
			{
				int tempint = line.find("class") + 7;
				return line.substr(tempint, line.find("signature") - tempint - 2);
			}
		}

		fclose(input);
	}
	else
	{
		cout << "ERROR: GetClass Inputfile(" << id << ", " << inputfile << ")" << endl;
	}

	cout << "ERROR: GetClass(" << id << ", " << inputfile << ")" << endl;
	Error = true;

	return "null";
}

string hkbmanualselectorgenerator::GetAddress()
{
	return address;
}

bool hkbmanualselectorgenerator::IsNegate()
{
	return IsNegated;
}

void hkbManualSelectorGeneratorExport(string id)
{
	//stage 1 reading
	vector<string> storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		bool start = false;

		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if (start)
			{
				if (line.find("</hkparam>") != string::npos)
				{
					start = false;
				}
				else
				{
					stringstream sstream(line);
					istream_iterator<string> ssbegin(sstream);
					istream_iterator<string> ssend;
					vector<string> curElements(ssbegin, ssend);
					copy(curElements.begin(), curElements.end(), curElements.begin());

					for (auto& element : curElements)
					{
						storeline1.push_back("				" + element);
					}
				}
			}
			else if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}

			if (line.find("numelements=\"") != string::npos)
			{
				start = true;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbManualSelectorGenerator Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vector<string> storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		bool start = false;

		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (start)
			{
				if (line.find("</hkparam>") != string::npos)
				{
					start = false;
				}
				else
				{
					stringstream sstream(line);
					istream_iterator<string> ssbegin(sstream);
					istream_iterator<string> ssend;
					vector<string> curElements(ssbegin, ssend);
					copy(curElements.begin(), curElements.end(), curElements.begin());

					for (auto& element : curElements)
					{
						storeline2.push_back("				" + element);
					}
				}
			}
			else if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}

			if (line.find("numelements=\"") != string::npos)
			{
				start = true;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbManualSelectorGenerator Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vector<string> output;
	vector<string> oriline;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	bool IsOpenOut = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos) // existing variable value
		{
			if (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (storeline1[curline].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos)
				{
					output.push_back("");
					oriline.push_back(storeline1[curline]);
					++curline;
				}
				
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), oriline.begin(), oriline.end());
				output.push_back("<!-- CLOSE -->");
				oriline.clear();
				--i;
				open = false;
			}
			else
			{
				if (storeline1[curline] == storeline2[i])
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), oriline.begin(), oriline.end());
						output.push_back("<!-- CLOSE -->");
						oriline.clear();
						open = false;
					}

					output.push_back(storeline1[curline]);
				}
				else
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
					oriline.push_back(storeline1[curline]);
				}

				curline++;
			}
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos)
			{
				output.push_back("			</hkparam>");

				if (storeline1[curline] == storeline2[i])
				{
					output.push_back(storeline1[curline]);
				}
				else
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					oriline.push_back(storeline1[curline]);
					IsEdited = true;
					open = true;
				}

				curline++;
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos)
				{
					output.push_back(storeline2[i]);
					++i;
				}

				output.push_back("<!-- CLOSE -->");
				oriline.clear();
				open = false;
				--i;
			}
		}
	}

	if (open) // close unclosed edits
	{
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), oriline.begin(), oriline.end());
		output.push_back("<!-- CLOSE -->");
		oriline.clear();
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
				fwrite << output[i] << "\n";
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit hkbManualSelectorGenerator Output Not Found (File: " << filename << ")" << endl;
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
