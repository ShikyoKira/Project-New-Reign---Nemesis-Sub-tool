#include <boost\thread.hpp>
#include "hkbstatemachineeventpropertyarray.h"
#include "highestscore.h"
#include "src\hkx\hkbeventproperty.h"

using namespace std;

namespace statemachineeventpropertyarray
{
	string key = "o";
	string classname = "hkbStateMachineEventPropertyArray";
	string signature = "0xb07b4388";
}

struct events
{
	bool proxy = true;

	unsigned int id;
	shared_ptr<hkbstringeventpayload> payload;
};

void inputEvent(vecstr& input, shared_ptr<events> transition);
void eventInfoProcess(string line, vector<shared_ptr<events>>& binds, shared_ptr<events>& curBind);
bool matchScoring(vector<shared_ptr<events>>& ori, vector<shared_ptr<events>>& edit, string id);

hkbstatemachineeventpropertyarray::hkbstatemachineeventpropertyarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + statemachineeventpropertyarray::key + to_string(functionlayer) + ">";

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
	vecstr newline;
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
			addressChange.erase(address);
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
	return payload[child] == "null";
}

string hkbstatemachineeventpropertyarray::GetAddress()
{
	return address;
}

bool hkbstatemachineeventpropertyarray::IsNegate()
{
	return IsNegated;
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
		if (events[i].id.graphroot == nullptr)
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
		else if (ctrpart->events[i].id.graphroot == nullptr)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->events[i].id.graphroot == nullptr)
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

	for (unsigned int i = 0; i < oriEvents.size(); i++)
	{
		vecstr storeline;
		bool open = false;

		if (newEvents[i]->proxy)
		{
			vecstr instore;
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

				output.push_back("					<hkparam name=\"payload\">" + newEvents[i]->payload->ID + "</hkparam>");
				storeline.push_back("					<hkparam name=\"payload\">" + oriEvents[i]->payload->ID + "</hkparam>");
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

				output.push_back("					<hkparam name=\"payload\">" + oriEvents[i]->payload->ID + "</hkparam>");
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
		curEvent->payload->ID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
}

void inputEvent(vecstr& input, shared_ptr<events> element)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"id\">" + to_string(element->id) + "</hkparam>");
	input.push_back("					<hkparam name=\"payload\">" + element->payload->ID + "</hkparam>");
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

			if (matchIDCompare(ori[i]->payload, edit[j]->payload))
			{
				scorelist[i][j] += 2;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<events>> newOri;
	vector<shared_ptr<events>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

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
			cout << "ERROR: Edit hkbStateMachineEventPropertyArray Input Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		vecstr storeline2;
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
					if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
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
