#include <boost\thread.hpp>
#include "bseventonfalsetotruemodifier.h"

using namespace std;

namespace eventonfalsetotruemodifier
{
	string key = "bd";
	string classname = "BSEventOnFalseToTrueModifier";
	string signature = "0x81d0777a";
}

bseventonfalsetotruemodifier::bseventonfalsetotruemodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + eventonfalsetotruemodifier::key + to_string(functionlayer) + ">";

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

void bseventonfalsetotruemodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSEventOnFalseToTrueModifier(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	payload.reserve(3);

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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: BSEventOnFalseToTrueModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSEventOnFalseToTrueModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bseventonfalsetotruemodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSEventOnFalseToTrueModifier(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	payload.reserve(3);

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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
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
		cout << "ERROR: BSEventOnFalseToTrueModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSEventOnFalseToTrueModifier(newID: " << id << ") with BSEventOnFalseToTrueModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

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
			cout << "Comparing BSEventOnFalseToTrueModifier(newID: " << id << ") with BSEventOnFalseToTrueModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSEventOnFalseToTrueModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bseventonfalsetotruemodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSEventOnFalseToTrueModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
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
		cout << "ERROR: Dummy BSEventOnFalseToTrueModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSEventOnFalseToTrueModifier(ID: " << id << ") is complete!" << endl;
	}
}

int bseventonfalsetotruemodifier::GetPayloadCount()
{
	return int(payload.size());
}

string bseventonfalsetotruemodifier::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bseventonfalsetotruemodifier::IsPayloadNull(int child)
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

string bseventonfalsetotruemodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bseventonfalsetotruemodifier::IsBindingNull()
{
	if (variablebindingset.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bseventonfalsetotruemodifier::GetAddress()
{
	return address;
}

bool bseventonfalsetotruemodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bseventonfalsetotruemodifier>> bseventonfalsetotruemodifierList;
safeStringUMap<shared_ptr<bseventonfalsetotruemodifier>> bseventonfalsetotruemodifierList_E;

void bseventonfalsetotruemodifier::regis(string id, bool isEdited)
{
	isEdited ? bseventonfalsetotruemodifierList_E[id] = shared_from_this() : bseventonfalsetotruemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bseventonfalsetotruemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("enable", line, enable)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("bEnableEvent1", line, bEnableEvent1)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("bVariableToTest1", line, bVariableToTest1)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("id", line, EventToSend1.id)) ++type;

					break;
				}
				case 7:
				{
					string output;

					if (readParam("payload", line, output))
					{
						EventToSend1.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 8:
				{
					if (readParam("bEnableEvent2", line, bEnableEvent2)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("bVariableToTest2", line, bVariableToTest2)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("id", line, EventToSend2.id)) ++type;

					break;
				}
				case 11:
				{
					string output;

					if (readParam("payload", line, output))
					{
						EventToSend2.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 12:
				{
					if (readParam("bEnableEvent3", line, bEnableEvent3)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("bVariableToTest3", line, bVariableToTest3)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("id", line, EventToSend3.id)) ++type;

					break;
				}
				case 15:
				{
					string output;

					if (readParam("payload", line, output))
					{
						EventToSend3.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << eventonfalsetotruemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bseventonfalsetotruemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + eventonfalsetotruemodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	EventToSend1.id.connectEventInfo(ID, graphroot);
	EventToSend2.id.connectEventInfo(ID, graphroot);
	EventToSend3.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (EventToSend1.payload) threadedNextNode(EventToSend1.payload, filepath, curadd + "0", functionlayer, graphroot);

			if (EventToSend2.payload) threadedNextNode(EventToSend2.payload, filepath, curadd + "1", functionlayer, graphroot);

			if (EventToSend3.payload) threadedNextNode(EventToSend3.payload, filepath, curadd + "2", functionlayer, graphroot);
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
				bseventonfalsetotruemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bseventonfalsetotruemodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
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
				bseventonfalsetotruemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bseventonfalsetotruemodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
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

string bseventonfalsetotruemodifier::getClassCode()
{
	return eventonfalsetotruemodifier::key;
}

void bseventonfalsetotruemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(30);
	output.reserve(30);
	usize base = 2;
	bseventonfalsetotruemodifier* ctrpart = static_cast<bseventonfalsetotruemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, eventonfalsetotruemodifier::classname, eventonfalsetotruemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("bEnableEvent1", bEnableEvent1, ctrpart->bEnableEvent1, output, storeline, base, false, open, isEdited);
	paramMatch("bVariableToTest1", bVariableToTest1, ctrpart->bVariableToTest1, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "EventToSend1"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", EventToSend1.id, ctrpart->EventToSend1.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", EventToSend1.payload, ctrpart->EventToSend1.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("bEnableEvent2", bEnableEvent2, ctrpart->bEnableEvent2, output, storeline, base, false, open, isEdited);
	paramMatch("bVariableToTest2", bVariableToTest2, ctrpart->bVariableToTest2, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "EventToSend2"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", EventToSend2.id, ctrpart->EventToSend2.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", EventToSend2.payload, ctrpart->EventToSend2.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("bEnableEvent3", bEnableEvent3, ctrpart->bEnableEvent3, output, storeline, base, false, open, isEdited);
	paramMatch("bVariableToTest3", bVariableToTest3, ctrpart->bVariableToTest3, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "EventToSend3"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", EventToSend3.id, ctrpart->EventToSend3.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", EventToSend3.payload, ctrpart->EventToSend3.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", eventonfalsetotruemodifier::classname, output, isEdited);
}

void bseventonfalsetotruemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(30);

	output.push_back(openObject(base, ID, eventonfalsetotruemodifier::classname, eventonfalsetotruemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "bEnableEvent1", bEnableEvent1));
	output.push_back(autoParam(base, "bVariableToTest1", bVariableToTest1));
	output.push_back(openParam(base, "EventToSend1"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", EventToSend1.id));
	output.push_back(autoParam(base, "payload", EventToSend1.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "bEnableEvent2", bEnableEvent2));
	output.push_back(autoParam(base, "bVariableToTest2", bVariableToTest2));
	output.push_back(openParam(base, "EventToSend2"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", EventToSend2.id));
	output.push_back(autoParam(base, "payload", EventToSend2.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "bEnableEvent3", bEnableEvent3));
	output.push_back(autoParam(base, "bVariableToTest3", bVariableToTest3));
	output.push_back(openParam(base, "EventToSend3"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", EventToSend3.id));
	output.push_back(autoParam(base, "payload", EventToSend3.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, eventonfalsetotruemodifier::classname, output, true);
}

void bseventonfalsetotruemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (EventToSend1.payload) hkb_parent[EventToSend1.payload] = shared_from_this();
	if (EventToSend2.payload) hkb_parent[EventToSend2.payload] = shared_from_this();
	if (EventToSend3.payload) hkb_parent[EventToSend3.payload] = shared_from_this();
}

void bseventonfalsetotruemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (EventToSend1.payload)
	{
		parentRefresh();
		EventToSend1.payload->connect(filepath, isOld ? address + "0" : address, functionlayer, true, graphroot);
	}

	if (EventToSend2.payload)
	{
		parentRefresh();
		EventToSend2.payload->connect(filepath, isOld ? address + "1" : address, functionlayer, true, graphroot);
	}

	if (EventToSend3.payload)
	{
		parentRefresh();
		EventToSend3.payload->connect(filepath, isOld ? address + "2" : address, functionlayer, true, graphroot);
	}
}

void bseventonfalsetotruemodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSEventOnFalseToTrueModifierExport(string id)
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
		cout << "ERROR: Edit BSEventOnFalseToTrueModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit BSEventOnFalseToTrueModifier Output Not Found (File: " << filename << ")" << endl;
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