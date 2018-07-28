#include "hkbbehaviorgraphstringdata.h"
#include "Global.h"

using namespace std;

struct graphstringdata
{
	vector<string> eventNames;
	vector<string> attributeNames;
	vector<string> variableNames;
	vector<string> characterPropertyNames;
};

void inputGraphData(vector<string>& output, vector<string> orinamelist, vector<string> newnamelist, bool& IsEdited, string key);
void graphStringDataProcess(string line, graphstringdata& graphData, string& current);

hkbbehaviorgraphstringdata::hkbbehaviorgraphstringdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "b" + to_string(functionlayer) + ">";

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

void hkbbehaviorgraphstringdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphStringData(ID: " << id << ") has been initialized!" << endl;
	}
	
	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraphStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphstringdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphStringData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	string line;

	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbBehaviorGraphStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbBehaviorGraphStringData(newID: " << id << ") with hkbBehaviorGraphStringData(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbBehaviorGraphStringData(newID: " << id << ") with hkbBehaviorGraphStringData(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbBehaviorGraphStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphstringdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBehaviorGraphStringData(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbBehaviorGraphStringData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBehaviorGraphStringData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbbehaviorgraphstringdata::GetAddress()
{
	return address;
}

bool hkbbehaviorgraphstringdata::IsNegate()
{
	return IsNegated;
}

void hkbBehaviorGraphStringDataExport(string id)
{
	//stage 1 reading
	string line;
	graphstringdata oriData;

	if (FunctionLineTemp[id].size() > 0)
	{
		string current = "";

		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];
			graphStringDataProcess(line, oriData, current);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbBehaviorGraphStringData Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	graphstringdata newData;

	if (FunctionLineNew[id].size() > 0)
	{
		string current = "";

		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];
			graphStringDataProcess(line, newData, current);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbBehaviorGraphStringData Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vector<string> output;
	bool IsEdited = false;
	output.push_back(FunctionLineTemp[id][0]);
	inputGraphData(output, oriData.eventNames, newData.eventNames, IsEdited, "eventNames");
	inputGraphData(output, oriData.attributeNames, newData.attributeNames, IsEdited, "attributeNames");
	inputGraphData(output, oriData.variableNames, newData.variableNames, IsEdited, "variableNames");
	inputGraphData(output, oriData.characterPropertyNames, newData.characterPropertyNames, IsEdited, "characterPropertyNames");
	output.push_back("		</hkobject>");

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
			cout << "ERROR: Edit hkbBehaviorGraphStringData Output Not Found (File: " << filename << ")" << endl;
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

void graphStringDataProcess(string line, graphstringdata& graphData, string& current)
{
	if (current == "")
	{
		if (line.find("<hkparam name=\"eventNames\" numelements=\"") != string::npos)
		{
			current = "eventNames";
		}
	}
	else if (current == "eventNames")
	{
		if (line.find("<hkcstring>") != string::npos)
		{
			string name = boost::regex_replace(string(line), boost::regex("([\t]+)<hkcstring>([^<]+)</hkcstring>"), string("\\2"));
			graphData.eventNames.push_back(name);
		}
		else if (line.find("<hkparam name=\"attributeNames\" numelements=\"") != string::npos)
		{
			current = "attributeNames";
		}
	}
	else if (current == "attributeNames")
	{
		if (line.find("<hkcstring>") != string::npos)
		{
			cout << "ERROR: Unrecognized hkbBehaviorGraphStringData. Please report it to Nemesis' author" << endl;
			Error = true;
			return;

			string name = boost::regex_replace(string(line), boost::regex("([\t]+)<hkcstring>([^<]+)</hkcstring>"), string("\\2"));
			graphData.attributeNames.push_back(name);
		}
		else if (line.find("<hkparam name=\"variableNames\" numelements=\"") != string::npos)
		{
			current = "variableNames";
		}
	}
	else if (current == "variableNames")
	{
		if (line.find("<hkcstring>") != string::npos)
		{
			string name = boost::regex_replace(string(line), boost::regex("([\t]+)<hkcstring>([^<]+)</hkcstring>"), string("\\2"));
			graphData.variableNames.push_back(name);
		}
		else if (line.find("<hkparam name=\"characterPropertyNames\" numelements=\"") != string::npos)
		{
			current = "characterPropertyNames";
		}
	}
	else if (current == "characterPropertyNames")
	{
		if (line.find("<hkcstring>") != string::npos)
		{
			string name = boost::regex_replace(string(line), boost::regex("([\t]+)<hkcstring>([^<]+)</hkcstring>"), string("\\2"));
			graphData.characterPropertyNames.push_back(name);
		}
		else if (line.find("<hkparam name=\"characterPropertyNames\" numelements=\"") != string::npos)
		{
			current = "characterPropertyNames";
		}
	}
}

void inputGraphData(vector<string>& output, vector<string> orinamelist, vector<string> newnamelist, bool& IsEdited, string key)
{
	bool open = false;
	vector<string> storeline;
	size_t size = max(orinamelist.size(), newnamelist.size());

	if (orinamelist.size() != newnamelist.size())
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		open = true;

		if (newnamelist.size() > 0)
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"" + to_string(newnamelist.size()) + "\">");
		}
		else
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"0\"></hkparam>");
		}

		if (orinamelist.size() != 0)
		{
			storeline.push_back("			<hkparam name=\"" + key + "\" numelements=\"" + to_string(orinamelist.size()) + "\">");
		}
		else
		{
			storeline.push_back("			<hkparam name=\"" + key + "\" numelements=\"0\"></hkparam>");
		}

		IsEdited = true;
	}
	else
	{
		if (orinamelist.size() == 0)
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"0\"></hkparam>");
		}
		else
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"" + to_string(orinamelist.size()) + "\">");
		}
	}

	for (unsigned int i = 0; i < size; i++)
	{
		if (i < orinamelist.size())
		{
			if (i < newnamelist.size())
			{
				if (orinamelist[i] != newnamelist[i])
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back("				<hkcstring>" + newnamelist[i] + "</hkcstring>");
					storeline.push_back("				<hkcstring>" + orinamelist[i] + "</hkcstring>");
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						output.push_back("<!-- CLOSE -->");
						storeline.clear();
						open = false;
					}

					output.push_back("				<hkcstring>" + orinamelist[i] + "</hkcstring>");
				}
			}
			else
			{
				if (newnamelist.size() != 0)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (i < orinamelist.size())
				{
					output.push_back("");
					storeline.push_back("				<hkcstring>" + orinamelist[i] + "</hkcstring>");
					++i;
				}

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}
		}
		else
		{
			if (orinamelist.size() != 0)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			while (i < newnamelist.size())
			{
				output.push_back("				<hkcstring>" + newnamelist[i] + "</hkcstring>");
				++i;
			}
		}
	}

	if (orinamelist.size() != newnamelist.size())
	{
		if (newnamelist.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else if (orinamelist.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("<!-- CLOSE -->");
			output.push_back("			</hkparam>");
		}
	}
	else if (orinamelist.size() != 0)
	{
		if (open)
		{
			output.push_back("<!-- CLOSE -->");
		}

		output.push_back("			</hkparam>");
	}
}

namespace keepsake
{
	void hkbBehaviorGraphStringDataExport(string id)
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
			cout << "ERROR: Edit hkbBehaviorGraphStringData Input Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbBehaviorGraphStringData Output Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		// stage 2 identify edits
		vector<string> output;
		bool IsEdited = false;
		bool IsChanged = false;
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
			if ((storeline1[curline].find("<hkparam name=\"attributeNames\" numelements=", 0) == string::npos) && (part == 0)) // existing eventNames value
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

				curline++;
			}
			else if ((storeline1[curline].find("<hkparam name=\"variableNames\" numelements=", 0) == string::npos) && (part == 1)) // existing attributeNames value
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

				curline++;
			}
			else if ((storeline1[curline].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) == string::npos) && (part == 2)) // existing variableNames value
			{
				if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
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

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);
				}

				curline++;
			}
			else if ((curline != storeline1.size()) && (part == 3)) // existing characterPropertyNames value
			{
				if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
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

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);
				}

				if (curline + 1 != storeline1.size())
				{
					curline++;
				}
			}
			else // added variable value
			{
				if (storeline2[i].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos)
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

					part = 1;
					curline++;
				}
				else if (storeline2[i].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos)
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

					part = 2;
					curline++;
				}
				else if (storeline2[i].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos)
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

					part = 3;

					if (curline + 1 != storeline1.size())
					{
						curline++;
					}
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

						output.push_back(storeline2[i]);

						if (storeline2[i + 1].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos)
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
					else if (part == 1)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(storeline2[i]);

						if (storeline2[i + 1].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos)
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
					else if (part == 2)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(storeline2[i]);

						if (storeline2[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos)
						{
							if (IsChanged)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
								{
									output.push_back(storeline1[j]);
								}

								IsChanged = false;
							}

							output.push_back("<!-- CLOSE -->");
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
						if ((output[i + 1].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"eventNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
						{
							if (!closeEdit)
							{
								fwrite << "			</hkparam>" << "\n";
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"attributeNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
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
						else if ((output[i + 1].find("<hkparam name=\"variableNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
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
						else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
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
				cout << "ERROR: Edit hkbBehaviorGraphStringData Output Not Found (File: " << filename << ")" << endl;
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

}
