#include <boost\thread.hpp>
#include "hkbeventrangedataarray.h"
#include "highestscore.h"

using namespace std;

namespace eventrangedataarray
{
	const string key = "av";
	const string classname = "hkbEventRangeDataArray";
	const string signature = "0x330a56ee";
}

string hkbeventrangedataarray::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbeventrangedataarray>> hkbeventrangedataarrayList;
safeStringUMap<shared_ptr<hkbeventrangedataarray>> hkbeventrangedataarrayList_E;

void hkbeventrangedataarray::regis(string id, bool isEdited)
{
	isEdited ? hkbeventrangedataarrayList_E[id] = shared_from_this() : hkbeventrangedataarrayList[id] = shared_from_this();
	ID = id;
}

void hkbeventrangedataarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("eventData", line, numelement))
				{
					eventData.reserve(numelement);
					++type;
				}
			}
			case 1:
			{
				double output;

				if (readParam("upperBound", line, output))
				{
					eventData.push_back(output);
					++type;
				}

				break;
			}
			case 2:
			{
				if (readParam("id", line, eventData.back().event.id)) ++type;

				break;
			}
			case 3:
			{
				string output;

				if (readParam("payload", line, output))
				{
					eventData.back().event.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
					++type;
				}

				break;
			}
			case 4:
			{
				string data;

				if (readParam("eventMode", line, data))
				{
					eventData.back().eventMode = data == "EVENT_MODE_SEND_ON_ENTER_RANGE" ? eventdata::EVENT_MODE_SEND_ON_ENTER_RANGE :
						eventdata::EVENT_MODE_SEND_WHEN_IN_RANGE;

					type = 1;
				}

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << eventrangedataarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbeventrangedataarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + eventrangedataarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& event : eventData)
	{
		event.event.id.connectEventInfo(ID, graphroot);
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

			for (auto& event : eventData)
			{
				if (event.event.payload) threadedNextNode(event.event.payload, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
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
				hkbeventrangedataarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbeventrangedataarrayList_E[ID] = protect;
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
				hkbeventrangedataarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbeventrangedataarrayList_E[ID] = protect;
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

string hkbeventrangedataarray::getClassCode()
{
	return eventrangedataarray::key;
}

void hkbeventrangedataarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = eventData.size();
	usize orisize = size;
	hkbeventrangedataarray* ctrpart = static_cast<hkbeventrangedataarray*>(counterpart.get());

	output.push_back(openObject(base, ID, eventrangedataarray::classname, eventrangedataarray::signature));	// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "eventData", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "eventData", size));		// 2

	matchScoring(eventData, ctrpart->eventData, ID);
	size = eventData.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (eventData[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));				// 3
				output.push_back(autoParam(base, "upperBound", ctrpart->eventData[i].upperBound));
				output.push_back(openParam(base, "event"));		// 4
				output.push_back(openObject(base));				// 5
				output.push_back(autoParam(base, "id", ctrpart->eventData[i].event.id));
				output.push_back(autoParam(base, "payload", ctrpart->eventData[i].event.payload));
				output.push_back(closeObject(base));			// 5
				output.push_back(closeParam(base));				// 4
				output.push_back(autoParam(base, "eventMode", ctrpart->eventData[i].getEventMode()));
				output.push_back(closeObject(base));			// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->eventData[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->eventData[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));				// 3
				storeline.push_back(autoParam(base, "upperBound", eventData[i].upperBound));
				storeline.push_back(openParam(base, "event"));		// 4
				storeline.push_back(openObject(base));				// 5
				storeline.push_back(autoParam(base, "id", eventData[i].event.id));
				storeline.push_back(autoParam(base, "payload", eventData[i].event.payload));
				storeline.push_back(closeObject(base));				// 5
				storeline.push_back(closeParam(base));				// 4
				storeline.push_back(autoParam(base, "eventMode", eventData[i].getEventMode()));
				storeline.push_back(closeObject(base));				// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 10)
				{
					output.push_back("");	// 10 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));				// 3
			paramMatch("memberPath", eventData[i].upperBound, ctrpart->eventData[i].upperBound, output, storeline, base, true, open, isEdited);
			output.push_back(openParam(base, "event"));		// 4
			output.push_back(openObject(base));				// 5
			paramMatch("id", eventData[i].event.id, ctrpart->eventData[i].event.id, output, storeline, base, false, open, isEdited);
			paramMatch("payload", eventData[i].event.payload, ctrpart->eventData[i].event.payload, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));			// 5
			output.push_back(closeParam(base));				// 4
			paramMatch("eventMode", eventData[i].getEventMode(), ctrpart->eventData[i].getEventMode(), output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));			// 3
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

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", eventrangedataarray::classname, output, isEdited);
}

void hkbeventrangedataarray::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = eventData.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, eventrangedataarray::classname, eventrangedataarray::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "eventData", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "eventData", size));		// 2
	
	for (auto& data : eventData)
	{
		output.push_back(openObject(base));			// 3
		output.push_back(autoParam(base, "upperBound", data.upperBound));
		output.push_back(openParam(base, "event"));	// 4
		output.push_back(openObject(base));			// 5
		output.push_back(autoParam(base, "id", data.event.id));
		output.push_back(autoParam(base, "payload", data.event.payload));
		output.push_back(closeObject(base));		// 5
		output.push_back(closeParam(base));			// 4
		output.push_back(autoParam(base, "eventMode", data.getEventMode()));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, eventrangedataarray::classname, output, true);
}

void hkbeventrangedataarray::parentRefresh()
{
	for (auto& event : eventData)
	{
		if (event.event.payload) hkb_parent[event.event.payload] = shared_from_this();
	}
}

void hkbeventrangedataarray::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld)
	{
		++functionlayer;
		usize tempint = 0;

		for (auto& event : eventData)
		{
			if (event.event.payload)
			{
				parentRefresh();
				event.event.payload->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(event.event.payload->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& event : eventData)
		{
			if (event.event.payload)
			{
				parentRefresh();
				event.event.payload->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}
}

void hkbeventrangedataarray::matchScoring(vector<eventdata>& ori, vector<eventdata>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<eventdata> newOri;
		vector<eventdata> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(eventdata());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	map<int, map<int, int>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].upperBound == edit[j].upperBound)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i].event.id == edit[j].event.id)
			{
				scorelist[i][j] += 5;
			}

			if (!ori[i].event.payload)
			{
				if (!edit[j].event.payload)
				{
					scorelist[i][j] += 2;
				}
			}
			else if (edit[j].event.payload)
			{
				if (ori[i].event.payload->ID == edit[i].event.payload->ID)
				{
					scorelist[i][j] += 2;
				}
			}

			if (ori[i].eventMode == edit[j].eventMode)
			{
				++scorelist[i][j];
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<eventdata> newOri;
	vector<eventdata> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(eventdata());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(eventdata());
		else newEdit.push_back(edit[order.edited]);
	}

	ori = newOri;
	edit = newEdit;
}

void hkbeventrangedataarray::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

string hkbeventrangedataarray::eventdata::getEventMode()
{
	return eventMode == EVENT_MODE_SEND_WHEN_IN_RANGE ? "EVENT_MODE_SEND_WHEN_IN_RANGE" : "EVENT_MODE_SEND_ON_ENTER_RANGE";
}
