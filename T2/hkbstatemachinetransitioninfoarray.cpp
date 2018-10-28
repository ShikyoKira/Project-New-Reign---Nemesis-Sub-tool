#include "hkbstatemachinetransitioninfoarray.h"
#include "Global.h"
#include "highestscore.h"
#include "src\stateid.h"

using namespace std;

struct interval
{
	int enterEventID = -1;
	int exitEventID = -1;
	double enterTime = 0;
	double exitTime = 0;
};

struct transitioninfo
{
	bool proxy = true;

	interval trigger;
	interval initiate;
	string transition = "null";
	string condition = "null";
	string eventID = "-1";
	string stateID = "0";
	string toNestedStateID = "0";
	string fromNestStateID = "0";
	int priority = 0;
	string flags = "0";
};

string GetClass(string id, bool compare); // get class
void transitionInfoProcess(string line, vector<shared_ptr<transitioninfo>>& Transition, shared_ptr<transitioninfo>& curTransition, bool& trigger);
void inputTransition(std::vector<string>& input, shared_ptr<transitioninfo> transition);
bool matchScoring(vector<shared_ptr<transitioninfo>>& ori, vector<shared_ptr<transitioninfo>>& edit, string id);

hkbstatemachinetransitioninfoarray::hkbstatemachinetransitioninfoarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "q" + to_string(functionlayer) + ">";

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

void hkbstatemachinetransitioninfoarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineTransitionInfoArray (ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 1; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("#", 0) != string::npos)
			{
				int tempInt = line.find("#");
				string reference = line.substr(tempInt, line.find("</hkparam>") - tempInt);

				if (line.find("name=\"transition\">", 0) != string::npos)
				{
					transition.push_back(reference);
				}
				else
				{
					condition.push_back(reference);
				}

				if (!IsReferenceExist[reference])
				{
					referencingIDs[reference].push_back(id);
					IsReferenceExist[reference] = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineTransitionInfoArray (ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinetransitioninfoarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineTransitionInfoArray (ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	unordered_map<string, bool> IsReferenceExist;
	vector<string> newline;
	string line;

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
		vector<shared_ptr<transitioninfo>> newTransition;
		shared_ptr<transitioninfo> curTransition;

		if (!FunctionLineEdited[id].empty())
		{
			bool trigger = false;
			usize size = FunctionLineEdited[id].size();
			newline.push_back(FunctionLineEdited[id][0]);

			for (usize i = 1; i < size; i++)
			{
				line = FunctionLineEdited[id][i];
				transitionInfoProcess(line, newTransition, curTransition, trigger);
				newline.push_back(line);
			}
		}
		else
		{
			cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
			Error = true;
		}

		if (curTransition)
		{
			curTransition->proxy = false;
			newTransition.push_back(curTransition);
		}

		curTransition = nullptr;
		vector<shared_ptr<transitioninfo>> oriTransition;

		{
			string parentID = parent[id];

			while (parentID.length() > 0 && GetClass(parentID, true) != "hkbStateMachine")
			{
				parentID = parent[parentID];
			}

			if (isStateIDExist[parentID])
			{
				if (!FunctionLineTemp[tempid].empty())
				{
					bool trigger = false;
					usize size = FunctionLineTemp[tempid].size();

					for (usize i = 1; i < size; i++)
					{
						line = FunctionLineTemp[tempid][i];

						if (stateChange && line.find("<hkparam name=\"toStateId\">", 0) != string::npos)
						{
							string stateID = line.substr(31, line.find("</hkparam>", 31) - 31);

							if (stateID.find("stateID[") == string::npos && StateID[parentID].IsNewState(static_cast<unsigned int>(stoi(stateID))))
							{
								size_t oldStateIDSize = stateID.length();
								stateID = "$stateID[" + parentID + "][" + StateID[parentID].GetBaseStr() + "][" + modcode + "][" + StateID[parentID].GetStateID(stateID) + "]$";
								line.replace(31, oldStateIDSize, stateID);
							}
						}

						transitionInfoProcess(line, oriTransition, curTransition, trigger);
					}
				}
				else
				{
					cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
					Error = true;
				}
			}
			else
			{
				cout << "ERROR: hkbStateMachineTransitionInfoArray BUG FOUND! Missing StateID (node ID: " << parentID << ")" << endl;
				Error = true;
				return;
			}
		}

		if (curTransition)
		{
			curTransition->proxy = false;
			oriTransition.push_back(curTransition);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineTransitionInfoArray (newID: " << id << ") with hkbStateMachineTransitionInfoArray (oldID: " << tempid << ")" << endl;
		}

		if (!matchScoring(oriTransition, newTransition, id))
		{
			return;
		}

		for (auto& curInfo : newTransition)
		{
			if (!curInfo->proxy)
			{
				if (curInfo->transition != "null")
				{
					if (!exchangeID[curInfo->transition].empty())
					{
						curInfo->transition = exchangeID[curInfo->transition];
					}

					transition.push_back(curInfo->transition);

					if (!IsReferenceExist[curInfo->transition])
					{
						parent[curInfo->transition] = tempid;
						referencingIDs[curInfo->transition].push_back(tempid);
						IsReferenceExist[curInfo->transition] = true;
					}
				}

				if (curInfo->condition != "null")
				{
					if (!exchangeID[curInfo->condition].empty())
					{
						curInfo->condition = exchangeID[curInfo->condition];
					}

					condition.push_back(curInfo->condition);

					if (!IsReferenceExist[curInfo->condition])
					{
						parent[curInfo->condition] = tempid;
						referencingIDs[curInfo->condition].push_back(tempid);
						IsReferenceExist[curInfo->condition] = true;
					}
				}
			}
		}

		elements[tempid] = condition.size();
		elements[tempid + "T"] = transition.size();

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);
		FunctionLineNew[tempid].push_back(FunctionLineEdited[id][1]);

		for (auto& curInfo : newTransition)
		{
			if (!curInfo->proxy)
			{
				inputTransition(FunctionLineNew[tempid], curInfo);
			}
		}

		FunctionLineNew[tempid].push_back("			</hkparam>");
		FunctionLineNew[tempid].push_back("		</hkobject>");

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineTransitionInfoArray (newID: " << id << ") with hkbStateMachineTransitionInfoArray (oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		string parentID = parent[id];

		while (parentID.length() > 0 && GetClass(parentID, true) != "hkbStateMachine")
		{
			parentID = parent[parentID];
		}

		if (!IsForeign[parentID])
		{
			if (isStateIDExist[parentID])
			{
				if (!FunctionLineEdited[id].empty())
				{
					usize size = FunctionLineEdited[id].size();
					newline.push_back(FunctionLineEdited[id][0]);
					vector<string>* functionlines = &FunctionLineEdited[id];

					for (usize i = 1; i < size; i++)
					{
						string& line = (*functionlines)[i];

						if (line.find("#", 0) != string::npos)
						{
							size_t tempInt = line.find("#");
							string reference = line.substr(tempInt, line.find("</hkparam>") - tempInt);

							if (!exchangeID[reference].empty())
							{
								int tempint = line.find(reference);
								reference = exchangeID[reference];
								line.replace(tempint, line.find("</hkparam>") - tempint, reference);
							}

							if (line.find("name=\"transition\">", 0) != string::npos)
							{
								transition.push_back(reference);
							}
							else
							{
								condition.push_back(reference);
							}

							if (!IsReferenceExist[reference])
							{
								parent[reference] = id;
								referencingIDs[reference].push_back(id);
								IsReferenceExist[reference] = true;
							}
						}
						else if (stateChange && line.find("<hkparam name=\"toStateId\">", 0) != string::npos)
						{
							string stateID = line.substr(31, line.find("</hkparam>", 31) - 31);

							if (stateID.find("stateID[") == string::npos && StateID[parentID].IsNewState(static_cast<unsigned int>(stoi(stateID))))
							{
								size_t oldStateIDSize = stateID.length();
								stateID = "$stateID[" + parentID + "][" + StateID[parentID].GetBaseStr() + "][" + modcode + "][" + StateID[parentID].GetStateID(stateID) + "]$";
								line.replace(31, oldStateIDSize, stateID);
							}
						}

						newline.push_back(line);
					}
				}
				else
				{
					cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
					Error = true;
				}
			}
			else
			{
				cout << "ERROR: hkbStateMachineTransitionInfoArray BUG FOUND! Missing StateID (node ID: " << parent[id] << ")" << endl;
				Error = true;
				return;
			}
		}
		else
		{
			if (!FunctionLineEdited[id].empty())
			{
				usize size = FunctionLineEdited[id].size();
				newline.push_back(FunctionLineEdited[id][0]);

				for (usize i = 1; i < size; i++)
				{
					line = FunctionLineEdited[id][i];

					if (line.find("#", 0) != string::npos)
					{
						size_t tempInt = line.find("#");
						string reference = line.substr(tempInt, line.find("</hkparam>") - tempInt);

						if (!exchangeID[reference].empty())
						{
							int tempint = line.find(reference);
							reference = exchangeID[reference];
							line.replace(tempint, line.find("</hkparam>") - tempint, reference);
						}

						if (line.find("name=\"transition\">", 0) != string::npos)
						{
							transition.push_back(reference);
						}
						else
						{
							condition.push_back(reference);
						}

						if (!IsReferenceExist[reference])
						{
							parent[reference] = id;
							referencingIDs[reference].push_back(id);
							IsReferenceExist[reference] = true;
						}
					}

					newline.push_back(line);
				}
			}
			else
			{
				cout << "ERROR: hkbStateMachineTransitionInfoArray Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
				Error = true;
			}
		}

		IsForeign[id] = true;
		elements[id] = condition.size();
		elements[id + "T"] = transition.size();
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineTransitionInfoArray (ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinetransitioninfoarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachineTransitionInfoArray (ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("#", 0) != string::npos && line.find("signature", 0) == string::npos && line.find("$stateID[", 0) == string::npos)
			{
				string reference = boost::regex_replace(string(line), boost::regex(".*<hkparam name=\"(transition|condition)\">(#[0-9]+)</hkparam>.*"), string("\\2"));

				if (reference != line)
				{
					if (!exchangeID[reference].empty())
					{
						reference = exchangeID[reference];
					}

					if (line.find("name=\"transition\">", 0) != string::npos)
					{
						transition.push_back(reference);
					}
					else if (line.find("name=\"condition\">", 0) != string::npos)
					{
						condition.push_back(reference);
					}

					if (!IsReferenceExist[reference])
					{
						parent[reference] = id;
						IsReferenceExist[reference] = true;
					}
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachineTransitionInfoArray Inputfile (ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachineTransitionInfoArray (ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachinetransitioninfoarray::GetTransition(int number)
{
	return "#" + boost::regex_replace(string(transition[number]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachinetransitioninfoarray::GetTransitionCount()
{
	return transition.size();
}

string hkbstatemachinetransitioninfoarray::GetCondition(int number)
{
	return "#" + boost::regex_replace(string(condition[number]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachinetransitioninfoarray::GetConditionCount()
{
	return condition.size();
}

string hkbstatemachinetransitioninfoarray::GetAddress()
{
	return address;
}

bool hkbstatemachinetransitioninfoarray::IsNegate()
{
	return IsNegated;
}

void hkbStateMachineTransitionInfoArrayExport(string id)
{
	// stage 1 reading
	vector<shared_ptr<transitioninfo>> oriTransition;
	shared_ptr<transitioninfo> curTransition;
	vector<string> test;
	vector<string> test2;

	if (FunctionLineTemp[id].size() > 0)
	{
		bool trigger = false;
		test = FunctionLineTemp[id];

		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			transitionInfoProcess(FunctionLineTemp[id][i], oriTransition, curTransition, trigger);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curTransition)
	{
		curTransition->proxy = false;
		oriTransition.push_back(curTransition);
	}

	curTransition = nullptr;
	vector<shared_ptr<transitioninfo>> newTransition;

	if (FunctionLineNew[id].size() > 0)
	{
		bool trigger = false;
		test2 = FunctionLineNew[id];

		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			transitionInfoProcess(FunctionLineNew[id][i], newTransition, curTransition, trigger);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curTransition)
	{
		curTransition->proxy = false;
		newTransition.push_back(curTransition);
	}

	if (!matchScoring(oriTransition, newTransition, id))
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

	for (unsigned int i = 0; i < oriTransition.size(); i++)
	{
		vector<string> storeline;
		bool open = false;

		if (newTransition[i]->proxy)
		{
			vector<string> instore;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			bool nobreak = true;

			while (i < oriTransition.size())
			{
				if (!newTransition[i]->proxy)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), instore.begin(), instore.end());
					output.push_back("<!-- CLOSE -->");
					nobreak = false;
					--i;
					break;
				}

				int add = 0;

				while (add < 26)
				{
					output.push_back("");
					++add;
				}

				inputTransition(instore, oriTransition[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());
				output.push_back("<!-- CLOSE -->");
			}
		}
		else if (!oriTransition[i]->proxy)
		{
			output.push_back("				<hkobject>");
			output.push_back("					<hkparam name=\"triggerInterval\">");
			output.push_back("						<hkobject>");

			if (oriTransition[i]->trigger.enterEventID != newTransition[i]->trigger.enterEventID)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("							<hkparam name=\"enterEventId\">" + to_string(newTransition[i]->trigger.enterEventID) + "</hkparam>");
				storeline.push_back("							<hkparam name=\"enterEventId\">" + to_string(oriTransition[i]->trigger.enterEventID) + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"enterEventId\">" + to_string(oriTransition[i]->trigger.enterEventID) + "</hkparam>");
			}

			if (oriTransition[i]->trigger.exitEventID != newTransition[i]->trigger.exitEventID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("							<hkparam name=\"exitEventId\">" + to_string(newTransition[i]->trigger.exitEventID) + "</hkparam>");
				storeline.push_back("							<hkparam name=\"exitEventId\">" + to_string(oriTransition[i]->trigger.exitEventID) + "</hkparam>");
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

				output.push_back("							<hkparam name=\"exitEventId\">" + to_string(oriTransition[i]->trigger.exitEventID) + "</hkparam>");
			}

			if (oriTransition[i]->trigger.enterTime != newTransition[i]->trigger.enterTime)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				stringstream enter1;
				stringstream enter2;

				enter1 << setprecision(6) << fixed << newTransition[i]->trigger.enterTime;
				enter2 << setprecision(6) << fixed << oriTransition[i]->trigger.enterTime;

				output.push_back("							<hkparam name=\"enterTime\">" + enter1.str() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"enterTime\">" + enter2.str() + "</hkparam>");
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

				stringstream enter;
				enter << setprecision(6) << fixed << oriTransition[i]->trigger.enterTime;
				output.push_back("							<hkparam name=\"enterTime\">" + enter.str() + "</hkparam>");
			}

			if (oriTransition[i]->trigger.exitTime != newTransition[i]->trigger.exitTime)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				stringstream enter1;
				stringstream enter2;

				enter1 << setprecision(6) << fixed << newTransition[i]->trigger.exitTime;
				enter2 << setprecision(6) << fixed << oriTransition[i]->trigger.exitTime;

				output.push_back("							<hkparam name=\"exitTime\">" + enter1.str() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"exitTime\">" + enter2.str() + "</hkparam>");

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
				storeline.clear();
				open = false;
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

				stringstream enter;
				enter << setprecision(6) << fixed << oriTransition[i]->trigger.exitTime;
				output.push_back("							<hkparam name=\"exitTime\">" + enter.str() + "</hkparam>");
			}

			output.push_back("						</hkobject>");
			output.push_back("					</hkparam>");
			output.push_back("					<hkparam name=\"initiateInterval\">");
			output.push_back("						<hkobject>");

			if (oriTransition[i]->initiate.enterEventID != newTransition[i]->initiate.enterEventID)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("							<hkparam name=\"enterEventId\">" + to_string(newTransition[i]->initiate.enterEventID) + "</hkparam>");
				storeline.push_back("							<hkparam name=\"enterEventId\">" + to_string(oriTransition[i]->initiate.enterEventID) + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"enterEventId\">" + to_string(oriTransition[i]->initiate.enterEventID) + "</hkparam>");
			}

			if (oriTransition[i]->initiate.exitEventID != newTransition[i]->initiate.exitEventID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("							<hkparam name=\"exitEventId\">" + to_string(newTransition[i]->initiate.exitEventID) + "</hkparam>");
				storeline.push_back("							<hkparam name=\"exitEventId\">" + to_string(oriTransition[i]->initiate.exitEventID) + "</hkparam>");
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

				output.push_back("							<hkparam name=\"exitEventId\">" + to_string(oriTransition[i]->initiate.exitEventID) + "</hkparam>");
			}

			if (oriTransition[i]->initiate.enterTime != newTransition[i]->initiate.enterTime)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				stringstream enter1;
				stringstream enter2;

				enter1 << setprecision(6) << fixed << newTransition[i]->initiate.enterTime;
				enter2 << setprecision(6) << fixed << oriTransition[i]->initiate.enterTime;

				output.push_back("							<hkparam name=\"enterTime\">" + enter1.str() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"enterTime\">" + enter2.str() + "</hkparam>");
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

				stringstream enter;
				enter << setprecision(6) << fixed << oriTransition[i]->initiate.enterTime;
				output.push_back("							<hkparam name=\"enterTime\">" + enter.str() + "</hkparam>");
			}

			if (oriTransition[i]->initiate.exitTime != newTransition[i]->initiate.exitTime)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				stringstream enter1;
				stringstream enter2;

				enter1 << setprecision(6) << fixed << newTransition[i]->initiate.exitTime;
				enter2 << setprecision(6) << fixed << oriTransition[i]->initiate.exitTime;

				output.push_back("							<hkparam name=\"exitTime\">" + enter1.str() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"exitTime\">" + enter2.str() + "</hkparam>");

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
				storeline.clear();
				open = false;
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

				stringstream enter;
				enter << setprecision(6) << fixed << oriTransition[i]->initiate.exitTime;
				output.push_back("							<hkparam name=\"exitTime\">" + enter.str() + "</hkparam>");
			}

			output.push_back("						</hkobject>");
			output.push_back("					</hkparam>");

			if (oriTransition[i]->transition != newTransition[i]->transition)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"transition\">" + newTransition[i]->transition + "</hkparam>");
				storeline.push_back("					<hkparam name=\"transition\">" + oriTransition[i]->transition + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"transition\">" + oriTransition[i]->transition + "</hkparam>");
			}

			if (oriTransition[i]->condition != newTransition[i]->condition)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"condition\">" + newTransition[i]->condition + "</hkparam>");
				storeline.push_back("					<hkparam name=\"condition\">" + oriTransition[i]->condition + "</hkparam>");
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

				output.push_back("					<hkparam name=\"condition\">" + oriTransition[i]->condition + "</hkparam>");
			}

			if (oriTransition[i]->eventID != newTransition[i]->eventID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"eventId\">" + newTransition[i]->eventID + "</hkparam>");
				storeline.push_back("					<hkparam name=\"eventId\">" + oriTransition[i]->eventID + "</hkparam>");
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

				output.push_back("					<hkparam name=\"eventId\">" + oriTransition[i]->eventID + "</hkparam>");
			}

			if (oriTransition[i]->stateID != newTransition[i]->stateID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"toStateId\">" + newTransition[i]->stateID + "</hkparam>");
				storeline.push_back("					<hkparam name=\"toStateId\">" + oriTransition[i]->stateID + "</hkparam>");
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

				output.push_back("					<hkparam name=\"toStateId\">" + oriTransition[i]->stateID + "</hkparam>");
			}

			if (oriTransition[i]->fromNestStateID != newTransition[i]->fromNestStateID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"fromNestedStateId\">" + newTransition[i]->fromNestStateID + "</hkparam>");
				storeline.push_back("					<hkparam name=\"fromNestedStateId\">" + oriTransition[i]->fromNestStateID + "</hkparam>");
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

				output.push_back("					<hkparam name=\"fromNestedStateId\">" + oriTransition[i]->fromNestStateID + "</hkparam>");
			}

			if (oriTransition[i]->toNestedStateID != newTransition[i]->toNestedStateID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"toNestedStateId\">" + newTransition[i]->toNestedStateID + "</hkparam>");
				storeline.push_back("					<hkparam name=\"toNestedStateId\">" + oriTransition[i]->toNestedStateID + "</hkparam>");
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

				output.push_back("					<hkparam name=\"toNestedStateId\">" + oriTransition[i]->toNestedStateID + "</hkparam>");
			}

			if (oriTransition[i]->priority != newTransition[i]->priority)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"priority\">" + to_string(newTransition[i]->priority) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"priority\">" + to_string(oriTransition[i]->priority) + "</hkparam>");
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

				output.push_back("					<hkparam name=\"priority\">" + to_string(oriTransition[i]->priority) + "</hkparam>");
			}

			if (oriTransition[i]->flags != newTransition[i]->flags)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"flags\">" + newTransition[i]->flags + "</hkparam>");
				storeline.push_back("					<hkparam name=\"flags\">" + oriTransition[i]->flags + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
				}

				output.push_back("					<hkparam name=\"flags\">" + oriTransition[i]->flags + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;

			while (i < oriTransition.size())
			{
				inputTransition(output, newTransition[i]);
				++i;
			}

			output.push_back("<!-- CLOSE -->");
		}
	}
	
	output.push_back("			</hkparam>");
	output.push_back("		</hkobject>");
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
				fwrite << output[i] << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Output Not Found (File: " << filename << ")" << endl;
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

void transitionInfoProcess(string line, vector<shared_ptr<transitioninfo>>& Transition, shared_ptr<transitioninfo>& curTransition, bool& trigger)
{
	if (line.find("<hkparam name=\"triggerInterval\">") != string::npos)
	{
		if (curTransition)
		{
			curTransition->proxy = false;
			Transition.push_back(curTransition);
		}

		curTransition = make_shared<transitioninfo>();
		trigger = true;
	}

	if (line.find("<hkparam name=\"initiateInterval\">") != string::npos)
	{
		trigger = false;
	}
	else if (line.find("<hkparam name=\"enterEventId\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"enterEventId\">") + 29;

		if (trigger)
		{
			curTransition->trigger.enterEventID = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
		else
		{
			curTransition->initiate.enterEventID = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
	}
	else if (line.find("<hkparam name=\"exitEventId\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"exitEventId\">") + 28;

		if (trigger)
		{
			curTransition->trigger.exitEventID = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
		else
		{
			curTransition->initiate.exitEventID = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
	}
	else if (line.find("<hkparam name=\"enterTime\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"enterTime\">") + 26;

		if (trigger)
		{
			curTransition->trigger.enterTime = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
		else
		{
			curTransition->initiate.enterTime = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
	}
	else if (line.find("<hkparam name=\"exitTime\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"exitTime\">") + 25;

		if (trigger)
		{
			curTransition->trigger.exitTime = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
		else
		{
			curTransition->initiate.exitTime = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
		}
	}
	else if (line.find("<hkparam name=\"transition\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"transition\">") + 27;
		curTransition->transition = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"condition\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"condition\">") + 26;
		curTransition->condition = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"eventId\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"eventId\">") + 24;
		curTransition->eventID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"toStateId\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"toStateId\">") + 26;
		curTransition->stateID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"fromNestedStateId\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"fromNestedStateId\">") + 34;
		curTransition->fromNestStateID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"toNestedStateId\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"toNestedStateId\">") + 32;
		curTransition->toNestedStateID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"priority\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"priority\">") + 25;
		curTransition->priority = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"flags\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"flags\">") + 22;
		curTransition->flags = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
}

void inputTransition(vector<string>& input, shared_ptr<transitioninfo> transition)
{
	stringstream tenter;
	tenter << setprecision(6) << fixed << transition->trigger.enterTime;

	stringstream texit;
	texit << setprecision(6) << fixed << transition->trigger.enterTime;

	stringstream ienter;
	ienter << setprecision(6) << fixed << transition->initiate.enterTime;

	stringstream iexit;
	iexit << setprecision(6) << fixed << transition->initiate.enterTime;

	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"triggerInterval\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"enterEventId\">" + to_string(transition->trigger.enterEventID) + "</hkparam>");
	input.push_back("							<hkparam name=\"exitEventId\">" + to_string(transition->trigger.exitEventID) + "</hkparam>");
	input.push_back("							<hkparam name=\"enterTime\">" + tenter.str() + "</hkparam>");
	input.push_back("							<hkparam name=\"exitTime\">" + texit.str() + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"initiateInterval\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"enterEventId\">" + to_string(transition->initiate.enterEventID) + "</hkparam>");
	input.push_back("							<hkparam name=\"exitEventId\">" + to_string(transition->initiate.exitEventID) + "</hkparam>");
	input.push_back("							<hkparam name=\"enterTime\">" + ienter.str() + "</hkparam>");
	input.push_back("							<hkparam name=\"exitTime\">" + iexit.str() + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"transition\">" + transition->transition + "</hkparam>");
	input.push_back("					<hkparam name=\"condition\">" + transition->condition + "</hkparam>");
	input.push_back("					<hkparam name=\"eventId\">" + transition->eventID + "</hkparam>");
	input.push_back("					<hkparam name=\"toStateId\">" + transition->stateID + "</hkparam>");
	input.push_back("					<hkparam name=\"fromNestedStateId\">" + transition->fromNestStateID + "</hkparam>");
	input.push_back("					<hkparam name=\"toNestedStateId\">" + transition->toNestedStateID + "</hkparam>");
	input.push_back("					<hkparam name=\"priority\">" + to_string(transition->priority) + "</hkparam>");
	input.push_back("					<hkparam name=\"flags\">" + transition->flags + "</hkparam>");
	input.push_back("				</hkobject>");
}

bool matchScoring(vector<shared_ptr<transitioninfo>>& ori, vector<shared_ptr<transitioninfo>>& edit, string id)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: hkbStateMachineTransitionInfoArray empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 0;
	map<int, map<int, double>> scorelist;
	map<int, bool> taken;
	vector<shared_ptr<transitioninfo>> newOri;
	vector<shared_ptr<transitioninfo>> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->trigger.enterEventID == edit[j]->trigger.enterEventID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->trigger.exitEventID == edit[j]->trigger.exitEventID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->trigger.enterTime == edit[j]->trigger.enterTime)
			{
				++scorelist[i][j];
			}

			if (ori[i]->trigger.exitTime == edit[j]->trigger.exitTime)
			{
				++scorelist[i][j];
			}

			if (ori[i]->initiate.enterEventID == edit[j]->initiate.enterEventID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->initiate.exitEventID == edit[j]->initiate.exitEventID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->initiate.enterTime == edit[j]->initiate.enterTime)
			{
				++scorelist[i][j];
			}

			if (ori[i]->initiate.exitTime == edit[j]->initiate.exitTime)
			{
				++scorelist[i][j];
			}

			if (ori[i]->transition == edit[j]->transition)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->condition == edit[j]->condition)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->eventID == edit[j]->eventID)
			{
				scorelist[i][j] += 6;
			}

			if (ori[i]->stateID == edit[j]->stateID)
			{
				scorelist[i][j] += 6;
			}

			if (ori[i]->toNestedStateID == edit[j]->toNestedStateID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->fromNestStateID == edit[j]->fromNestStateID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->priority == edit[j]->priority)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i]->flags == edit[j]->flags)
			{
				scorelist[i][j] += 2;
			}

			if (i == j)
			{
				++scorelist[i][j];
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<transitioninfo>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<transitioninfo>());
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
	void hkbStateMachineTransitionInfoArrayExport(string id)
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
			cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Input Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		vector<string> storeline2;

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
			cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Output Not Found (ID: " << id << ")" << endl;
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
								if ((storeline2[i].find("<hkparam name=\"triggerInterval\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
								{
									output.push_back("				<hkobject>");
								}

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
						if (storeline2[i].find("<hkparam name=\"triggerInterval\">", 0) != string::npos)
						{
							output.push_back("				<hkobject>");
						}

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
						output.push_back("				<hkobject>");
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
								if (storeline2[i].find("<hkparam name=\"flags\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}

								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"flags\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}
								}
							}
						}
						else
						{
							if (storeline2[i].find("<hkparam name=\"flags\">", 0) != string::npos)
							{
								output.push_back("				</hkobject>");
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}
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
					if (output[i].find("<hkparam name=\"triggerInterval\">", 0) != string::npos)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							if (output[i - 1].find("ORIGINAL", 0) == string::npos)
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							fwrite << "						<hkobject>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							if (output[i - 1].find("OPEN", 0) == string::npos)
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else
						{
							if ((output[i - 1].find("ORIGINAL", 0) == string::npos) && (output[i - 1].find("OPEN", 0) == string::npos) && (output[i - 1].find("<hkobject>", 0) == string::npos))
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";
							fwrite << "						<hkobject>" << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"initiateInterval\">", 0) != string::npos)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							fwrite << "						<hkobject>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "						<hkobject>" << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"exitTime\">", 0) != string::npos)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"flags\">", 0) != string::npos)
					{
						if (i != output.size() - 1)
						{
							if (output[i + 1].find("CLOSE", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
								fwrite << "<!-- CLOSE -->" << "\n";
								fwrite << "				</hkobject>" << "\n";
								i++;
							}
							else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
								fwrite << "<!-- ORIGINAL -->" << "\n";
								i++;
							}
							else if (output[i + 1].find("</hkobject>", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
							}
							else if (output[i + 1].find("<hkobject>", 0) != string::npos)
							{
								if (output[i + 2].find("ORIGINAL", 0) != string::npos)
								{
									fwrite << output[i] << "\n";
									fwrite << "<!-- ORIGINAL -->" << "\n";
									i += 2;
								}
								else
								{
									fwrite << output[i] << "\n";
									fwrite << "				</hkobject>" << "\n";
								}
							}
							else
							{
								fwrite << output[i] << "\n";
								fwrite << "				</hkobject>" << "\n";
							}
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "				</hkobject>" << "\n";
						}
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
				cout << "ERROR: Edit hkbStateMachineTransitionInfoArray Output Not Found (File: " << filename << ")" << endl;
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
