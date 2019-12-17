#include <boost\thread.hpp>
#include "hkbstatemachinestateinfo.h"
#include "highestscore.h"
#include "src\stateid.h"
#include "hkbstatemachine.h"

using namespace std;

namespace statemachinestateinfo
{
	const string key = "p";
	const string classname = "hkbStateMachineStateInfo";
	const string signature = "0xed7f9d0";
}

string hkbstatemachinestateinfo::GetAddress()
{
	return address;
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
