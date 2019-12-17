#include <boost\thread.hpp>
#include "bseventeveryneventsmodifier.h"

using namespace std;

namespace eventeveryneventsmodifier
{
	const string key = "ba";
	const string classname = "BSEventEveryNEventsModifier";
	const string signature = "0x6030970c";
}

string bseventeveryneventsmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bseventeveryneventsmodifier>> bseventeveryneventsmodifierList;
safeStringUMap<shared_ptr<bseventeveryneventsmodifier>> bseventeveryneventsmodifierList_E;

void bseventeveryneventsmodifier::regis(string id, bool isEdited)
{
	isEdited ? bseventeveryneventsmodifierList_E[id] = shared_from_this() : bseventeveryneventsmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bseventeveryneventsmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("id", line, eventToCheckFor.id)) ++type;

					break;
				}
				case 5:
				{
					string output;

					if (readParam("payload", line, output))
					{
						eventToCheckFor.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("id", line, eventToSend.id)) ++type;

					break;
				}
				case 7:
				{
					string output;

					if (readParam("payload", line, output))
					{
						eventToSend.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 8:
				{
					if (readParam("numberOfEventsBeforeSend", line, numberOfEventsBeforeSend)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("minimumNumberOfEventsBeforeSend", line, minimumNumberOfEventsBeforeSend)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("randomizeNumberOfEvents", line, randomizeNumberOfEvents)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << eventeveryneventsmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bseventeveryneventsmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + eventeveryneventsmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	eventToCheckFor.id.connectEventInfo(ID, graphroot);
	eventToSend.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (eventToCheckFor.payload) threadedNextNode(eventToCheckFor.payload, filepath, curadd + "0", functionlayer, graphroot);

			if (eventToSend.payload) threadedNextNode(eventToSend.payload, filepath, curadd + "1", functionlayer, graphroot);
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
				bseventeveryneventsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bseventeveryneventsmodifierList_E[ID] = protect;
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
				bseventeveryneventsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bseventeveryneventsmodifierList_E[ID] = protect;
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

string bseventeveryneventsmodifier::getClassCode()
{
	return eventeveryneventsmodifier::key;
}

void bseventeveryneventsmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(21);
	output.reserve(21);
	usize base = 2;
	bseventeveryneventsmodifier* ctrpart = static_cast<bseventeveryneventsmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, eventeveryneventsmodifier::classname, eventeveryneventsmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "eventToCheckFor"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", eventToCheckFor.id, ctrpart->eventToCheckFor.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", eventToCheckFor.payload, ctrpart->eventToCheckFor.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "eventToSend"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", eventToSend.id, ctrpart->eventToSend.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", eventToSend.payload, ctrpart->eventToSend.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("numberOfEventsBeforeSend", numberOfEventsBeforeSend, ctrpart->numberOfEventsBeforeSend, output, storeline, base, false, open, isEdited);
	paramMatch("minimumNumberOfEventsBeforeSend", minimumNumberOfEventsBeforeSend, ctrpart->minimumNumberOfEventsBeforeSend, output, storeline, base, false, open, isEdited);
	paramMatch("randomizeNumberOfEvents", randomizeNumberOfEvents, ctrpart->randomizeNumberOfEvents, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", eventeveryneventsmodifier::classname, output, isEdited);
}

void bseventeveryneventsmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(21);

	output.push_back(openObject(base, ID, eventeveryneventsmodifier::classname, eventeveryneventsmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "eventToCheckFor"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", eventToCheckFor.id));
	output.push_back(autoParam(base, "payload", eventToCheckFor.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "eventToSend"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", eventToSend.id));
	output.push_back(autoParam(base, "payload", eventToSend.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "numberOfEventsBeforeSend", numberOfEventsBeforeSend));
	output.push_back(autoParam(base, "minimumNumberOfEventsBeforeSend", minimumNumberOfEventsBeforeSend));
	output.push_back(autoParam(base, "randomizeNumberOfEvents", randomizeNumberOfEvents));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, eventeveryneventsmodifier::classname, output, true);
}

void bseventeveryneventsmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (eventToCheckFor.payload) hkb_parent[eventToCheckFor.payload] = shared_from_this();
	if (eventToSend.payload) hkb_parent[eventToSend.payload] = shared_from_this();
}

void bseventeveryneventsmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (eventToCheckFor.payload)
	{
		parentRefresh();
		eventToCheckFor.payload->connect(filepath, isOld ? address + "0" : address, functionlayer, true, graphroot);
	}

	if (eventToSend.payload)
	{
		parentRefresh();
		eventToSend.payload->connect(filepath, isOld ? address + "1" : address, functionlayer, true, graphroot);
	}
}

void bseventeveryneventsmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
