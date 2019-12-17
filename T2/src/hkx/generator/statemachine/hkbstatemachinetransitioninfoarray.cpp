#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>

#include "highestscore.h"
#include "hkbstatemachinetransitioninfoarray.h"

#include "src\stateid.h"

using namespace std;

namespace statemachinetransitioninfoarray
{
	const string key = "q";
	const string classname = "hkbStateMachineTransitionInfoArray";
	const string signature = "0xe397b11e";
}

string hkbstatemachinetransitioninfoarray::GetAddress()
{
	return address;
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
