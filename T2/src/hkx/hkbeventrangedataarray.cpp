#include <boost\thread.hpp>
#include "hkbeventrangedataarray.h"
#include "highestscore.h"

using namespace std;

namespace eventrangedataarray
{
	string key = "av";
	string classname = "hkbEventRangeDataArray";
	string signature = "0x330a56ee";
}

hkbeventrangedataarray::hkbeventrangedataarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + eventrangedataarray::key + to_string(functionlayer) + ">";

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
}

void hkbeventrangedataarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventRangeDataArray(ID: " << id << ") has been initialized!" << endl;
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
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbEventRangeDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbEventRangeDataArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventrangedataarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventRangeDataArray(ID: " << id << ") has been initialized!" << endl;
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
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

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
		cout << "ERROR: hkbEventRangeDataArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbEventRangeDataArray(newID: " << id << ") with hkbEventRangeDataArray(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbEventRangeDataArray(newID: " << id << ") with hkbEventRangeDataArray(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbEventRangeDataArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventrangedataarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbEventRangeDataArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

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
		cout << "ERROR: Dummy hkbEventRangeDataArray Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbEventRangeDataArray(ID: " << id << ") is complete!" << endl;
	}
}

int hkbeventrangedataarray::GetPayloadCount()
{
	return int(payload.size());
}

string hkbeventrangedataarray::GetPayload(int child)
{
	return payload[child];
}

bool hkbeventrangedataarray::IsPayloadNull(int child)
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

string hkbeventrangedataarray::GetAddress()
{
	return address;
}

bool hkbeventrangedataarray::IsNegate()
{
	return IsNegated;
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

void hkbEventRangeDataArrayExport(string id)
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
		cout << "ERROR: Edit hkbEventRangeDataArray Input Not Found (ID: " << id << ")" << endl;
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
		cout << "ERROR: Edit hkbEventRangeDataArray Output Not Found (ID: " << id << ")" << endl;
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
							if ((storeline2[i].find("<hkparam name=\"upperBound\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
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
					if (storeline2[i].find("<hkparam name=\"upperBound\">", 0) != string::npos)
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
							if (storeline2[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
							{
								output.push_back("				</hkobject>");
							}

							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"eventMode\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}
							}
						}
					}
					else
					{
						if (storeline2[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
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
				if (output[i].find("<hkparam name=\"upperBound\">", 0) != string::npos)
				{
					if (output[i + 1].find("CLOSE", 0) != string::npos)
					{
						if (output[i - 1].find("ORIGINAL", 0) == string::npos)
						{
							fwrite << "				<hkobject>" << "\n";
						}

						fwrite << output[i] << "\n";
						fwrite << "<!-- CLOSE -->" << "\n";
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
					}
				}
				else if (output[i].find("<hkparam name=\"event\">", 0) != string::npos)
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
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
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
				else if (output[i].find("<hkparam name=\"eventMode\">", 0) != string::npos)
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
			cout << "ERROR: Edit hkbEventRangeDataArray Output Not Found (File: " << filename << ")" << endl;
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

string hkbeventrangedataarray::eventdata::getEventMode()
{
	return eventMode == EVENT_MODE_SEND_WHEN_IN_RANGE ? "EVENT_MODE_SEND_WHEN_IN_RANGE" : "EVENT_MODE_SEND_ON_ENTER_RANGE";
}
