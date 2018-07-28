#include "hkbstatemachineeventpropertyarray.h"
#include "Global.h"
#include "highestscore.h"

using namespace std;

struct events
{
	bool proxy = true;

	unsigned int id;
	string payload;
};

void inputEvent(vector<string>& input, shared_ptr<events> transition);
void eventInfoProcess(string line, vector<shared_ptr<events>>& binds, shared_ptr<events>& curBind);
bool matchScoring(vector<shared_ptr<events>>& ori, vector<shared_ptr<events>>& edit, string id);

hkbstatemachineeventpropertyarray::hkbstatemachineeventpropertyarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "o" + to_string(functionlayer) + ">";

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

void hkbstatemachineeventpropertyarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineEventPropertyArray (ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						payload.back() = exchangeID[payload.back()];
					}

					referencingIDs[payload.back()].push_back(id);
				}

			}
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineEventPropertyArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineEventPropertyArray (ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachineeventpropertyarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineEventPropertyArray (ID: " << id << ") has been initialized!" << endl;
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

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
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
		cout << "ERROR: hkbStateMachineEventPropertyArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbStateMachineEventPropertyArray (newID: " << id << ") with hkbStateMachineEventPropertyArray(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID
		
		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
				parent[payload[i]] = tempid;
			}
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
			cout << "Comparing hkbStateMachineEventPropertyArray (newID: " << id << ") with hkbStateMachineEventPropertyArray(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbStateMachineEventPropertyArray (ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachineeventpropertyarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachineEventPropertyArray (ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
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
		cout << "ERROR: Dummy hkbStateMachineEventPropertyArray Inputfile (ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachineEventPropertyArray (ID: " << id << ") is complete!" << endl;
	}
}

int hkbstatemachineeventpropertyarray::GetPayloadCount()
{
	return int(payload.size());
}

string hkbstatemachineeventpropertyarray::GetPayload(int child)
{
	return payload[child];
}

bool hkbstatemachineeventpropertyarray::IsPayloadNull(int child)
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

string hkbstatemachineeventpropertyarray::GetAddress()
{
	return address;
}

bool hkbstatemachineeventpropertyarray::IsNegate()
{
	return IsNegated;
}

void hkbStateMachineEventPropertyArrayExport(string id)
{
	// stage 1 reading
	vector<shared_ptr<events>> oriEvents;
	shared_ptr<events> curEvent;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			eventInfoProcess(FunctionLineTemp[id][i], oriEvents, curEvent);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbStateMachineEventPropertyArray Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curEvent)
	{
		curEvent->proxy = false;
		oriEvents.push_back(curEvent);
	}

	curEvent = nullptr;
	vector<shared_ptr<events>> newEvents;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			eventInfoProcess(FunctionLineNew[id][i], newEvents, curEvent);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbStateMachineEventPropertyArray Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curEvent)
	{
		curEvent->proxy = false;
		newEvents.push_back(curEvent);
	}

	if (!matchScoring(oriEvents, newEvents, id))
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

	for (unsigned int i = 0; i < oriEvents.size(); i++)
	{
		vector<string> storeline;
		bool open = false;

		if (newEvents[i]->proxy)
		{
			vector<string> instore;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			bool nobreak = true;

			while (i < oriEvents.size())
			{
				if (!newEvents[i]->proxy)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), instore.begin(), instore.end());
					output.push_back("<!-- CLOSE -->");
					nobreak = false;
					--i;
					break;
				}

				int add = 0;

				while (add < 4)
				{
					output.push_back("");
					++add;
				}

				inputEvent(instore, oriEvents[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());
				output.push_back("<!-- CLOSE -->");
			}
		}
		else if (!oriEvents[i]->proxy)
		{
			output.push_back("				<hkobject>");

			if (oriEvents[i]->id != newEvents[i]->id)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"id\">" + to_string(newEvents[i]->id) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"id\">" + to_string(oriEvents[i]->id) + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"id\">" + to_string(oriEvents[i]->id) + "</hkparam>");
			}

			if (oriEvents[i]->payload != newEvents[i]->payload)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"payload\">" + newEvents[i]->payload + "</hkparam>");
				storeline.push_back("					<hkparam name=\"payload\">" + oriEvents[i]->payload + "</hkparam>");
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

				output.push_back("					<hkparam name=\"payload\">" + oriEvents[i]->payload + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;

			while (i < oriEvents.size())
			{
				inputEvent(output, newEvents[i]);
				++i;
			}

			output.push_back("<!-- CLOSE -->");
		}
	}

	output.push_back("			</hkparam>");
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
			cout << "ERROR: Edit hkbStateMachineEventPropertyArray Output Not Found (File: " << filename << ")" << endl;
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

void eventInfoProcess(string line, vector<shared_ptr<events>>& eventlist, shared_ptr<events>& curEvent)
{
	if (line.find("<hkparam name=\"id\">") != string::npos)
	{
		if (curEvent)
		{
			curEvent->proxy = false;
			eventlist.push_back(curEvent);
		}

		curEvent = make_shared<events>();
		size_t pos = line.find("<hkparam name=\"id\">") + 19;
		curEvent->id = abs(stoi(line.substr(pos, line.find("</hkparam>", pos) - pos)));
	}
	else if (line.find("<hkparam name=\"payload\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"payload\">") + 24;
		curEvent->payload = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
}

void inputEvent(vector<string>& input, shared_ptr<events> element)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"id\">" + to_string(element->id) + "</hkparam>");
	input.push_back("					<hkparam name=\"payload\">" + element->payload + "</hkparam>");
	input.push_back("				</hkobject>");
}

bool matchScoring(vector<shared_ptr<events>>& ori, vector<shared_ptr<events>>& edit, string id)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: hkbStateMachineEventPropertyArray empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 0;
	map<int, map<int, int>> scorelist;
	map<int, bool> taken;
	vector<shared_ptr<events>> newOri;
	vector<shared_ptr<events>> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->id == edit[j]->id)
			{
				scorelist[i][j] += 10;
			}

			if (ori[i]->payload == edit[j]->payload)
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
			newOri.push_back(make_shared<events>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<events>());
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
	void hkbStateMachineEventPropertyArrayExport(string id)
	{
		// stage 1 reading
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
			cout << "ERROR: Edit hkbStateMachineEventPropertyArray Input Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbStateMachineEventPropertyArray Output Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		// stage 2 identify edits
		vector<string> output;
		bool newtransition = false;
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
			if (!newtransition) // existing data
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
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
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

				output.push_back(storeline2[i]);

				if (curline != storeline1.size() - 1)
				{
					curline++;
				}
				else
				{
					newtransition = true;
				}

				if (i == storeline2.size() - 1) // if close no new element
				{
					if (open)
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
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}
			}
			else // new added data
			{
				if (i != storeline2.size() - 1)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);

					if (open)
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
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
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
					if (output[i].find("<hkparam name=\"id\">", 0) != string::npos && output[i].find("<hkparam name=\"id\">-1</hkparam>", 0) == string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
						usize eventpos = output[i].find("id\">") + 4;
						string eventid = output[i].substr(eventpos, output[i].find("</hkparam>"));

						if (eventID[eventid].length() != 0)
						{
							output[i].replace(eventpos, output[i].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
						}

						fwrite << output[i] << "\n";
					}
					else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}

				fwrite << "			</hkparam>" << "\n";
				fwrite << "		</hkobject>" << "\n";
				outputfile.close();
			}
			else
			{
				cout << "ERROR: Edit hkbStateMachineEventPropertyArray Output Not Found (File: " << filename << ")" << endl;
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
