#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>
#include "hkbstatemachinetransitioninfoarray.h"
#include "highestscore.h"
#include "src\stateid.h"

using namespace std;

namespace statemachinetransitioninfoarray
{
	string key = "q";
	string classname = "hkbStateMachineTransitionInfoArray";
	string signature = "0xe397b11e";
}

struct interval
{
	EventId enterEventID;
	EventId exitEventID;
	double enterTime = 0;
	double exitTime = 0;
};

struct transitionInfo
{
	bool proxy = true;

	interval trigger;
	interval initiate;
	string transition = "null";
	string condition = "null";
	EventId eventID;
	int stateID = 0;
	string toNestedStateID = "0";
	string fromNestStateID = "0";
	int priority = 0;
	string flags = "0";
};

string GetClass(string id, bool compare); // get class
void transitionInfoProcess(string line, vector<shared_ptr<transitionInfo>>& Transition, shared_ptr<transitionInfo>& curTransition, bool& trigger);
void inputTransition(vecstr& input, shared_ptr<transitionInfo> transition);
bool matchScoring(vector<shared_ptr<transitionInfo>>& ori, vector<shared_ptr<transitionInfo>>& edit, string id);

hkbstatemachinetransitioninfoarray::hkbstatemachinetransitioninfoarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + statemachinetransitioninfoarray::key + to_string(functionlayer) + ">";

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
	vecstr newline;
	string line;

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
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
		vector<shared_ptr<transitionInfo>> newTransition;
		shared_ptr<transitionInfo> curTransition;

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
		vector<shared_ptr<transitionInfo>> oriTransition;

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

		// if (!matchScoring(oriTransition, newTransition, id))
		{
			// return;
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
			vecstr emptyVS;
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
					vecstr* functionlines = &FunctionLineEdited[id];

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

safeStringUMap<shared_ptr<hkbstatemachinetransitioninfoarray>> hkbstatemachinetransitioninfoarrayList;
safeStringUMap<shared_ptr<hkbstatemachinetransitioninfoarray>> hkbstatemachinetransitioninfoarrayList_E;

void hkbstatemachinetransitioninfoarray::regis(string id, bool isEdited)
{
	isEdited ? hkbstatemachinetransitioninfoarrayList_E[id] = shared_from_this() : hkbstatemachinetransitioninfoarrayList[id] = shared_from_this();
	ID = id;
}

void hkbstatemachinetransitioninfoarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos)
		{
			switch (type)
			{
				case 0:
				{
					usize numelement;

					if (readEleParam("transitions", line, numelement))
					{
						transitions.reserve(numelement);
						++type;
					}

					break;
				}
				case 1:
				{
					int output;

					if (readParam("enterEventId", line, output))
					{
						transitions.push_back(output);
						++type;
					}

					break;
				}
				case 2:
				{
					if (readParam("exitEventId", line, transitions.back().triggerInterval.exitEventId)) ++type;

					break;
				}
				case 3:
				{
					if (readParam("enterTime", line, transitions.back().triggerInterval.enterTime)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("exitTime", line, transitions.back().triggerInterval.exitTime)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("enterEventId", line, transitions.back().initiateInterval.enterEventId)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("exitEventId", line, transitions.back().initiateInterval.exitEventId)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("enterTime", line, transitions.back().initiateInterval.enterTime)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("exitTime", line, transitions.back().initiateInterval.exitTime)) ++type;

					break;
				}
				case 9:
				{
					string output;

					if (readParam("transition", line, output))
					{
						transitions.back().transition = (isEdited ? hkbtransitioneffectList_E : hkbtransitioneffectList)[output];
						++type;
					}

					break;
				}
				case 10:
				{
					string output;

					if (readParam("condition", line, output))
					{
						transitions.back().condition = (isEdited ? hkbconditionList_E : hkbconditionList)[output];
						++type;
					}

					break;
				}
				case 11:
				{
					if (readParam("eventId", line, transitions.back().eventId)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("toStateId", line, transitions.back().toStateId)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("fromNestedStateId", line, transitions.back().fromNestedStateId)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("toNestedStateId", line, transitions.back().toNestedStateId)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("priority", line, transitions.back().priority)) ++type;

					break;
				}
				case 16:
				{
					string output;

					if (readParam("flags", line, output))
					{
						vecstr list;
						boost::trim_if(output, boost::is_any_of("\t "));
						boost::split(list, output, boost::is_any_of("|"), boost::token_compress_on);

						for (auto& each : list)
						{
							transitions.back().flags.update(each);
						}

						type = 1;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << statemachinetransitioninfoarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstatemachinetransitioninfoarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + statemachinetransitioninfoarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& transition : transitions)
	{
		transition.triggerInterval.enterEventId.connectEventInfo(ID, graphroot);
		transition.triggerInterval.exitEventId.connectEventInfo(ID, graphroot);

		transition.initiateInterval.enterEventId.connectEventInfo(ID, graphroot);
		transition.initiateInterval.exitEventId.connectEventInfo(ID, graphroot);

		transition.eventId.connectEventInfo(ID, graphroot);
	}

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			usize t = 0;
			usize c = 0;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			for (auto& transition : transitions)
			{
				if (transition.transition) threadedNextNode(transition.transition, filepath, curadd + to_string(t++), functionlayer, graphroot);

				if (transition.condition) threadedNextNode(transition.condition, filepath, curadd + to_string(c++), functionlayer, graphroot);
			}
		}
		else
		{
			auto parent_itr = hkb_parent.find(shared_from_this());

			// existed
			if (addressID.find(address) != addressID.end() && parent_itr != hkb_parent.end() && IsForeign.find(parent_itr->second->ID) == IsForeign.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbstatemachinetransitioninfoarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbstatemachinetransitioninfoarrayList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				de_transitions = transitions;
				vector<transitioninfo> d_transitions = hkbstatemachinetransitioninfoarrayList[ID]->transitions;
				matchScoring(d_transitions, de_transitions, ID);
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
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare, true);

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
				hkbstatemachinetransitioninfoarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbstatemachinetransitioninfoarrayList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				de_transitions = transitions;
				vector<transitioninfo> d_transitions = hkbstatemachinetransitioninfoarrayList[ID]->transitions;
				matchScoring(d_transitions, de_transitions, ID);
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

string hkbstatemachinetransitioninfoarray::getClassCode()
{
	return statemachinetransitioninfoarray::key;
}

void hkbstatemachinetransitioninfoarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(200);
	output.reserve(500);
	usize base = 2;
	hkbstatemachinetransitioninfoarray* ctrpart = static_cast<hkbstatemachinetransitioninfoarray*>(counterpart.get());

	output.push_back(openObject(base, ID, statemachinetransitioninfoarray::classname, statemachinetransitioninfoarray::signature));		// 1

	usize size = transitions.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "transitions", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "transitions", size));		// 2

	matchScoring(transitions, ctrpart->transitions, ID);
	size = transitions.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (transitions[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(openParam(base, "triggerInterval"));		// 4
				output.push_back(openObject(base));		// 5
				output.push_back(autoParam(base, "enterEventId", ctrpart->transitions[i].triggerInterval.enterEventId));
				output.push_back(autoParam(base, "exitEventId", ctrpart->transitions[i].triggerInterval.exitEventId));
				output.push_back(autoParam(base, "enterTime", ctrpart->transitions[i].triggerInterval.enterTime));
				output.push_back(autoParam(base, "exitTime", ctrpart->transitions[i].triggerInterval.exitTime));
				output.push_back(closeObject(base));		// 5
				output.push_back(closeParam(base));		// 4
				output.push_back(openParam(base, "initiateInterval"));		// 4
				output.push_back(openObject(base));		// 5
				output.push_back(autoParam(base, "enterEventId", ctrpart->transitions[i].initiateInterval.enterEventId));
				output.push_back(autoParam(base, "exitEventId", ctrpart->transitions[i].initiateInterval.exitEventId));
				output.push_back(autoParam(base, "enterTime", ctrpart->transitions[i].initiateInterval.enterTime));
				output.push_back(autoParam(base, "exitTime", ctrpart->transitions[i].initiateInterval.exitTime));
				output.push_back(closeObject(base));		// 5
				output.push_back(closeParam(base));		// 4
				output.push_back(autoParam(base, "transition", ctrpart->transitions[i].transition));
				output.push_back(autoParam(base, "condition", ctrpart->transitions[i].condition));
				output.push_back(autoParam(base, "eventId", ctrpart->transitions[i].eventId));
				output.push_back(autoParam(base, "toStateId", ctrpart->transitions[i].toStateId));
				output.push_back(autoParam(base, "fromNestedStateId", ctrpart->transitions[i].fromNestedStateId));
				output.push_back(autoParam(base, "toNestedStateId", ctrpart->transitions[i].toNestedStateId));
				output.push_back(autoParam(base, "priority", ctrpart->transitions[i].priority));
				output.push_back(autoParam(base, "flags", ctrpart->transitions[i].flags.getString()));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->transitions[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->transitions[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(openParam(base, "triggerInterval"));		// 4
				storeline.push_back(openObject(base));		// 5
				storeline.push_back(autoParam(base, "enterEventId", transitions[i].triggerInterval.enterEventId));
				storeline.push_back(autoParam(base, "exitEventId", transitions[i].triggerInterval.exitEventId));
				storeline.push_back(autoParam(base, "enterTime", transitions[i].triggerInterval.enterTime));
				storeline.push_back(autoParam(base, "exitTime", transitions[i].triggerInterval.exitTime));
				storeline.push_back(closeObject(base));		// 5
				storeline.push_back(closeParam(base));		// 4
				storeline.push_back(openParam(base, "initiateInterval"));		// 4
				storeline.push_back(openObject(base));		// 5
				storeline.push_back(autoParam(base, "enterEventId", transitions[i].initiateInterval.enterEventId));
				storeline.push_back(autoParam(base, "exitEventId", transitions[i].initiateInterval.exitEventId));
				storeline.push_back(autoParam(base, "enterTime", transitions[i].initiateInterval.enterTime));
				storeline.push_back(autoParam(base, "exitTime", transitions[i].initiateInterval.exitTime));
				storeline.push_back(closeObject(base));		// 5
				storeline.push_back(closeParam(base));		// 4
				storeline.push_back(autoParam(base, "transition", transitions[i].transition));
				storeline.push_back(autoParam(base, "condition", transitions[i].condition));
				storeline.push_back(autoParam(base, "eventId", transitions[i].eventId));
				storeline.push_back(autoParam(base, "toStateId", transitions[i].toStateId));
				storeline.push_back(autoParam(base, "fromNestedStateId", transitions[i].fromNestedStateId));
				storeline.push_back(autoParam(base, "toNestedStateId", transitions[i].toNestedStateId));
				storeline.push_back(autoParam(base, "priority", transitions[i].priority));
				storeline.push_back(autoParam(base, "flags", transitions[i].flags.getString()));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 26)
				{
					output.push_back("");	// 26 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			output.push_back(openParam(base, "triggerInterval"));		// 4
			output.push_back(openObject(base));		// 5
			paramMatch("enterEventId", transitions[i].triggerInterval.enterEventId, ctrpart->transitions[i].triggerInterval.enterEventId, output, storeline, base, false, open, isEdited);
			paramMatch("exitEventId", transitions[i].triggerInterval.exitEventId, ctrpart->transitions[i].triggerInterval.exitEventId, output, storeline, base, false, open, isEdited);
			paramMatch("enterTime", transitions[i].triggerInterval.enterTime, ctrpart->transitions[i].triggerInterval.enterTime, output, storeline, base, false, open, isEdited);
			paramMatch("exitTime", transitions[i].triggerInterval.exitTime, ctrpart->transitions[i].triggerInterval.exitTime, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 5
			output.push_back(closeParam(base));		// 4
			output.push_back(openParam(base, "initiateInterval"));		// 4
			output.push_back(openObject(base));		// 5
			paramMatch("enterEventId", transitions[i].initiateInterval.enterEventId, ctrpart->transitions[i].initiateInterval.enterEventId, output, storeline, base, false, open, isEdited);
			paramMatch("exitEventId", transitions[i].initiateInterval.exitEventId, ctrpart->transitions[i].initiateInterval.exitEventId, output, storeline, base, false, open, isEdited);
			paramMatch("enterTime", transitions[i].initiateInterval.enterTime, ctrpart->transitions[i].initiateInterval.enterTime, output, storeline, base, false, open, isEdited);
			paramMatch("exitTime", transitions[i].initiateInterval.exitTime, ctrpart->transitions[i].initiateInterval.exitTime, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 5
			output.push_back(closeParam(base));		// 4
			paramMatch("transition", transitions[i].transition, ctrpart->transitions[i].transition, output, storeline, base, false, open, isEdited);
			paramMatch("condition", transitions[i].condition, ctrpart->transitions[i].condition, output, storeline, base, false, open, isEdited);
			paramMatch("eventId", transitions[i].eventId, ctrpart->transitions[i].eventId, output, storeline, base, false, open, isEdited);
			paramMatch("toStateId", transitions[i].toStateId, ctrpart->transitions[i].toStateId, output, storeline, base, false, open, isEdited);
			paramMatch("fromNestedStateId", transitions[i].fromNestedStateId, ctrpart->transitions[i].fromNestedStateId, output, storeline, base, false, open, isEdited);
			paramMatch("toNestedStateId", transitions[i].toNestedStateId, ctrpart->transitions[i].toNestedStateId, output, storeline, base, false, open, isEdited);
			paramMatch("priority", transitions[i].priority, ctrpart->transitions[i].priority, output, storeline, base, false, open, isEdited);
			paramMatch("flags", transitions[i].flags.getString(), ctrpart->transitions[i].flags.getString(), output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 3
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

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", statemachinetransitioninfoarray::classname, output, isEdited);
}

void hkbstatemachinetransitioninfoarray::newNode()
{
	string modID = NodeIDCheck(ID);	
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(500);

	output.push_back(openObject(base, ID, statemachinetransitioninfoarray::classname, statemachinetransitioninfoarray::signature));		// 1

	usize size = transitions.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "transitions", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "transitions", size));		// 2

	for (auto& transition : transitions)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(openParam(base, "triggerInterval"));		// 4
		output.push_back(openObject(base));		// 5
		output.push_back(autoParam(base, "enterEventId", transition.triggerInterval.enterEventId));
		output.push_back(autoParam(base, "exitEventId", transition.triggerInterval.exitEventId));
		output.push_back(autoParam(base, "enterTime", transition.triggerInterval.enterTime));
		output.push_back(autoParam(base, "exitTime", transition.triggerInterval.exitTime));
		output.push_back(closeObject(base));		// 5
		output.push_back(closeParam(base));		// 4
		output.push_back(openParam(base, "initiateInterval"));		// 4
		output.push_back(openObject(base));		// 5
		output.push_back(autoParam(base, "enterEventId", transition.initiateInterval.enterEventId));
		output.push_back(autoParam(base, "exitEventId", transition.initiateInterval.exitEventId));
		output.push_back(autoParam(base, "enterTime", transition.initiateInterval.enterTime));
		output.push_back(autoParam(base, "exitTime", transition.initiateInterval.exitTime));
		output.push_back(closeObject(base));		// 5
		output.push_back(closeParam(base));		// 4
		output.push_back(autoParam(base, "transition", transition.transition));
		output.push_back(autoParam(base, "condition", transition.condition));
		output.push_back(autoParam(base, "eventId", transition.eventId));
		output.push_back(autoParam(base, "toStateId", transition.toStateId));
		output.push_back(autoParam(base, "fromNestedStateId", transition.fromNestedStateId));
		output.push_back(autoParam(base, "toNestedStateId", transition.toNestedStateId));
		output.push_back(autoParam(base, "priority", transition.priority));
		output.push_back(autoParam(base, "flags", transition.flags.getString()));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, statemachinetransitioninfoarray::classname, output, true);
}

void hkbstatemachinetransitioninfoarray::parentRefresh()
{
	for (auto& transition : transitions)
	{
		if (transition.transition) hkb_parent[transition.transition] = shared_from_this();
		if (transition.condition) hkb_parent[transition.condition] = shared_from_this();
	}
}

void hkbstatemachinetransitioninfoarray::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld)
	{
		usize t = 0;
		usize c = 0;
		++functionlayer;

		for (auto& transition : de_transitions)
		{
			if (!transition.proxy)
			{
				if (transition.transition)
				{
					parentRefresh();
					transition.transition->connect(filepath, address + to_string(t), functionlayer, true, graphroot);

					if (IsForeign.find(transition.transition->ID) == IsForeign.end()) ++t;
				}

				if (transition.condition)
				{
					parentRefresh();
					transition.condition->connect(filepath, address + to_string(c), functionlayer, true, graphroot);

					if (IsForeign.find(transition.condition->ID) == IsForeign.end()) ++c;
				}
			}
		}
	}
	else
	{
		for (auto& transition : transitions)
		{
			if (transition.transition)
			{
				parentRefresh();
				transition.transition->connect(filepath, address, functionlayer, true, graphroot);
			}

			if (transition.condition)
			{
				parentRefresh();
				transition.condition->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}
}

void hkbstatemachinetransitioninfoarray::matchScoring(vector<transitioninfo>& ori, vector<transitioninfo>& edit, std::string id)
{
	if (ori.size() == 0)
	{
		vector<transitioninfo> newOri;
		vector<transitioninfo> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(transitioninfo());
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

			if (ori[i].triggerInterval.enterEventId == edit[j].triggerInterval.enterEventId)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].triggerInterval.exitEventId == edit[j].triggerInterval.exitEventId)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].triggerInterval.enterTime == edit[j].triggerInterval.enterTime)
			{
				++scorelist[i][j];
			}

			if (ori[i].triggerInterval.exitTime == edit[j].triggerInterval.exitTime)
			{
				++scorelist[i][j];
			}

			if (ori[i].initiateInterval.enterEventId == edit[j].initiateInterval.enterEventId)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].initiateInterval.exitEventId == edit[j].initiateInterval.exitEventId)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].initiateInterval.enterTime == edit[j].initiateInterval.enterTime)
			{
				++scorelist[i][j];
			}

			if (ori[i].initiateInterval.exitTime == edit[j].initiateInterval.exitTime)
			{
				++scorelist[i][j];
			}

			if (matchIDCompare(ori[i].transition, edit[j].transition))
			{
				scorelist[i][j] += 2;
			}

			if (matchIDCompare(ori[i].condition, edit[j].condition))
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].eventId == edit[j].eventId)
			{
				scorelist[i][j] += 6;
			}

			if (ori[i].toStateId == edit[j].toStateId)
			{
				scorelist[i][j] += 6;
			}

			if (ori[i].fromNestedStateId == edit[j].fromNestedStateId)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].toNestedStateId == edit[j].toNestedStateId)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].priority == edit[j].priority)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].flags == edit[j].flags)
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
	vector<transitioninfo> newOri;
	vector<transitioninfo> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(transitioninfo());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(transitioninfo());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbstatemachinetransitioninfoarray::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbStateMachineTransitionInfoArrayExport(string id)
{
	// stage 1 reading
	vector<shared_ptr<transitionInfo>> oriTransition;
	shared_ptr<transitionInfo> curTransition;
	vecstr test;
	vecstr test2;

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
	vector<shared_ptr<transitionInfo>> newTransition;

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
	vecstr output;
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
		vecstr storeline;
		bool open = false;

		if (newTransition[i]->proxy)
		{
			vecstr instore;
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
				output.push_back("							<hkparam name=\"enterEventId\">" + newTransition[i]->trigger.enterEventID.getID() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"enterEventId\">" + oriTransition[i]->trigger.enterEventID.getID() + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"enterEventId\">" + oriTransition[i]->trigger.enterEventID.getID() + "</hkparam>");
			}

			if (oriTransition[i]->trigger.exitEventID != newTransition[i]->trigger.exitEventID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("							<hkparam name=\"exitEventId\">" + newTransition[i]->trigger.exitEventID.getID() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"exitEventId\">" + oriTransition[i]->trigger.exitEventID.getID() + "</hkparam>");
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

				output.push_back("							<hkparam name=\"exitEventId\">" + oriTransition[i]->trigger.exitEventID.getID() + "</hkparam>");
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
				output.push_back("							<hkparam name=\"enterEventId\">" + newTransition[i]->initiate.enterEventID.getID() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"enterEventId\">" + oriTransition[i]->initiate.enterEventID.getID() + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"enterEventId\">" + oriTransition[i]->initiate.enterEventID.getID() + "</hkparam>");
			}

			if (oriTransition[i]->initiate.exitEventID != newTransition[i]->initiate.exitEventID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("							<hkparam name=\"exitEventId\">" + newTransition[i]->initiate.exitEventID.getID() + "</hkparam>");
				storeline.push_back("							<hkparam name=\"exitEventId\">" + oriTransition[i]->initiate.exitEventID.getID() + "</hkparam>");
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

				output.push_back("							<hkparam name=\"exitEventId\">" + oriTransition[i]->initiate.exitEventID.getID() + "</hkparam>");
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

				output.push_back("					<hkparam name=\"eventId\">" + newTransition[i]->eventID.getID() + "</hkparam>");
				storeline.push_back("					<hkparam name=\"eventId\">" + oriTransition[i]->eventID.getID() + "</hkparam>");
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

				output.push_back("					<hkparam name=\"eventId\">" + oriTransition[i]->eventID.getID() + "</hkparam>");
			}

			if (oriTransition[i]->stateID != newTransition[i]->stateID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"toStateId\">" + to_string(newTransition[i]->stateID) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"toStateId\">" + to_string(oriTransition[i]->stateID) + "</hkparam>");
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

				output.push_back("					<hkparam name=\"toStateId\">" + to_string(oriTransition[i]->stateID) + "</hkparam>");
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
	NemesisReaderFormat(stoi(id.substr(1)), output);

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

void transitionInfoProcess(string line, vector<shared_ptr<transitionInfo>>& Transition, shared_ptr<transitionInfo>& curTransition, bool& trigger)
{
	if (line.find("<hkparam name=\"triggerInterval\">") != string::npos)
	{
		if (curTransition)
		{
			curTransition->proxy = false;
			Transition.push_back(curTransition);
		}

		curTransition = make_shared<transitionInfo>();
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
		curTransition->stateID = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
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

void inputTransition(vecstr& input, shared_ptr<transitionInfo> transition)
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
	input.push_back("							<hkparam name=\"enterEventId\">" + transition->trigger.enterEventID.getID() + "</hkparam>");
	input.push_back("							<hkparam name=\"exitEventId\">" + transition->trigger.exitEventID.getID() + "</hkparam>");
	input.push_back("							<hkparam name=\"enterTime\">" + tenter.str() + "</hkparam>");
	input.push_back("							<hkparam name=\"exitTime\">" + texit.str() + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"initiateInterval\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"enterEventId\">" + transition->initiate.enterEventID.getID() + "</hkparam>");
	input.push_back("							<hkparam name=\"exitEventId\">" + transition->initiate.exitEventID.getID() + "</hkparam>");
	input.push_back("							<hkparam name=\"enterTime\">" + ienter.str() + "</hkparam>");
	input.push_back("							<hkparam name=\"exitTime\">" + iexit.str() + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"transition\">" + transition->transition + "</hkparam>");
	input.push_back("					<hkparam name=\"condition\">" + transition->condition + "</hkparam>");
	input.push_back("					<hkparam name=\"eventId\">" + transition->eventID.getID() + "</hkparam>");
	input.push_back("					<hkparam name=\"toStateId\">" + to_string(transition->stateID) + "</hkparam>");
	input.push_back("					<hkparam name=\"fromNestedStateId\">" + transition->fromNestStateID + "</hkparam>");
	input.push_back("					<hkparam name=\"toNestedStateId\">" + transition->toNestedStateID + "</hkparam>");
	input.push_back("					<hkparam name=\"priority\">" + to_string(transition->priority) + "</hkparam>");
	input.push_back("					<hkparam name=\"flags\">" + transition->flags + "</hkparam>");
	input.push_back("				</hkobject>");
}

bool matchScoring(vector<shared_ptr<transitionInfo>>& ori, vector<shared_ptr<transitionInfo>>& edit, string id)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: hkbStateMachineTransitionInfoArray empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	map<int, map<int, double>> scorelist;

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
	vector<shared_ptr<transitionInfo>> newOri;
	vector<shared_ptr<transitionInfo>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<transitionInfo>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<transitionInfo>());
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
		vecstr storeline1;
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

		vecstr storeline2;

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
		vecstr output;
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

		NemesisReaderFormat(stoi(id.substr(1)), output);

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

bool hkbstatemachinetransitioninfoarray::transitioninfo::transitionflags::operator==(transitionflags& ctrpart)
{
	if (FLAG_USE_TRIGGER_INTERVAL != ctrpart.FLAG_USE_TRIGGER_INTERVAL) return false;
	if (FLAG_USE_INITIATE_INTERVAL != ctrpart.FLAG_USE_INITIATE_INTERVAL) return false;
	if (FLAG_UNINTERRUPTIBLE_WHILE_PLAYING != ctrpart.FLAG_UNINTERRUPTIBLE_WHILE_PLAYING) return false;
	if (FLAG_UNINTERRUPTIBLE_WHILE_DELAYED != ctrpart.FLAG_UNINTERRUPTIBLE_WHILE_DELAYED) return false;
	if (FLAG_DELAY_STATE_CHANGE != ctrpart.FLAG_DELAY_STATE_CHANGE) return false;
	if (FLAG_DISABLED != ctrpart.FLAG_DISABLED) return false;
	if (FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE != ctrpart.FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE) return false;
	if (FLAG_DISALLOW_RANDOM_TRANSITION != ctrpart.FLAG_DISALLOW_RANDOM_TRANSITION) return false;
	if (FLAG_DISABLE_CONDITION != ctrpart.FLAG_DISABLE_CONDITION) return false;
	if (FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE != ctrpart.FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE) return false;
	if (FLAG_IS_GLOBAL_WILDCARD != ctrpart.FLAG_IS_GLOBAL_WILDCARD) return false;
	if (FLAG_IS_LOCAL_WILDCARD != ctrpart.FLAG_IS_LOCAL_WILDCARD) return false;
	if (FLAG_FROM_NESTED_STATE_ID_IS_VALID != ctrpart.FLAG_FROM_NESTED_STATE_ID_IS_VALID) return false;
	if (FLAG_TO_NESTED_STATE_ID_IS_VALID != ctrpart.FLAG_TO_NESTED_STATE_ID_IS_VALID) return false;
	if (FLAG_ABUT_AT_END_OF_FROM_GENERATOR != ctrpart.FLAG_ABUT_AT_END_OF_FROM_GENERATOR) return false;

	return true;
}

bool hkbstatemachinetransitioninfoarray::transitioninfo::transitionflags::operator!=(transitionflags& ctrpart)
{
	if (FLAG_USE_TRIGGER_INTERVAL != ctrpart.FLAG_USE_TRIGGER_INTERVAL) return true;
	if (FLAG_USE_INITIATE_INTERVAL != ctrpart.FLAG_USE_INITIATE_INTERVAL) return true;
	if (FLAG_UNINTERRUPTIBLE_WHILE_PLAYING != ctrpart.FLAG_UNINTERRUPTIBLE_WHILE_PLAYING) return true;
	if (FLAG_UNINTERRUPTIBLE_WHILE_DELAYED != ctrpart.FLAG_UNINTERRUPTIBLE_WHILE_DELAYED) return true;
	if (FLAG_DELAY_STATE_CHANGE != ctrpart.FLAG_DELAY_STATE_CHANGE) return true;
	if (FLAG_DISABLED != ctrpart.FLAG_DISABLED) return true;
	if (FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE != ctrpart.FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE) return true;
	if (FLAG_DISALLOW_RANDOM_TRANSITION != ctrpart.FLAG_DISALLOW_RANDOM_TRANSITION) return true;
	if (FLAG_DISABLE_CONDITION != ctrpart.FLAG_DISABLE_CONDITION) return true;
	if (FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE != ctrpart.FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE) return true;
	if (FLAG_IS_GLOBAL_WILDCARD != ctrpart.FLAG_IS_GLOBAL_WILDCARD) return true;
	if (FLAG_IS_LOCAL_WILDCARD != ctrpart.FLAG_IS_LOCAL_WILDCARD) return true;
	if (FLAG_FROM_NESTED_STATE_ID_IS_VALID != ctrpart.FLAG_FROM_NESTED_STATE_ID_IS_VALID) return true;
	if (FLAG_TO_NESTED_STATE_ID_IS_VALID != ctrpart.FLAG_TO_NESTED_STATE_ID_IS_VALID) return true;
	if (FLAG_ABUT_AT_END_OF_FROM_GENERATOR != ctrpart.FLAG_ABUT_AT_END_OF_FROM_GENERATOR) return true;

	return false;
}

string hkbstatemachinetransitioninfoarray::transitioninfo::transitionflags::getString()
{
	string line;

	if (FLAG_ABUT_AT_END_OF_FROM_GENERATOR) line.append("FLAG_ABUT_AT_END_OF_FROM_GENERATOR|");
	if (FLAG_TO_NESTED_STATE_ID_IS_VALID) line.append("FLAG_TO_NESTED_STATE_ID_IS_VALID|");
	if (FLAG_FROM_NESTED_STATE_ID_IS_VALID) line.append("FLAG_FROM_NESTED_STATE_ID_IS_VALID|");
	if (FLAG_IS_LOCAL_WILDCARD) line.append("FLAG_IS_LOCAL_WILDCARD|");
	if (FLAG_IS_GLOBAL_WILDCARD) line.append("FLAG_IS_GLOBAL_WILDCARD|");
	if (FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE) line.append("FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE|");
	if (FLAG_DISABLE_CONDITION) line.append("FLAG_DISABLE_CONDITION|");
	if (FLAG_DISALLOW_RANDOM_TRANSITION) line.append("FLAG_DISALLOW_RANDOM_TRANSITION|");
	if (FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE) line.append("FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE|");
	if (FLAG_DISABLED) line.append("FLAG_DISABLED|");
	if (FLAG_DELAY_STATE_CHANGE) line.append("FLAG_DELAY_STATE_CHANGE|");
	if (FLAG_UNINTERRUPTIBLE_WHILE_DELAYED) line.append("FLAG_UNINTERRUPTIBLE_WHILE_DELAYED|");
	if (FLAG_UNINTERRUPTIBLE_WHILE_PLAYING) line.append("FLAG_UNINTERRUPTIBLE_WHILE_PLAYING|");
	if (FLAG_USE_INITIATE_INTERVAL) line.append("FLAG_USE_INITIATE_INTERVAL|");
	if (FLAG_USE_TRIGGER_INTERVAL) line.append("FLAG_USE_TRIGGER_INTERVAL|");

	if (line.length() == 0) return "0";

	line.pop_back();
	return line;
}

void hkbstatemachinetransitioninfoarray::transitioninfo::transitionflags::update(std::string flag)
{
	if (flag == "FLAG_USE_TRIGGER_INTERVAL") FLAG_USE_TRIGGER_INTERVAL = true;
	else if (flag == "FLAG_USE_INITIATE_INTERVAL") FLAG_USE_INITIATE_INTERVAL = true;
	else if (flag == "FLAG_UNINTERRUPTIBLE_WHILE_PLAYING") FLAG_UNINTERRUPTIBLE_WHILE_PLAYING = true;
	else if (flag == "FLAG_UNINTERRUPTIBLE_WHILE_DELAYED") FLAG_UNINTERRUPTIBLE_WHILE_DELAYED = true;
	else if (flag == "FLAG_DELAY_STATE_CHANGE") FLAG_DELAY_STATE_CHANGE = true;
	else if (flag == "FLAG_DISABLED") FLAG_DISABLED = true;
	else if (flag == "FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE") FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE = true;
	else if (flag == "FLAG_DISALLOW_RANDOM_TRANSITION") FLAG_DISALLOW_RANDOM_TRANSITION = true;
	else if (flag == "FLAG_DISABLE_CONDITION") FLAG_DISABLE_CONDITION = true;
	else if (flag == "FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE") FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE = true;
	else if (flag == "FLAG_IS_GLOBAL_WILDCARD") FLAG_IS_GLOBAL_WILDCARD = true;
	else if (flag == "FLAG_IS_LOCAL_WILDCARD") FLAG_IS_LOCAL_WILDCARD = true;
	else if (flag == "FLAG_FROM_NESTED_STATE_ID_IS_VALID") FLAG_FROM_NESTED_STATE_ID_IS_VALID = true;
	else if (flag == "FLAG_TO_NESTED_STATE_ID_IS_VALID") FLAG_TO_NESTED_STATE_ID_IS_VALID = true;
	else if (flag == "FLAG_ABUT_AT_END_OF_FROM_GENERATOR") FLAG_ABUT_AT_END_OF_FROM_GENERATOR = true;
}
