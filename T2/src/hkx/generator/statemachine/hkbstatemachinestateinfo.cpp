#include <boost\thread.hpp>
#include "hkbstatemachinestateinfo.h"
#include "highestscore.h"
#include "src\stateid.h"
#include "hkbstatemachine.h"

using namespace std;

namespace statemachinestateinfo
{
	string key = "p";
	string classname = "hkbStateMachineStateInfo";
	string signature = "0xed7f9d0";
}

string GetClass(string id, bool compare); // get class
void statIDCheck(string id, string stateID, vecstr& newline);
void stateIDTransitionReplacement(string id, string oldStateID, string stateID, string lastStateID = "", bool toNested = false, bool fromNested = false);

hkbstatemachinestateinfo::hkbstatemachinestateinfo(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + statemachinestateinfo::key + to_string(functionlayer) + ">";

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

void hkbstatemachinestateinfo::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineStateInfo(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("enterNotifyEvents", 0) != string::npos)
			{
				enternotifyevent = line.substr(37, line.find("</hkparam>") - 37);

				if (enternotifyevent != "null")
				{
					referencingIDs[enternotifyevent].push_back(id);
				}
			}
			else if (line.find("exitNotifyEvents", 0) != string::npos)
			{
				exitnotifyevent = line.substr(36, line.find("</hkparam>") - 36);

				if (exitnotifyevent != "null")
				{
					referencingIDs[exitnotifyevent].push_back(id);
				}
			}
			else if (line.find("transitions", 0) != string::npos)
			{
				s_transitions = line.substr(31, line.find("</hkparam>") - 31);

				if (s_transitions != "null")
				{
					referencingIDs[s_transitions].push_back(id);
				}
			}
			else if (line.find("generator", 0) != string::npos)
			{
				s_generator = line.substr(29, line.find("</hkparam>") - 29);
				referencingIDs[s_generator].push_back(id);
			}
			else if (stateChange && line.find("stateId", 0) != string::npos)
			{
				unsigned int currentState = static_cast<unsigned int>(stoi(line.substr(27, line.find("</hkparam>", 27) - 27)));

				if (isStateIDExist[parent[id]])
				{
					StateID[parent[id]].InstallBase(id, currentState);
				}
				else
				{
					cout << "ERROR: hkbStateMachineStateInfo BUG FOUND! Missing StateID (node ID: " << parent[id] << ", state ID: " << to_string(currentState) << ")" << endl;
					Error = true;
					return;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineStateInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineStateInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinestateinfo::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStateMachineStateInfo(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	string stateID;

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
			else if (line.find("enterNotifyEvents", 0) != string::npos)
			{
				enternotifyevent = line.substr(37, line.find("</hkparam>") - 37);

				if (enternotifyevent != "null")
				{
					if (!exchangeID[enternotifyevent].empty())
					{
						int tempint = line.find(enternotifyevent);
						enternotifyevent = exchangeID[enternotifyevent];
						line.replace(tempint, line.find("</hkparam>") - tempint, enternotifyevent);
					}

					parent[enternotifyevent] = id;
					referencingIDs[enternotifyevent].push_back(id);
				}
			}
			else if (line.find("exitNotifyEvents", 0) != string::npos)
			{
				exitnotifyevent = line.substr(36, line.find("</hkparam>") - 36);

				if (exitnotifyevent != "null")
				{
					if (!exchangeID[exitnotifyevent].empty())
					{
						int tempint = line.find(exitnotifyevent);
						exitnotifyevent = exchangeID[exitnotifyevent];
						line.replace(tempint, line.find("</hkparam>") - tempint, exitnotifyevent);
					}

					parent[exitnotifyevent] = id;
					referencingIDs[exitnotifyevent].push_back(id);
				}
			}
			else if (line.find("transitions", 0) != string::npos)
			{
				s_transitions = line.substr(31, line.find("</hkparam>") - 31);

				if (s_transitions != "null")
				{
					if (!exchangeID[s_transitions].empty())
					{
						int tempint = line.find(s_transitions);
						s_transitions = exchangeID[s_transitions];
						line.replace(tempint, line.find("</hkparam>") - tempint, s_transitions);
					}

					parent[s_transitions] = id;
					referencingIDs[s_transitions].push_back(id);
				}
			}
			else if (line.find("generator", 0) != string::npos)
			{
				s_generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[s_generator].empty())
				{
					int tempint = line.find(s_generator);
					s_generator = exchangeID[s_generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, s_generator);
				}

				parent[s_generator] = id;
				referencingIDs[s_generator].push_back(id);
			}
			else if (stateChange && line.find("stateId", 0) != string::npos)
			{
				stateID = line.substr(27, line.find("</hkparam>", 27) - 27);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbStateMachineStateInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineStateInfo(newID: " << id << ") with hkbStateMachineStateInfo(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (enternotifyevent != "null")
		{
			referencingIDs[enternotifyevent].pop_back();
			referencingIDs[enternotifyevent].push_back(tempid);
			parent[enternotifyevent] = tempid;
		}

		if (exitnotifyevent != "null")
		{
			referencingIDs[exitnotifyevent].pop_back();
			referencingIDs[exitnotifyevent].push_back(tempid);
			parent[exitnotifyevent] = tempid;
		}

		if (s_transitions != "null")
		{
			referencingIDs[s_transitions].pop_back();
			referencingIDs[s_transitions].push_back(tempid);
			parent[s_transitions] = tempid;
		}
		
		referencingIDs[s_generator].pop_back();
		referencingIDs[s_generator].push_back(tempid);
		parent[s_generator] = tempid;

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStateMachineStateInfo(newID: " << id << ") with hkbStateMachineStateInfo(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		if (stateChange)
		{
			statIDCheck(id, stateID, newline);
		}

		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStateMachineStateInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstatemachinestateinfo::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStateMachineStateInfo(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("enterNotifyEvents", 0) != string::npos)
			{
				enternotifyevent = line.substr(37, line.find("</hkparam>") - 37);

				if (enternotifyevent != "null")
				{
					if (!exchangeID[enternotifyevent].empty())
					{
						enternotifyevent = exchangeID[enternotifyevent];
					}

					parent[enternotifyevent] = id;
				}
			}
			else if (line.find("exitNotifyEvents", 0) != string::npos)
			{
				exitnotifyevent = line.substr(36, line.find("</hkparam>") - 36);

				if (exitnotifyevent != "null")
				{
					if (!exchangeID[exitnotifyevent].empty())
					{
						exitnotifyevent = exchangeID[exitnotifyevent];
					}

					parent[exitnotifyevent] = id;
				}
			}
			else if (line.find("transitions", 0) != string::npos)
			{
				s_transitions = line.substr(31, line.find("</hkparam>") - 31);

				if (s_transitions != "null")
				{
					if (!exchangeID[s_transitions].empty())
					{
						s_transitions = exchangeID[s_transitions];
					}

					parent[s_transitions] = id;
				}
			}
			else if (line.find("generator", 0) != string::npos)
			{
				s_generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[s_generator].empty())
				{
					s_generator = exchangeID[s_generator];
				}

				parent[s_generator] = id;
			}
			else if (stateChange && line.find("stateId", 0) != string::npos)
			{
				string stateID = line.substr(27, line.find("</hkparam>", 27) - 27);

				if (!IsForeign[parent[id]] && stateID.find("stateID[") == string::npos)
				{
					string parentID = parent[id];

					if (!exchangeID[parentID].empty())
					{
						parentID = exchangeID[parentID];
					}

					if (isStateIDExist[parentID])
					{
						if (StateID[parentID].IsNewState(static_cast<unsigned int>(stoi(stateID))))
						{
							size_t oldStateIDSize = stateID.length();
							stateID = "$stateID[" + parentID + "][" + StateID[parentID].GetBaseStr() + "][" + modcode + "][" + StateID[parentID].GetStateID(stateID) + "]$";
							FunctionLineNew[id][i].replace(27, oldStateIDSize, stateID);
						}
					}
					else
					{
						cout << "ERROR: Dummy hkbStateMachineStateInfo BUG FOUND! Missing StateID (parent ID: " << parentID << ", node ID: " << id << ", state ID: " << stateID << ")" << endl;
						Error = true;
						return;
					}
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbStateMachineStateInfo Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStateMachineStateInfo(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstatemachinestateinfo::NextGenerator()
{
	return "#" + boost::regex_replace(string(s_generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbstatemachinestateinfo::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbstatemachinestateinfo::IsBindingNull()
{
	return variablebindingset == "null";
}

string hkbstatemachinestateinfo::GetTransitions()
{
	return s_transitions;
}

bool hkbstatemachinestateinfo::IsTransitionsNull()
{
	return s_transitions == "null";
}

string hkbstatemachinestateinfo::GetEnterNotifyEvent()
{
	return enternotifyevent;
}

bool hkbstatemachinestateinfo::IsEnterNotifyEventNull()
{
	return enternotifyevent == "null";
}

string hkbstatemachinestateinfo::GetExitNotifyEvent()
{
	return exitnotifyevent;
}

bool hkbstatemachinestateinfo::IsExitNotifyEventNull()
{
	return exitnotifyevent == "null";
}

string hkbstatemachinestateinfo::GetAddress()
{
	return address;
}

bool hkbstatemachinestateinfo::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbstatemachinestateinfo>> hkbstatemachinestateinfoList;
safeStringUMap<shared_ptr<hkbstatemachinestateinfo>> hkbstatemachinestateinfoList_E;

void hkbstatemachinestateinfo::regis(string id, bool isEdited)
{
	isEdited ? hkbstatemachinestateinfoList_E[id] = shared_from_this() : hkbstatemachinestateinfoList[id] = shared_from_this();
	ID = id;
}

void hkbstatemachinestateinfo::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					usize numelement;

					if (readEleParam("listeners", line, numelement))
					{
						listeners.reserve(numelement);
						++type;
					}

					break;
				}
				case 2:
				{
					vecstr reflist;

					if (!getNodeRefList(line, reflist))
					{
						string output;

						if (readParam("enterNotifyEvents", line, output))
						{
							enterNotifyEvents = (isEdited ? hkbstatemachineeventpropertyarrayList_E : hkbstatemachineeventpropertyarrayList)[output];
							++type;
						}

						break;
					}

					for (auto& ref : reflist)
					{
						listeners.emplace_back((isEdited ? hkbstatelistenerList_E : hkbstatelistenerList)[ref]);
					}

					break;
				}
				case 3:
				{
					string output;

					if (readParam("exitNotifyEvents", line, output))
					{
						exitNotifyEvents = (isEdited ? hkbstatemachineeventpropertyarrayList_E : hkbstatemachineeventpropertyarrayList)[output];
						++type;
					}

					break;
				}
				case 4:
				{
					string output;

					if (readParam("transitions", line, output))
					{
						transitions = (isEdited ? hkbstatemachinetransitioninfoarrayList_E : hkbstatemachinetransitioninfoarrayList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("generator", line, output))
					{
						generator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("name", line, name)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("stateId", line, stateId)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("probability", line, probability)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("enable", line, enable)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << statemachinestateinfo::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstatemachinestateinfo::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + statemachinestateinfo::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			for (usize i = 0; i < listeners.size(); ++i)
			{
				threadedNextNode(listeners[i], filepath, curadd + to_string(i), functionlayer, graphroot);
			}

			if (enterNotifyEvents) threadedNextNode(enterNotifyEvents, filepath, curadd + "0", functionlayer, graphroot);

			if (exitNotifyEvents) threadedNextNode(exitNotifyEvents, filepath, curadd + "1", functionlayer, graphroot);

			if (transitions) threadedNextNode(transitions, filepath, curadd, functionlayer, graphroot);

			if (generator) threadedNextNode(generator, filepath, curadd, functionlayer, graphroot);
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
				hkbstatemachinestateinfoList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbstatemachinestateinfoList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, true);
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
				hkbstatemachinestateinfoList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbstatemachinestateinfoList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
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

string hkbstatemachinestateinfo::getClassCode()
{
	return statemachinestateinfo::key;
}

void hkbstatemachinestateinfo::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(12);
	output.reserve(12);
	usize base = 2;
	hkbstatemachinestateinfo* ctrpart = static_cast<hkbstatemachinestateinfo*>(counterpart.get());

	output.push_back(openObject(base, ID, statemachinestateinfo::classname, statemachinestateinfo::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, true, open, isEdited);

	usize size = listeners.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "listeners", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "listeners", size));		// 2

	matchScoring(listeners, ctrpart->listeners, ID);
	size = listeners.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (listeners[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->listeners[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->listeners[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->listeners[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, listeners[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(listeners[i], ctrpart->listeners[i], output, storeline, base, false, open, isEdited);
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

	paramMatch("enterNotifyEvents", enterNotifyEvents, ctrpart->enterNotifyEvents, output, storeline, base, false, open, isEdited);
	paramMatch("exitNotifyEvents", exitNotifyEvents, ctrpart->exitNotifyEvents, output, storeline, base, false, open, isEdited);
	paramMatch("transitions", transitions, ctrpart->transitions, output, storeline, base, false, open, isEdited);
	paramMatch("generator", generator, ctrpart->generator, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("stateId", stateId, ctrpart->stateId, output, storeline, base, false, open, isEdited);
	paramMatch("probability", probability, ctrpart->probability, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", statemachinestateinfo::classname, output, isEdited);
}

void hkbstatemachinestateinfo::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(12);

	output.push_back(openObject(base, ID, statemachinestateinfo::classname, statemachinestateinfo::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));

	usize size = listeners.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "listeners", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "listeners", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(listeners[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "enterNotifyEvents", enterNotifyEvents));
	output.push_back(autoParam(base, "exitNotifyEvents", exitNotifyEvents));
	output.push_back(autoParam(base, "transitions", transitions));
	output.push_back(autoParam(base, "generator", generator));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "stateId", stateId));
	output.push_back(autoParam(base, "probability", probability));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, statemachinestateinfo::classname, output, true);
}

void hkbstatemachinestateinfo::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (enterNotifyEvents) hkb_parent[enterNotifyEvents] = shared_from_this();
	if (exitNotifyEvents) hkb_parent[exitNotifyEvents] = shared_from_this();
	if (transitions) hkb_parent[transitions] = shared_from_this();
	if (generator) hkb_parent[generator] = shared_from_this();

	for (auto& listener : listeners)
	{
		hkb_parent[listener] = shared_from_this();
	}
}

void hkbstatemachinestateinfo::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (isOld)
	{
		usize tempint = 0;

		for (auto listener : listeners)
		{
			parentRefresh();
			listener->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

			if (IsForeign.find(listener->ID) == IsForeign.end()) ++tempint;
		}
	}
	else
	{
		for (auto listener : listeners)
		{
			parentRefresh();
			listener->connect(filepath, address, functionlayer, true, graphroot);
		}
	}

	if (enterNotifyEvents)
	{
		parentRefresh();
		enterNotifyEvents->connect(filepath, (isOld ? address + "0" : address), functionlayer, true, graphroot);
	}

	if (exitNotifyEvents)
	{
		parentRefresh();
		exitNotifyEvents->connect(filepath, (isOld ? address + "1" : address), functionlayer, true, graphroot);
	}

	if (transitions)
	{
		parentRefresh();
		transitions->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (generator)
	{
		parentRefresh();
		generator->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbstatemachinestateinfo::matchScoring(vector<shared_ptr<hkbstatelistener>>& ori, vector<shared_ptr<hkbstatelistener>>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<hkbstatelistener>> newOri;
		vector<shared_ptr<hkbstatelistener>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<hkbstatelistener>());
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
	vector<shared_ptr<hkbstatelistener>> newOri;
	vector<shared_ptr<hkbstatelistener>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<hkbstatelistener>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<hkbstatelistener>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbstatemachinestateinfo::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbStateMachineStateInfoExport(string id)
{
	//stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	//stage 2 reading and identifying edits
	vecstr storeline2;
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

			if (line[line.size() - 1] == '\n')
			{
				line.pop_back();
			}

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
		cout << "ERROR: Edit hkbStateMachineStateInfo Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(stoi(id.substr(1)), storeline2);

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
			cout << "ERROR: Edit hkbStateMachineStateInfo Output Not Found (File: " << filename << ")" << endl;
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

void statIDCheck(string id, string stateID, vecstr& newline)
{
	string parentID = parent[id];

	if (!IsForeign[parentID])
	{
		if (isStateIDExist[parentID])
		{
			if (StateID[parentID].IsNewState(static_cast<unsigned int>(stoi(stateID))))
			{
				string oldStateID = stateID;
				stateID = "$stateID[" + parentID + "][" + StateID[parentID].GetBaseStr() + "][" + modcode + "][" + StateID[parentID].GetStateID(stateID) + "]$";
				newline[8].replace(27, oldStateID.length(), stateID);

				if (isSMIDExist[parentID])
				{
					shared_ptr<hkbstatemachine> lastSM = StateMachineID[parentID];

					if (!lastSM->IsWildcardNull())
					{
						string wildcard = lastSM->GetWildcard();

						if (!exchangeID[wildcard].empty())
						{
							wildcard = exchangeID[wildcard];
						}

						stateIDTransitionReplacement(wildcard, oldStateID, stateID);
					}

					int children = lastSM->GetChildren();

					for (int i = 0; i < children; ++i)
					{
						string generator = lastSM->NextGenerator(i);
						string transition;

						if (!exchangeID[generator].empty())
						{
							generator = exchangeID[generator];
						}

						if (IsExist[generator])
						{
							transition = boost::regex_replace(string(FunctionLineNew[generator][5]), boost::regex(".*<hkparam name=\"transitions\">(.*)</hkparam>.*"), string("\\1"));
						}
						else
						{
							transition = boost::regex_replace(string(FunctionLineEdited[generator][5]), boost::regex(".*<hkparam name=\"transitions\">(.*)</hkparam>.*"), string("\\1"));
						}

						if (transition != "null")
						{
							stateIDTransitionReplacement(transition, oldStateID, stateID);
						}
					}

					if (lastSM->previousSMExist)
					{
						parentID = parent[parentID];

						while (parentID.length() > 0 && GetClass(parentID, true) != "hkbStateMachineStateInfo")
						{
							parentID = parent[parentID];
						}

						string transition = boost::regex_replace(string(FunctionLineNew[parentID][5]), boost::regex(".*<hkparam name=\"transitions\">(.*)</hkparam>.*"), string("\\1"));
						string lastStateID = boost::regex_replace(string(FunctionLineNew[parentID][8]), boost::regex(".*<hkparam name=\"stateId\">([0-9]+)</hkparam>.*"), string("\\1"));
						lastSM = lastSM->previousSM;

						if (transition != "null")
						{
							stateIDTransitionReplacement(transition, oldStateID, stateID, lastStateID, false, true);
						}

						// get last last SM wildcard 
						if (!lastSM->IsWildcardNull())
						{
							string wildcard = lastSM->GetWildcard();

							if (!exchangeID[wildcard].empty())
							{
								wildcard = exchangeID[wildcard];
							}

							stateIDTransitionReplacement(wildcard, oldStateID, stateID, lastStateID, true);
						}

						children = lastSM->GetChildren();

						for (int i = 0; i < children; ++i)
						{
							string generator = lastSM->NextGenerator(i);
							string transition;

							if (!exchangeID[generator].empty())
							{
								generator = exchangeID[generator];
							}

							if (IsExist[generator])
							{
								transition = boost::regex_replace(string(FunctionLineNew[generator][5]), boost::regex(".*<hkparam name=\"transitions\">(.*)</hkparam>.*"), string("\\1"));
							}
							else
							{
								transition = boost::regex_replace(string(FunctionLineEdited[generator][5]), boost::regex(".*<hkparam name=\"transitions\">(.*)</hkparam>.*"), string("\\1"));
							}

							if (transition != "null")
							{
								stateIDTransitionReplacement(transition, oldStateID, stateID, lastStateID, true);
							}
						}
					}
				}
			}
		}
		else
		{
			cout << "ERROR: hkbStateMachineStateInfo BUG FOUND! Missing StateID (node ID: " << parentID << ", state ID: " << stateID << ")" << endl;
			Error = true;
			return;
		}
	}
}

void stateIDTransitionReplacement(string id, string oldStateID, string stateID, string lastStateID, bool toNested, bool fromNested)
{
	vecstr* functionlines;

	if (IsExist[id])
	{
		functionlines = &FunctionLineNew[id];
	}
	else
	{
		functionlines = &FunctionLineEdited[id];
	}

	if (toNested)
	{
		for (unsigned int i = 0; i < functionlines->size(); ++i)
		{
			string& line = (*functionlines)[i];

			if (line.find("<hkparam name=\"toNestedStateId\">") != string::npos)
			{
				if (lastStateID == boost::regex_replace(string((*functionlines)[i - 2]), boost::regex(".*<hkparam name=\"toStateId\">([0-9]+)</hkparam>.*"), string("\\1")))
				{
					if ((*functionlines)[i + 2].find("FLAG_TO_NESTED_STATE_ID_IS_VALID") != string::npos || oldStateID != "0")
					{
						if (oldStateID == boost::regex_replace(string(line), boost::regex(".*<hkparam name=\"toNestedStateId\">([0-9]+)</hkparam>.*"), string("\\1")))
						{
							line.replace(37, oldStateID.length(), stateID);
						}
					}
				}
			}
		}
	}
	else if (fromNested)
	{
		for (unsigned int i = 0; i < functionlines->size(); ++i)
		{
			string& line = (*functionlines)[i];

			if (line.find("<hkparam name=\"fromNestedStateId\">") != string::npos)
			{
				if ((*functionlines)[i + 2].find("FLAG_FROM_NESTED_STATE_ID_IS_VALID") != string::npos || oldStateID != "0")
				{
					if (oldStateID == boost::regex_replace(string(line), boost::regex(".*<hkparam name=\"fromNestedStateId\">([0-9]+)</hkparam>.*"), string("\\1")))
					{
						line.replace(39, oldStateID.length(), stateID);
					}
				}
			}
		}
	}
	else
	{
		for (auto& line : *functionlines)
		{
			if (line.find("<hkparam name=\"toStateId\">") != string::npos)
			{
				if (oldStateID == boost::regex_replace(string(line), boost::regex(".*<hkparam name=\"toStateId\">([0-9]+)</hkparam>.*"), string("\\1")))
				{
					line.replace(31, oldStateID.length(), stateID);
				}
			}
		}
	}
}
