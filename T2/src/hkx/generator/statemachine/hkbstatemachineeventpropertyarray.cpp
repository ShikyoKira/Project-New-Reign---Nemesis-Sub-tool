#include <boost\thread.hpp>
#include "hkbstatemachineeventpropertyarray.h"
#include "highestscore.h"
#include "src\hkx\hkbeventproperty.h"

using namespace std;

namespace statemachineeventpropertyarray
{
	const string key = "o";
	const string classname = "hkbStateMachineEventPropertyArray";
	const string signature = "0xb07b4388";
}

string hkbstatemachineeventpropertyarray::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbstatemachineeventpropertyarray>> hkbstatemachineeventpropertyarrayList;
safeStringUMap<shared_ptr<hkbstatemachineeventpropertyarray>> hkbstatemachineeventpropertyarrayList_E;

void hkbstatemachineeventpropertyarray::regis(string id, bool isEdited)
{
	isEdited ? hkbstatemachineeventpropertyarrayList_E[id] = shared_from_this() : hkbstatemachineeventpropertyarrayList[id] = shared_from_this();
	ID = id;
}

void hkbstatemachineeventpropertyarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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

					if (readEleParam("events", line, numelement))
					{
						events.reserve(numelement);
						++type;
					}

					break;
				}
				case 1:
				{
					int output;

					if (readParam("id", line, output))
					{
						events.push_back(output);
						++type;
					}

					break;
				}
				case 2:
				{
					string output;

					if (readParam("payload", line, output))
					{
						events.back().payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						type = 1;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << statemachineeventpropertyarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstatemachineeventpropertyarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + statemachineeventpropertyarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& event : events)
	{
		event.id.connectEventInfo(ID, graphroot);
	}

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			usize tempint = 0;
			curLock.unlock();

			for (auto& event : events)
			{
				if (event.payload) threadedNextNode(event.payload, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
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
				hkbstatemachineeventpropertyarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbstatemachineeventpropertyarrayList_E[ID] = protect;
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
				hkbstatemachineeventpropertyarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbstatemachineeventpropertyarrayList_E[ID] = protect;
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

string hkbstatemachineeventpropertyarray::getClassCode()
{
	return statemachineeventpropertyarray::key;
}

void hkbstatemachineeventpropertyarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(30);
	output.reserve(30);
	usize base = 2;
	hkbstatemachineeventpropertyarray* ctrpart = static_cast<hkbstatemachineeventpropertyarray*>(counterpart.get());

	output.push_back(openObject(base, ID, statemachineeventpropertyarray::classname, statemachineeventpropertyarray::signature));		// 1

	usize size = events.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "events", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "events", size));		// 2

	matchScoring(events, ctrpart->events, ID);
	size = events.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (events[i].id.nodeID.length() == 0 && events[i].id.id == -1)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "id", ctrpart->events[i].id));
				output.push_back(autoParam(base, "payload", ctrpart->events[i].payload));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->events[i].id.nodeID.length() == 0 && ctrpart->events[i].id.id == -1)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->events[i].id.nodeID.length() == 0 && ctrpart->events[i].id.id == -1)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "id", events[i].id));
				storeline.push_back(autoParam(base, "payload", events[i].payload));
				storeline.push_back(closeObject(base));		// 3
				++i;
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");	// 4 spaces
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("id", events[i].id, ctrpart->events[i].id, output, storeline, base, false, open, isEdited);
			paramMatch("payload", events[i].payload, ctrpart->events[i].payload, output, storeline, base, true, open, isEdited);
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", statemachineeventpropertyarray::classname, output, isEdited);
}

void hkbstatemachineeventpropertyarray::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(30);

	output.push_back(openObject(base, ID, statemachineeventpropertyarray::classname, statemachineeventpropertyarray::signature));		// 1

	usize size = events.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "events", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "events", size));		// 2
	
	for (auto& event : events)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "id", event.id));
		output.push_back(autoParam(base, "payload", event.payload));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, statemachineeventpropertyarray::classname, output, true);
}

void hkbstatemachineeventpropertyarray::parentRefresh()
{
	for (auto& event : events)
	{
		if (event.payload) hkb_parent[event.payload] = shared_from_this();
	}
}

void hkbstatemachineeventpropertyarray::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (isOld)
	{
		usize tempint = 0;

		for (auto& event : events)
		{
			if (event.payload)
			{
				parentRefresh();
				event.payload->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(event.payload->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& event : events)
		{
			if (event.payload)
			{
				parentRefresh();
				event.payload->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}
}

void hkbstatemachineeventpropertyarray::matchScoring(vector<eventproperty>& ori, vector<eventproperty>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<eventproperty> newOri;
		vector<eventproperty> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(eventproperty());
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

			if (ori[i].id == edit[j].id)
			{
				scorelist[i][j] += 4;
			}

			if (matchIDCompare(ori[i].payload, edit[j].payload))
			{
				scorelist[i][j] += 2;
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
	vector<eventproperty> newOri;
	vector<eventproperty> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(eventproperty());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(eventproperty());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbstatemachineeventpropertyarray::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
