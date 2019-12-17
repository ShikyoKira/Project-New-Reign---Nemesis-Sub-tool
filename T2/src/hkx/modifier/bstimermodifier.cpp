#include <boost\thread.hpp>
#include "bstimermodifier.h"

using namespace std;

namespace timermodifier_b
{
	const string key = "bw";
	const string classname = "BSTimerModifier";
	const string signature = "0x531f3292";
}

string bstimermodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bstimermodifier>> bstimermodifierList;
safeStringUMap<shared_ptr<bstimermodifier>> bstimermodifierList_E;

void bstimermodifier::regis(string id, bool isEdited)
{
	isEdited ? bstimermodifierList_E[id] = shared_from_this() : bstimermodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bstimermodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("alarmTimeSeconds", line, alarmTimeSeconds)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("id", line, alarmEvent.id)) ++type;

					break;
				}
				case 6:
				{
					string output;

					if (readParam("payload", line, output))
					{
						alarmEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 7:
				{
					if (readParam("resetAlarm", line, resetAlarm)) ++type;
				}
			}
		}
		if (line.find("<hkparam name=\"") != string::npos)
		{
			for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
				itr != boost::sregex_iterator(); ++itr)
			{
				string header = itr->str(1);

				if (header == "variableBindingSet") variableBindingSet = isEdited ? hkbvariablebindingsetList_E[itr->str(2)] : hkbvariablebindingsetList[itr->str(2)];
				else if (header == "userData") userData = stoi(itr->str(2));
				else if (header == "name") name = itr->str(2);
				else if (header == "enable") enable = itr->str(2) == "true" ? true : false;
				else if (header == "alarmTimeSeconds") alarmTimeSeconds = stod(itr->str(2));
				else if (header == "id") alarmEvent.id = stoi(itr->str(2));
				else if (header == "payload") alarmEvent.payload = isEdited ? hkbstringeventpayloadList_E[itr->str(2)] : hkbstringeventpayloadList[itr->str(2)];
				else if (header == "resetAlarm") resetAlarm = itr->str(2) == "true" ? true : false;

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << timermodifier_b::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bstimermodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + timermodifier_b::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	alarmEvent.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (alarmEvent.payload) threadedNextNode(alarmEvent.payload, filepath, curadd, functionlayer, graphroot);
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
				bstimermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bstimermodifierList_E[ID] = protect;
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
				bstimermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bstimermodifierList_E[ID] = protect;
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

string bstimermodifier::getClassCode()
{
	return timermodifier_b::key;
}

void bstimermodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(12);
	output.reserve(12);
	usize base = 2;
	bstimermodifier* ctrpart = static_cast<bstimermodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, timermodifier_b::classname, timermodifier_b::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("alarmTimeSeconds", alarmTimeSeconds, ctrpart->alarmTimeSeconds, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "alarmEvent"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", alarmEvent.id, ctrpart->alarmEvent.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", alarmEvent.payload, ctrpart->alarmEvent.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("resetAlarm", resetAlarm, ctrpart->resetAlarm, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", timermodifier_b::classname, output, isEdited);
}

void bstimermodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(12);

	output.push_back(openObject(base, ID, timermodifier_b::classname, timermodifier_b::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "alarmTimeSeconds", alarmTimeSeconds));
	output.push_back(openParam(base, "alarmEvent"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", alarmEvent.id));
	output.push_back(autoParam(base, "payload", alarmEvent.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "resetAlarm", resetAlarm));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, timermodifier_b::classname, output, true);
}

void bstimermodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (alarmEvent.payload) hkb_parent[alarmEvent.payload] = shared_from_this();
}

void bstimermodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (alarmEvent.payload)
	{
		parentRefresh();
		alarmEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bstimermodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
