#include <boost\thread.hpp>
#include "hkbstatemachine.h"
#include "generatorlines.h"
#include "src\stateid.h"

using namespace std;

namespace statemachine
{
	string key = "r";
	string classname = "hkbStateMachine";
	string signature = "0x816c1dcb";
}

string GetClass(string id, bool compare); // get class

hkbstatemachine::hkbstatemachine(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + statemachine::key + to_string(functionlayer) + ">region";

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
		bool statusChanged = false;

		if (IsForeign[id])
		{
			statusChanged = true;
		}

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
			else if (stateChange && statusChanged)
			{
				string temp = preaddress + "r" + to_string(functionlayer) + ">region";
				string lastSMAdd = boost::regex_replace(string(temp), boost::regex("(^.*[(]r[0-9]+[)]=>[^>]+>)[^>]+.*"), string("\\1"));

				if (lastSMAdd != temp)
				{
					string parentID = parent[addressID[lastSMAdd]];

					if (parentID.length() == 0)
					{
						cout << "ERROR: hkbStateMachine missing parent ID (ID: " << id << ")" << endl;
						Error = true;
						return;
					}

					if (isSMIDExist[parentID])
					{
						previousSM = StateMachineID[parentID];
						previousSMExist = true;
					}
				}
				else
				{
					string parentID = parent[id];

					while (parentID.length() > 0 && GetClass(parentID, true) != "hkbStateMachine")
					{
						if (IsForeign[parentID])
						{
							cout << "ERROR: hkbStateMachine foreign parent ID (ID: " << parentID << ")" << endl;
							Error = true;
							return;
						}

						parentID = parent[parentID];
					}

					if (isSMIDExist[parentID])
					{
						previousSM = StateMachineID[parentID];
						previousSMExist = true;
					}
				}

				StateMachineID[dummyID] = shared_from_this();
				isSMIDExist[dummyID] = true;
			}
		}
		else
		{
			IsNegated = true;
			address = region[id];
		}
	}
	else
	{
		return;
	}
}

void hkbstatemachine::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachine (ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;
	bool pauseline = false;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
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
						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
						referencingIDs[tempgenerator].push_back(id);
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
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);

					if (payload != "null")
					{
						parent[payload] = id;
						referencingIDs[payload].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;

					if (children > 0)
					{
						isStateIDExist[id] = true;
					}
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						parent[wildcard] = id;
						referencingIDs[wildcard].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"name\">", 0) != string::npos)
				{
					s_name = line.substr(24, line.find("</hkparam>") - 24);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	address = s_name + "(r" + to_string(regioncount[s_name]) + ")=>";
	regioncount[s_name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachine::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
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
							usize tempint = line.find(tempgenerator);
							int templength = tempgenerator.length();
							tempgenerator = exchangeID[tempgenerator];
							line.replace(tempint, templength, tempgenerator);
						}

						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
						referencingIDs[tempgenerator].push_back(id);
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
							int tempint = line.find(variablebindingset);
							line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[variablebindingset]);
							variablebindingset = exchangeID[variablebindingset];
						}

						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);

					if (payload != "null")
					{
						if (!exchangeID[payload].empty())
						{
							int tempint = line.find(payload);
							payload = exchangeID[payload];
							line.replace(tempint, line.find("</hkparam>") - tempint, payload);
						}

						parent[payload] = id;
						referencingIDs[payload].push_back(id);
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						if (!exchangeID[wildcard].empty())
						{
							usize tempint = line.find(wildcard);
							line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[wildcard]);
							wildcard = exchangeID[wildcard];
						}

						parent[wildcard] = id;
						referencingIDs[wildcard].push_back(id);
					}
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachine Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		elements[tempid] = children;
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachine(newID: " << id << ") with hkbStateMachine(oldID: " << tempid << ")" << endl;
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

		if (payload != "null")
		{
			referencingIDs[payload].pop_back();
			referencingIDs[payload].push_back(tempid);
			parent[payload] = tempid;
		}

		if (wildcard != "null")
		{
			referencingIDs[wildcard].pop_back();
			referencingIDs[wildcard].push_back(tempid);
			parent[wildcard] = tempid;
		}

		vecstr storeline = FunctionLineTemp[tempid];

		// stage 3
		int curline = 1;
		bool IsNewChild = false;
		vecstr newstoreline;
		vecstr newchild;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
			if (!IsNewChild)
			{
				if ((newline[i].find("numelements", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
				}

				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].find("wildcardTransitions", 0) == string::npos))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].find("wildcardTransitions", 0) == string::npos))
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
				else if ((storeline[curline].find(newline[i], 0) == string::npos) && (storeline[curline].find("</hkparam>", 0) == string::npos) &&
					(storeline[curline].find("</hkobject>", 0) == string::npos))
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

		if (stateChange)
		{
			string lastSMAdd = boost::regex_replace(string(address), boost::regex("(^.*[(]r[0-9]+[)]=>[^>]+>)[^>]+.*"), string("\\1"));

			if (lastSMAdd != address)
			{
				string parentID = parent[addressID[lastSMAdd]];

				if (parentID.length() == 0)
				{
					cout << "ERROR: hkbStateMachine missing parent ID (ID: " << id << ")" << endl;
					Error = true;
					return;
				}

				if (isSMIDExist[parentID])
				{
					previousSM = StateMachineID[parentID];
					previousSMExist = true;
				}
			}

			StateMachineID[tempid] = shared_from_this();
			isSMIDExist[tempid] = true;
		}

		address = region[tempid];

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachine(newID: " << id << ") with hkbStateMachine(oldID: " << tempid << ") is complete!" << endl;
		}

	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
		elements[id] = children;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachine::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachine(ID: " << id << ") has been initialized!" << endl;
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
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');
					generator.reserve(size);

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							tempgenerator = exchangeID[tempgenerator];
						}

						generator.push_back(tempgenerator);
						parent[tempgenerator] = id;
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

				else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					payload = line.substr(29, line.find("</hkparam>") - 29);

					if (payload != "null")
					{
						if (!exchangeID[payload].empty())
						{
							payload = exchangeID[payload];
						}

						parent[payload] = id;
					}
				}

				else if (line.find("<hkparam name=\"states\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(39, line.length() - 41));
					generator.reserve(children);
					pauseline = true;
				}

				else if (line.find("<hkparam name=\"wildcardTransitions\">", 0) != string::npos)
				{
					wildcard = line.substr(39, line.find("</hkparam>") - 39);

					if (wildcard != "null")
					{
						if (!exchangeID[wildcard].empty())
						{
							wildcard = exchangeID[wildcard];
						}

						parent[wildcard] = id;
					}

					break;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachine Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachine(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachine::NextGenerator(int child)
{
	return "#" + boost::regex_replace(string(generator[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbstatemachine::GetChildren()
{
	return children;
}

string hkbstatemachine::GetPayload()
{
	return "#" + boost::regex_replace(string(payload), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachine::IsPayloadNull()
{
	return payload == "null";
}

string hkbstatemachine::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachine::IsBindingNull()
{
	return variablebindingset == "null";
}

string hkbstatemachine::GetWildcard()
{
	return wildcard;
}

bool hkbstatemachine::IsWildcardNull()
{
	return wildcard == "null";
}

string hkbstatemachine::GetAddress()
{
	return address;
}

bool hkbstatemachine::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbstatemachine>> hkbstatemachineList;
safeStringUMap<shared_ptr<hkbstatemachine>> hkbstatemachineList_E;

void hkbstatemachine::regis(string id, bool isEdited)
{
	isEdited ? hkbstatemachineList_E[id] = shared_from_this() : hkbstatemachineList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbstatemachine::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos || line.find("	#") != string::npos)
		{
			switch (type)
			{
				case 0:
				{
					string output;

					if (readParam("variableBindingSet", line, output))
					{
						variableBindingSet = (isEdited ? hkbvariablebindingsetList_E : hkbvariablebindingsetList)[output];
						++type;
					}

					break;
				}
				case 1:
				{
					if (readParam("userData", line, userData)) ++type;

					break;
				}
				case 2:
				{
					if (readParam("name", line, name)) ++type;

					break;
				}
				case 3:
				{
					if (readParam("id", line, eventToSendWhenStateOrTransitionChanges.id)) ++type;

					break;
				}
				case 4:
				{
					string output;

					if (readParam("payload", line, output))
					{
						eventToSendWhenStateOrTransitionChanges.payload = isEdited ? hkbstringeventpayloadList_E[output] : hkbstringeventpayloadList[output];
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("startStateChooser", line, output))
					{
						startStateChooser = isEdited ? hkbstatechooserList_E[output] : hkbstatechooserList[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("startStateId", line, startStateId)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("returnToPreviousStateEventId", line, returnToPreviousStateEventId)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("randomTransitionEventId", line, randomTransitionEventId)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("transitionToNextHigherStateEventId", line, transitionToNextHigherStateEventId)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("transitionToNextLowerStateEventId", line, transitionToNextLowerStateEventId)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("syncVariableIndex", line, syncVariableIndex)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("wrapAroundStateId", line, wrapAroundStateId)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("maxSimultaneousTransitions", line, maxSimultaneousTransitions)) ++type;

					break;
				}
				case 14:
				{
					string data;

					if (readParam("startStateMode", line, data))
					{
						if (data == "START_STATE_MODE_DEFAULT") startStateMode = START_STATE_MODE_DEFAULT;
						else if (data == "START_STATE_MODE_SYNC") startStateMode = START_STATE_MODE_SYNC;
						else if (data == "START_STATE_MODE_RANDOM") startStateMode = START_STATE_MODE_RANDOM;
						else startStateMode = START_STATE_MODE_CHOOSER;

						++type;
					}

					break;
				}
				case 15:
				{
					string data;

					if (readParam("selfTransitionMode", line, data))
					{
						if (data == "SELF_TRANSITION_MODE_NO_TRANSITION") selfTransitionMode = SELF_TRANSITION_MODE_NO_TRANSITION;
						else if (data == "SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE") selfTransitionMode = SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE;
						else selfTransitionMode = SELF_TRANSITION_MODE_FORCE_TRANSITION_TO_START_STATE;

						++type;
					}

					break;
				}
				case 16:
				{
					usize numelement;

					if (readEleParam("states", line, numelement))
					{
						states.reserve(numelement);
						++type;
					}

					break;
				}
				case 17:
				{
					vecstr reflist;

					if (!getNodeRefList(line, reflist))
					{
						string output;

						if (readParam("wildcardTransitions", line, output))
						{
							wildcardTransitions = isEdited ? hkbstatemachinetransitioninfoarrayList_E[output] : hkbstatemachinetransitioninfoarrayList[output];
							++type;
						}

						break;
					}

					for (auto& ref : reflist)
					{
						states.emplace_back(isEdited ? hkbstatemachinestateinfoList_E[ref] : hkbstatemachinestateinfoList[ref]);
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << statemachine::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstatemachine::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + statemachine::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);
	eventToSendWhenStateOrTransitionChanges.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + statemachine::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			boost::thread_group multithreads;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (eventToSendWhenStateOrTransitionChanges.payload) threadedNextNode(eventToSendWhenStateOrTransitionChanges.payload, filepath, curadd, functionlayer, graphroot);

			if (startStateChooser) threadedNextNode(startStateChooser, filepath, curadd, functionlayer, graphroot);
			
			if (num_thread < boost::thread::hardware_concurrency())
			{
				for (usize i = 0; i < states.size(); ++i)
				{
					multithreads.create_thread(boost::bind(&hkbstatemachine::threadedNextNode, this, states[i], filepath, curadd + to_string(i), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
			}
			else
			{
				for (usize i = 0; i < states.size(); ++i)
				{
					threadedNextNode(states[i], filepath, curadd + to_string(i), 0, graphroot);
				}
			}

			if (wildcardTransitions) threadedNextNode(wildcardTransitions, filepath, curadd, functionlayer, graphroot);

			if (multithreads.size() > 0)
			{
				multithreads.join_all();
				num_thread -= multithreads.size();
			}
		}
		else
		{
			// existed
			if (IsOldFunction(filepath, shared_from_this(), address))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbstatemachineList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbstatemachineList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, compare);
				nextNode(filepath, functionlayer, false, graphroot);
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
				hkbstatemachineList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbstatemachineList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
				RecordID(ID, address, compare);
				nextNode(filepath, functionlayer, true, graphroot);
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

string hkbstatemachine::getClassCode()
{
	return statemachine::key;
}

void hkbstatemachine::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(50);
	usize base = 2;
	hkbstatemachine* ctrpart = static_cast<hkbstatemachine*>(counterpart.get());

	output.push_back(openObject(base, ID, statemachine::classname, statemachine::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "eventToSendWhenStateOrTransitionChanges"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", eventToSendWhenStateOrTransitionChanges.id, ctrpart->eventToSendWhenStateOrTransitionChanges.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", eventToSendWhenStateOrTransitionChanges.payload, ctrpart->eventToSendWhenStateOrTransitionChanges.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("startStateChooser", startStateChooser, ctrpart->startStateChooser, output, storeline, base, false, open, isEdited);
	paramMatch("startStateId", startStateId, ctrpart->startStateId, output, storeline, base, false, open, isEdited);
	paramMatch("returnToPreviousStateEventId", returnToPreviousStateEventId, ctrpart->returnToPreviousStateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("randomTransitionEventId", randomTransitionEventId, ctrpart->randomTransitionEventId, output, storeline, base, false, open, isEdited);
	paramMatch("transitionToNextHigherStateEventId", transitionToNextHigherStateEventId, ctrpart->transitionToNextHigherStateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("transitionToNextLowerStateEventId", transitionToNextLowerStateEventId, ctrpart->transitionToNextLowerStateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("syncVariableIndex", syncVariableIndex, ctrpart->syncVariableIndex, output, storeline, base, false, open, isEdited);
	paramMatch("wrapAroundStateId", wrapAroundStateId, ctrpart->wrapAroundStateId, output, storeline, base, false, open, isEdited);
	paramMatch("maxSimultaneousTransitions", maxSimultaneousTransitions, ctrpart->maxSimultaneousTransitions, output, storeline, base, false, open, isEdited);
	paramMatch("startStateMode", getStartStateMode(), ctrpart->getStartStateMode(), output, storeline, base, false, open, isEdited);
	paramMatch("selfTransitionMode", getSelfTransitionMode(), ctrpart->getSelfTransitionMode(), output, storeline, base, true, open, isEdited);

	usize size = states.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "states", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "states", size));		// 2

	matchScoring(states, ctrpart->states, ID);
	size = states.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (states[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->states[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->states[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->states[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, states[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(states[i], ctrpart->states[i], output, storeline, base, false, open, isEdited);
		}
	}

	if (size > 0)
	{
		if (orisize == 0)
		{
			nemesis::try_open(open, isEdited, output);
			output.push_back(closeParam(base));		// 2
			nemesis::try_close(open, output, storeline);
		}
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(closeParam(base));		// 2
		}
	}
	else nemesis::try_close(open, output, storeline);

	paramMatch("wildcardTransitions", wildcardTransitions, ctrpart->wildcardTransitions, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", statemachine::classname, output, isEdited);
}

void hkbstatemachine::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(50);

	output.push_back(openObject(base, ID, statemachine::classname, statemachine::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(openParam(base, "eventToSendWhenStateOrTransitionChanges"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", eventToSendWhenStateOrTransitionChanges.id));
	output.push_back(autoParam(base, "payload", eventToSendWhenStateOrTransitionChanges.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "startStateChooser", startStateChooser));
	output.push_back(autoParam(base, "startStateId", startStateId));
	output.push_back(autoParam(base, "returnToPreviousStateEventId", returnToPreviousStateEventId));
	output.push_back(autoParam(base, "randomTransitionEventId", randomTransitionEventId));
	output.push_back(autoParam(base, "transitionToNextHigherStateEventId", transitionToNextHigherStateEventId));
	output.push_back(autoParam(base, "transitionToNextLowerStateEventId", transitionToNextLowerStateEventId));
	output.push_back(autoParam(base, "syncVariableIndex", syncVariableIndex));
	output.push_back(autoParam(base, "wrapAroundStateId", wrapAroundStateId));
	output.push_back(autoParam(base, "maxSimultaneousTransitions", maxSimultaneousTransitions));
	output.push_back(autoParam(base, "startStateMode", startStateMode));
	output.push_back(autoParam(base, "selfTransitionMode", selfTransitionMode));

	usize size = states.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "states", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "states", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(states[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "wildcardTransitions", wildcardTransitions));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, statemachine::classname, output, true);
}

void hkbstatemachine::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (eventToSendWhenStateOrTransitionChanges.payload) hkb_parent[eventToSendWhenStateOrTransitionChanges.payload] = shared_from_this();
	if (startStateChooser) hkb_parent[startStateChooser] = shared_from_this();
	if (wildcardTransitions) hkb_parent[wildcardTransitions] = shared_from_this();

	for (auto& state : states)
	{
		hkb_parent[state] = shared_from_this();
	}
}

void hkbstatemachine::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) functionlayer = 0;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (eventToSendWhenStateOrTransitionChanges.payload)
	{
		parentRefresh();
		eventToSendWhenStateOrTransitionChanges.payload->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (startStateChooser)
	{
		parentRefresh();
		startStateChooser->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (isOld)
	{
		usize tempint = 0;

		for (auto& state : states)
		{
			parentRefresh();
			state->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

			if (IsForeign.find(state->ID) == IsForeign.end()) ++tempint;
			else
			{
				auto branch_itr = IsBranchOrigin.find(state->ID);

				if (branch_itr != IsBranchOrigin.end())
				{
					++tempint;
					IsBranchOrigin.erase(branch_itr);
				}
			}
		}
	}
	else
	{
		for (auto& state : states)
		{
			parentRefresh();
			state->connect(filepath, address, functionlayer, true, graphroot);
		}
	}

	if (wildcardTransitions)
	{
		parentRefresh();
		wildcardTransitions->connect(filepath, address, functionlayer, true, graphroot);
	}
}

string hkbstatemachine::getStartStateMode()
{
	switch (startStateMode)
	{
		case START_STATE_MODE_DEFAULT: return "START_STATE_MODE_DEFAULT";
		case START_STATE_MODE_SYNC: return "START_STATE_MODE_SYNC";
		case START_STATE_MODE_RANDOM: return "START_STATE_MODE_RANDOM";
		case START_STATE_MODE_CHOOSER: return "START_STATE_MODE_CHOOSER";
		default: return "START_STATE_MODE_DEFAULT";
	}
}

string hkbstatemachine::getSelfTransitionMode()
{
	switch (selfTransitionMode)
	{
		case SELF_TRANSITION_MODE_NO_TRANSITION: return "SELF_TRANSITION_MODE_NO_TRANSITION";
		case SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE: return "SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE";
		case SELF_TRANSITION_MODE_FORCE_TRANSITION_TO_START_STATE: return "SELF_TRANSITION_MODE_FORCE_TRANSITION_TO_START_STATE";
		default: return "SELF_TRANSITION_MODE_NO_TRANSITION";
	}
}

void hkbstatemachine::matchScoring(vector<shared_ptr<hkbstatemachinestateinfo>>& ori, vector<shared_ptr<hkbstatemachinestateinfo>>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<hkbstatemachinestateinfo>> newOri;
		vector<shared_ptr<hkbstatemachinestateinfo>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<hkbstatemachinestateinfo>());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	map<int, map<int, double>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->ID == edit[j]->ID)
			{
				scorelist[i][j] += 5;
			}

			if (i == j)
			{
				scorelist[i][j] += 2;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 2;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<hkbstatemachinestateinfo>> newOri;
	vector<shared_ptr<hkbstatemachinestateinfo>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<hkbstatemachinestateinfo>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<hkbstatemachinestateinfo>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbstatemachine::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbStateMachineExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	vecstr storeline2;

	if (!generatorLines(storeline1, storeline2, id, "hkbStateMachine"))
	{
		return;
	}

	vecstr oriline;
	vecstr output;
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	size_t size = max(storeline1.size(), storeline2.size());

	for (unsigned int i = 0; i < size; ++i)
	{
		if (storeline1[curline].find("<hkparam name=\"wildcardTransitions\">", 0) == string::npos)
		{
			if (storeline1[curline] != storeline2[i])
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				oriline.push_back(storeline1[curline]);
				output.push_back(storeline2[i]);
			}
			else
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

			++curline;
		}
		else if (storeline2[i].find("<hkparam name=\"wildcardTransitions\">", 0) == string::npos)
		{
			if (open && oriline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), oriline.begin(), oriline.end());
				output.push_back("<!-- CLOSE -->");
				oriline.clear();
				open = false;
			}

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
			if (open)
			{
				if (oriline.size() > 0)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), oriline.begin(), oriline.end());
					oriline.clear();
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			output.push_back("			</hkparam>");

			if (storeline1[curline] != storeline2[i])
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back(storeline2[i]);
				output.push_back("<!-- ORIGINAL -->");
				output.push_back(storeline1[curline]);
				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
			}
			else
			{
				output.push_back(storeline1[curline]);
			}
		}
	}

	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 2 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (output[i].find("<hkparam name=\"eventToSendWhenStateOrTransitionChanges\">", 0) != string::npos)
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
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) ||
						(output[i - 1].find("ORIGINAL", 0) != string::npos))
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
				else if (output[i].find("<hkparam name=\"startStateChooser\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) ||
						(output[i - 1].find("ORIGINAL", 0) != string::npos))
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
			cout << "ERROR: Edit hkbStateMachine Output Not Found (File: " << filename << ")" << endl;
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
