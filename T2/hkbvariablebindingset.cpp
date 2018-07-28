#include "hkbvariablebindingset.h"
#include "Global.h"
#include "highestscore.h"

using namespace std;

struct binding
{
	bool proxy = true;

	string memberPath;
	int variableIndex;
	int bitIndex;
	string bindingType;
};

void inputBinding(vector<string>& input, shared_ptr<binding> transition);
void bindingInfoProcess(string line, vector<shared_ptr<binding>>& binds, shared_ptr<binding>& curBind);
bool matchScoring(vector<shared_ptr<binding>>& ori, vector<shared_ptr<binding>>& edit, string id);

hkbvariablebindingset::hkbvariablebindingset(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "t" + to_string(functionlayer) + ">";

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

void hkbvariablebindingset::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableBindingSet(ID: " << id << ") has been initialized!" << endl;
	}
	
	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbVariableBindingSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbVariableBindingSet(ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablebindingset::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableBindingSet(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbVariableBindingSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbVariableBindingSet(newID: " << id << ") with hkbVariableBindingSet(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbVariableBindingSet(newID: " << id << ") with hkbVariableBindingSet(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbVariableBindingSet(ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablebindingset::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbVariableBindingSet(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].size() == 0)
	{
		cout << "ERROR: Dummy hkbVariableBindingSet Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbVariableBindingSet(ID: " << id << ") is complete!" << endl;
	}
}

string hkbvariablebindingset::GetAddress()
{
	return address;
}

bool hkbvariablebindingset::IsNegate()
{
	return IsNegated;
}

void hkbVariableBindingSetExport(string id)
{
	// stage 1 reading
	string line;
	string oriIOBE;
	vector<shared_ptr<binding>> oriBinds;
	shared_ptr<binding> curBind;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if (line.find("<hkparam name=\"indexOfBindingToEnable\">") != string::npos)
			{
				oriIOBE = line;
			}
			else
			{
				bindingInfoProcess(line, oriBinds, curBind);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbVariableBindingSet Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curBind)
	{
		curBind->proxy = false;
		oriBinds.push_back(curBind);
	}

	curBind = nullptr;
	string newIOBE;
	vector<shared_ptr<binding>> newBinds;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"indexOfBindingToEnable\">") != string::npos)
			{
				newIOBE = line;
			}
			else
			{
				bindingInfoProcess(line, newBinds, curBind);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbVariableBindingSet Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curBind)
	{
		curBind->proxy = false;
		newBinds.push_back(curBind);
	}

	if (!matchScoring(oriBinds, newBinds, id))
	{
		return;
	}

	// stage 2 identify edits
	vector<string> output;
	bool IsEdited = false;

	output.push_back(FunctionLineNew[id][0]);

	if ((FunctionLineTemp[id][1].find(FunctionLineNew[id][1], 0) == string::npos) || (FunctionLineTemp[id][1].length() != FunctionLineNew[id][1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(FunctionLineNew[id][1]);
		output.push_back("<!-- ORIGINAL -->");
		output.push_back(FunctionLineTemp[id][1]);
		output.push_back("<!-- CLOSE -->");
		IsEdited = true;
	}
	else
	{
		output.push_back(FunctionLineTemp[id][1]);
	}


	for (unsigned int i = 0; i < oriBinds.size(); i++)
	{
		vector<string> storeline;
		bool open = false;

		if (newBinds[i]->proxy)
		{
			vector<string> instore;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			bool nobreak = true;

			while (i < oriBinds.size())
			{
				if (!newBinds[i]->proxy)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), instore.begin(), instore.end());
					output.push_back("<!-- CLOSE -->");
					nobreak = false;
					--i;
					break;
				}

				int add = 0;

				while (add < 6)
				{
					output.push_back("");
					++add;
				}

				inputBinding(instore, oriBinds[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());
				output.push_back("<!-- CLOSE -->");
			}
		}
		else if (!oriBinds[i]->proxy)
		{
			output.push_back("				<hkobject>");

			if (oriBinds[i]->memberPath != newBinds[i]->memberPath)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"memberPath\">" + newBinds[i]->memberPath + "</hkparam>");
				storeline.push_back("					<hkparam name=\"memberPath\">" + oriBinds[i]->memberPath + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"memberPath\">" + oriBinds[i]->memberPath + "</hkparam>");
			}

			if (oriBinds[i]->variableIndex != newBinds[i]->variableIndex)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}
				
				output.push_back("					<hkparam name=\"variableIndex\">" + to_string(newBinds[i]->variableIndex) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"variableIndex\">" + to_string(oriBinds[i]->variableIndex) + "</hkparam>");
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

				output.push_back("					<hkparam name=\"variableIndex\">" + to_string(oriBinds[i]->variableIndex) + "</hkparam>");
			}

			if (oriBinds[i]->bitIndex != newBinds[i]->bitIndex)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"bitIndex\">" + to_string(newBinds[i]->bitIndex) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"bitIndex\">" + to_string(oriBinds[i]->bitIndex) + "</hkparam>");
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

				output.push_back("					<hkparam name=\"bitIndex\">" + to_string(oriBinds[i]->bitIndex) + "</hkparam>");
			}

			if (oriBinds[i]->bindingType != newBinds[i]->bindingType)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"bindingType\">" + newBinds[i]->bindingType + "</hkparam>");
				storeline.push_back("					<hkparam name=\"bindingType\">" + oriBinds[i]->bindingType + "</hkparam>");
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

				output.push_back("					<hkparam name=\"bindingType\">" + oriBinds[i]->bindingType + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;

			while (i < oriBinds.size())
			{
				inputBinding(output, newBinds[i]);
				++i;
			}

			output.push_back("<!-- CLOSE -->");
		}
	}

	output.push_back("			</hkparam>");

	if (oriIOBE != newIOBE)
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(newIOBE);
		output.push_back("<!-- ORIGINAL -->");
		output.push_back(oriIOBE);
		output.push_back("<!-- CLOSE -->");
		IsEdited = true;
	}
	else
	{
		output.push_back(oriIOBE);
	}

	output.push_back("		</hkobject>");

	for (auto& curline : output)
	{
		if (curline.find("<hkparam name=\"variableIndex\">", 0) != string::npos)
		{
			usize eventpos = curline.find("$variableID[");

			if (eventpos != string::npos)
			{
				eventpos += 12;
				string eventid = curline.substr(eventpos, curline.find("]$", eventpos) - eventpos + 2);

				if (!eventID[eventid].empty())
				{
					curline.replace(eventpos, curline.find("]$") - eventpos + 2, eventID[eventid]);
				}
				else
				{
					cout << "ERROR: Invalid event id. Please ensure that event id is valid (ID: " << id << ")" << endl;
					Error = true;
					return;
				}
			}
		}
	}

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
			cout << "ERROR: Edit hkbVariableBindingSet Output Not Found (File: " << filename << ")" << endl;
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

void bindingInfoProcess(string line, vector<shared_ptr<binding>>& binds, shared_ptr<binding>& curBind)
{
	if (line.find("<hkparam name=\"memberPath\">") != string::npos)
	{
		if (curBind)
		{
			curBind->proxy = false;
			binds.push_back(curBind);
		}

		curBind = make_shared<binding>();
		size_t pos = line.find("<hkparam name=\"memberPath\">") + 27;
		curBind->memberPath = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"variableIndex\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"variableIndex\">") + 30;
		curBind->variableIndex = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"bitIndex\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"bitIndex\">") + 25;
		curBind->bitIndex = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"bindingType\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"bindingType\">") + 28;
		curBind->bindingType = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
}

void inputBinding(vector<string>& input, shared_ptr<binding> bind)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"memberPath\">" + bind->memberPath + "</hkparam>");
	input.push_back("					<hkparam name=\"variableIndex\">" + to_string(bind->variableIndex) + "</hkparam>");
	input.push_back("					<hkparam name=\"bitIndex\">" + to_string(bind->bitIndex) + "</hkparam>");
	input.push_back("					<hkparam name=\"bindingType\">" + bind->bindingType + "</hkparam>");
	input.push_back("				</hkobject>");
}

bool matchScoring(vector<shared_ptr<binding>>& ori, vector<shared_ptr<binding>>& edit, string id)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: hkbVariableBindingSet empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 0;
	map<int, map<int, int>> scorelist;
	map<int, bool> taken;
	vector<shared_ptr<binding>> newOri;
	vector<shared_ptr<binding>> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->memberPath == edit[j]->memberPath)
			{
				scorelist[i][j] += 100;
			}

			if (ori[i]->variableIndex == edit[j]->variableIndex)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->bitIndex == edit[j]->bitIndex)
			{
				++scorelist[i][j];
			}

			if (ori[i]->bindingType == edit[j]->bindingType)
			{
				++scorelist[i][j];
			}
		}
	}
	
	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<binding>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<binding>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

namespace keepsake
{
	void hkbVariableBindingSetExport(string id)
	{
		// stage 1 reading
		vector<string> storeline1;
		string line;
		storeline1.reserve(FunctionLineTemp[id].size());

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
			cout << "ERROR: Edit hkbVariableBindingSet Input Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbVariableBindingSet Output Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		// stage 2 identify edits
		vector<string> output;
		bool IsChanged = false;
		bool open = false;
		bool IsEdited = false;
		int curline = 2;
		int openpoint;
		int closepoint;

		output.push_back(storeline2[0]);

		if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			openpoint = curline - 1;
			IsChanged = true;
			IsEdited = true;
			open = true;
		}

		output.push_back(storeline2[1]);

		for (unsigned int i = 2; i < storeline2.size(); i++)
		{
			if (storeline1[curline].find("<hkparam name=\"indexOfBindingToEnable\">", 0) == string::npos) // existing variable value
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
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}
				}

				curline++;
			}
			else // new element
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;

						if (storeline2[i].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos)
						{
							IsChanged = true;
						}

						IsEdited = true;
						open = true;
					}
				}
				else
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
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}

			if (storeline2[i].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos) // merging with new element or existing data or itself
			{
				if (open)
				{
					bool IsOpenOut = false;

					if ((openpoint == curline) && (IsChanged))
					{
						output.pop_back();
						output.push_back("			</hkparam>");
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);
					}
					else
					{
						output.push_back("			</hkparam>");
						output.push_back(storeline2[i]);
						IsOpenOut = true;
					}

					closepoint = curline + 1;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						if ((storeline1[j].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos) && (IsOpenOut))
						{
							output.push_back("			</hkparam>");
						}

						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
				else
				{
					output.push_back("			</hkparam>");
					output.push_back(storeline2[i]);
				}
			}
			else
			{
				output.push_back(storeline2[i]);
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
					if (output[i].find("<hkparam name=\"memberPath\">", 0) != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
						fwrite << output[i] << "\n";
					}
					else if (output[i].find("<hkparam name=\"bindingType\">", 0) != string::npos)
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
					}
					else if (output[i].find("<hkparam name=\"indexOfBindingToEnable\">", 0) != string::npos)
					{
						fwrite << output[i] << "\n";
					}
					else if (output[i].find("<hkparam name=\"variableIndex\">", 0) != string::npos && output[i].find("<hkparam name=\"variableIndex\">-1</hkparam>", 0) != string::npos)
					{
						usize varpos = output[i].find("variableIndex") + 15;
						string varID = output[i].substr(varpos, output[i].find("</hkparam>"));

						if (eventID[varID].length() != 0)
						{
							output[i].replace(varpos, output[i].find("</hkparam>") - varpos, "$variableID[" + variableID[varID] + "]$");
						}

						fwrite << output[i] << "\n";
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
				cout << "ERROR: Edit hkbVariableBindingSet Output Not Found (File: " << filename << ")" << endl;
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
