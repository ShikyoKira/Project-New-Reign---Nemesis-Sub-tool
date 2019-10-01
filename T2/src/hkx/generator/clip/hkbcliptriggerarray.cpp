#include <boost\thread.hpp>
#include "hkbcliptriggerarray.h"
#include "highestscore.h"
#include "boolstring.h"
#include "src\hkx\hkbeventproperty.h"

using namespace std;

namespace cliptriggerarray
{
	string key = "w";
	string classname = "hkbClipTriggerArray";
	string signature = "0x59c23a0f";
}

struct triggerinfo
{
	bool proxy = true;

	string localtime;
	eventproperty event;
	bool relativetoend;
	bool acyclic;
	bool isannotation;
};

void inputTrigger(vecstr& input, shared_ptr<triggerinfo> trigger);
void triggerInfoProcess(string line, vector<shared_ptr<triggerinfo>>& triggers, shared_ptr<triggerinfo>& curTrigger);
bool matchScoring(vector<shared_ptr<triggerinfo>>& ori, vector<shared_ptr<triggerinfo>>& edit, string id);

hkbcliptriggerarray::hkbcliptriggerarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + cliptriggerarray::key + to_string(functionlayer) + ">";

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

void hkbcliptriggerarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbClipTriggerArray(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbClipTriggerArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbClipTriggerArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcliptriggerarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbClipTriggerArray(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbClipTriggerArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbClipTriggerArray(newID: " << id << ") with hkbClipTriggerArray(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbClipTriggerArray(newID: " << id << ") with hkbClipTriggerArray(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbClipTriggerArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcliptriggerarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbClipTriggerArray(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: Dummy hkbClipTriggerArray Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbClipTriggerArray(ID: " << id << ") is complete!" << endl;
	}
}

int hkbcliptriggerarray::GetPayloadCount()
{
	return int(payload.size());
}

string hkbcliptriggerarray::GetPayload(int child)
{
	return payload[child];
}

bool hkbcliptriggerarray::IsPayloadNull(int child)
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

string hkbcliptriggerarray::GetAddress()
{
	return address;
}

bool hkbcliptriggerarray::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbcliptriggerarray>> hkbcliptriggerarrayList;
safeStringUMap<shared_ptr<hkbcliptriggerarray>> hkbcliptriggerarrayList_E;

void hkbcliptriggerarray::regis(string id, bool isEdited)
{
	isEdited ? hkbcliptriggerarrayList_E[id] = shared_from_this() : hkbcliptriggerarrayList[id] = shared_from_this();
	ID = id;
}

void hkbcliptriggerarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	bool done = false;
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (!done)
		{
			usize numelement;

			if (readEleParam("triggers", line, numelement))
			{
				triggers.reserve(numelement);
				done = true;
			}
		}
		else if (line.find("<hkparam name=\"") != string::npos)
		{
			switch (type)
			{
				case 0:
				{
					double output;

					if (readParam("localTime", line, output))
					{
						triggers.push_back(output);
						++type;
					}

					break;
				}
				case 1:
				{
					if (readParam("id", line, triggers.back().event.id)) ++type;

					break;
				}
				case 2:
				{
					string output;

					if (readParam("payload", line, output))
					{
						triggers.back().event.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 3:
				{
					if (readParam("relativeToEndOfClip", line, triggers.back().relativeToEndOfClip)) ++type;

					break; 
				}
				case 4:
				{
					if (readParam("acyclic", line, triggers.back().acyclic)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("isAnnotation", line, triggers.back().isAnnotation)) type = 0;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << cliptriggerarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcliptriggerarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + cliptriggerarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& trigger : triggers)
	{
		trigger.event.id.connectEventInfo(ID, graphroot);
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

			for (auto& trigger : triggers)
			{
				if (trigger.event.payload) threadedNextNode(trigger.event.payload, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
			}
		}
		else
		{
			// existed
			if (addressID.find(address) != addressID.end() && IsForeign.find(parent[ID]) == IsForeign.end())
			{
				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbcliptriggerarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbcliptriggerarrayList_E[ID] = protect;
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
				hkbcliptriggerarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbcliptriggerarrayList_E[ID] = protect;
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

string hkbcliptriggerarray::getClassCode()
{
	return cliptriggerarray::key;
}

void hkbcliptriggerarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(250);
	output.reserve(500);
	usize base = 2;
	hkbcliptriggerarray* ctrpart = static_cast<hkbcliptriggerarray*>(counterpart.get());

	output.push_back(openObject(base, ID, cliptriggerarray::classname, cliptriggerarray::signature));		// 1

	usize size = triggers.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "triggers", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "triggers", size));		// 2

	matchScoring(triggers, ctrpart->triggers, ID);
	size = triggers.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (triggers[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "localTime", ctrpart->triggers[i].localTime));
				output.push_back(openParam(base, "event"));		// 4
				output.push_back(openObject(base));		// 5
				output.push_back(autoParam(base, "id", ctrpart->triggers[i].event.id));
				output.push_back(autoParam(base, "payload", ctrpart->triggers[i].event.payload));
				output.push_back(closeObject(base));		// 5
				output.push_back(closeParam(base));		// 4
				output.push_back(autoParam(base, "relativeToEndOfClip", ctrpart->triggers[i].relativeToEndOfClip));
				output.push_back(autoParam(base, "acyclic", ctrpart->triggers[i].acyclic));
				output.push_back(autoParam(base, "isAnnotation", ctrpart->triggers[i].isAnnotation));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->triggers[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->triggers[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "localTime", triggers[i].localTime));
				storeline.push_back(openParam(base, "event"));		// 4
				storeline.push_back(openObject(base));		// 5
				storeline.push_back(autoParam(base, "id", triggers[i].event.id));
				storeline.push_back(autoParam(base, "payload", triggers[i].event.payload));
				storeline.push_back(closeObject(base));		// 5
				storeline.push_back(closeParam(base));		// 4
				storeline.push_back(autoParam(base, "relativeToEndOfClip", triggers[i].relativeToEndOfClip));
				storeline.push_back(autoParam(base, "acyclic", triggers[i].acyclic));
				storeline.push_back(autoParam(base, "isAnnotation", triggers[i].isAnnotation));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 12)
				{
					output.push_back("");	// 12 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("localTime", triggers[i].localTime, ctrpart->triggers[i].localTime, output, storeline, base, true, open, isEdited);
			output.push_back(openParam(base, "event"));		// 2
			output.push_back(openObject(base));		// 3
			paramMatch("id", triggers[i].event.id, ctrpart->triggers[i].event.id, output, storeline, base, false, open, isEdited);
			paramMatch("payload", triggers[i].event.payload, ctrpart->triggers[i].event.payload, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 3
			output.push_back(closeParam(base));		// 2
			paramMatch("relativeToEndOfClip", triggers[i].relativeToEndOfClip, ctrpart->triggers[i].relativeToEndOfClip, output, storeline, base, false, open, isEdited);
			paramMatch("acyclic", triggers[i].acyclic, ctrpart->triggers[i].acyclic, output, storeline, base, false, open, isEdited);
			paramMatch("isAnnotation", triggers[i].isAnnotation, ctrpart->triggers[i].isAnnotation, output, storeline, base, true, open, isEdited);
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", cliptriggerarray::classname, output, isEdited);
}

void hkbcliptriggerarray::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(500);

	output.push_back(openObject(base, ID, cliptriggerarray::classname, cliptriggerarray::signature));		// 1

	usize size = triggers.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "triggers", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "triggers", size));		// 2
	
	for (auto& trigger : triggers)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "localTime", trigger.localTime));
		output.push_back(openParam(base, "event"));		// 2
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "id", trigger.event.id));
		output.push_back(autoParam(base, "payload", trigger.event.payload));
		output.push_back(closeObject(base));		// 3
		output.push_back(closeParam(base));		// 2
		output.push_back(autoParam(base, "relativeToEndOfClip", trigger.relativeToEndOfClip));
		output.push_back(autoParam(base, "acyclic", trigger.acyclic));
		output.push_back(autoParam(base, "isAnnotation", trigger.isAnnotation));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, cliptriggerarray::classname, output, true);
}

void hkbcliptriggerarray::parentRefresh()
{
	for (auto& trigger : triggers)
	{
		if (trigger.event.payload) hkb_parent[trigger.event.payload] = shared_from_this();
	}
}

void hkbcliptriggerarray::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld)
	{
		++functionlayer;
		usize tempint = 0;

		for (auto trigger : triggers)
		{
			if (trigger.event.payload)
			{
				parentRefresh();
				trigger.event.payload->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(trigger.event.payload->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto trigger : triggers)
		{
			if (trigger.event.payload)
			{
				parentRefresh();
				trigger.event.payload->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}
}

void hkbcliptriggerarray::matchScoring(vector<hkbcliptrigger>& ori, vector<hkbcliptrigger>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<hkbcliptrigger> newOri;
		vector<hkbcliptrigger> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(hkbcliptrigger());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	int counter = 1;
	map<int, map<int, double>> scorelist;
	map<int, bool> taken;
	map<int, int> oriRank;
	map<int, int> newRank;
	multimap<double, int> proxyRank;

	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		proxyRank.insert(make_pair(ori[i].localTime, i));
	}

	for (auto& rank : proxyRank)
	{
		oriRank[rank.second] = counter;
		++counter;
	}

	proxyRank.clear();
	counter = 1;

	for (unsigned int i = 0; i < edit.size(); ++i)
	{
		proxyRank.insert(make_pair(edit[i].localTime, i));
	}

	for (auto& rank : proxyRank)
	{
		newRank[rank.second] = counter;
		++counter;
	}

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].relativeToEndOfClip == edit[j].relativeToEndOfClip)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i].event.id == edit[j].event.id)
			{
				scorelist[i][j] += 4;
			}

			if (matchIDCompare(ori[i].event.payload, edit[j].event.payload))
			{
				scorelist[i][j] += 3;
			}

			if (ori[i].localTime == edit[j].localTime)
			{
				scorelist[i][j] += 2.002;
			}
			else
			{
				double num1 = oriRank[i];
				double num2 = newRank[j];
				double num = 1 - (max(num1, num2) - min(num1, num2)) / max(num1, num2);
				scorelist[i][j] += num * 2;
			}

			if (i == j)
			{
				scorelist[i][j] += 0.001;
			}

			if (ori[i].acyclic == edit[j].acyclic)
			{
				++scorelist[i][j];
			}

			if (ori[i].isAnnotation == edit[j].isAnnotation)
			{
				++scorelist[i][j];
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<hkbcliptrigger> newOri;
	vector<hkbcliptrigger> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(hkbcliptrigger());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(hkbcliptrigger());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbcliptriggerarray::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbClipTriggerArrayExport(string id)
{
	// stage 1 reading
	vector<shared_ptr<triggerinfo>> oriTriggers;
	shared_ptr<triggerinfo> curTrigger;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			triggerInfoProcess(FunctionLineTemp[id][i], oriTriggers, curTrigger);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbClipTriggerArray Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curTrigger)
	{
		curTrigger->proxy = false;
		oriTriggers.push_back(curTrigger);
	}

	curTrigger = nullptr;
	vector<shared_ptr<triggerinfo>> newTriggers;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			triggerInfoProcess(FunctionLineNew[id][i], newTriggers, curTrigger);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curTrigger)
	{
		curTrigger->proxy = false;
		newTriggers.push_back(curTrigger);
	}

	if (!matchScoring(oriTriggers, newTriggers, id))
	{
		return;
	}
	
	// stage 2 identify edits
	vecstr output;
	bool IsEdited = false;

	output.push_back(FunctionLineTemp[id][0]);

	if (FunctionLineTemp[id][1] != FunctionLineNew[id][1])
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
		output.push_back(FunctionLineNew[id][1]);
	}

	for (unsigned int i = 0; i < oriTriggers.size(); i++)
	{
		vecstr storeline;
		bool open = false;

		if (newTriggers[i]->proxy)
		{
			vecstr instore;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			bool nobreak = true;

			while (i < oriTriggers.size())
			{
				if (!newTriggers[i]->proxy)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), instore.begin(), instore.end());
					output.push_back("<!-- CLOSE -->");
					nobreak = false;
					--i;
					break;
				}

				int add = 0;

				while (add < 12)
				{
					output.push_back("");
					++add;
				}

				inputTrigger(instore, oriTriggers[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());
				output.push_back("<!-- CLOSE -->");
			}
		}
		else if (!oriTriggers[i]->proxy)
		{
			output.push_back("				<hkobject>");

			if (oriTriggers[i]->localtime != newTriggers[i]->localtime)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"localTime\">" + newTriggers[i]->localtime + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.push_back("					<hkparam name=\"localTime\">" + oriTriggers[i]->localtime + "</hkparam>");
				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"localTime\">" + oriTriggers[i]->localtime + "</hkparam>");
			}

			output.push_back("					<hkparam name=\"event\">");
			output.push_back("						<hkobject>");

			if (oriTriggers[i]->event.id != newTriggers[i]->event.id)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("							<hkparam name=\"id\">" + newTriggers[i]->event.id.getID() + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"id\">" + oriTriggers[i]->event.id.getID() + "</hkparam>");
			}

			if (oriTriggers[i]->event.payload->ID != newTriggers[i]->event.payload->ID)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("							<hkparam name=\"payload\">" + newTriggers[i]->event.payload->ID + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"payload\">" + oriTriggers[i]->event.payload->ID + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					IsEdited = true;
				}
				else
				{
					output.push_back("							<hkparam name=\"payload\">" + newTriggers[i]->event.payload->ID + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + oriTriggers[i]->event.id.getID() + "</hkparam>");
					output.push_back("							<hkparam name=\"payload\">" + oriTriggers[i]->event.payload->ID + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + oriTriggers[i]->event.id.getID() + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back("							<hkparam name=\"payload\">" + oriTriggers[i]->event.payload->ID + "</hkparam>");
			}

			output.push_back("						</hkobject>");
			output.push_back("					</hkparam>");

			if (oriTriggers[i]->relativetoend != newTriggers[i]->relativetoend)
			{
				open = true;
				IsEdited = true;
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(newTriggers[i]->relativetoend) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(oriTriggers[i]->relativetoend) + "</hkparam>");
			}
			else
			{
				output.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(oriTriggers[i]->relativetoend) + "</hkparam>");
			}

			if (oriTriggers[i]->acyclic != newTriggers[i]->acyclic)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"acyclic\">" + from_bool(newTriggers[i]->acyclic) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"acyclic\">" + from_bool(oriTriggers[i]->acyclic) + "</hkparam>");
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

				output.push_back("					<hkparam name=\"acyclic\">" + from_bool(oriTriggers[i]->acyclic) + "</hkparam>");
			}

			if (oriTriggers[i]->isannotation != newTriggers[i]->isannotation)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(newTriggers[i]->isannotation) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(oriTriggers[i]->isannotation) + "</hkparam>");
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

				output.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(oriTriggers[i]->isannotation) + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;

			while (i < oriTriggers.size())
			{
				inputTrigger(output, newTriggers[i]);
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
			cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (File: " << filename << ")" << endl;
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

void triggerInfoProcess(string line, vector<shared_ptr<triggerinfo>>& triggers, shared_ptr<triggerinfo>& curTrigger)
{
	if (line.find("<hkparam name=\"localTime\">") != string::npos)
	{
		if (curTrigger)
		{
			curTrigger->proxy = false;
			triggers.push_back(curTrigger);
		}

		curTrigger = make_shared<triggerinfo>();
		size_t pos = line.find("<hkparam name=\"localTime\">") + 26;
		curTrigger->localtime = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"id\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"id\">") + 19;
		curTrigger->event.id = EventId(stoi(line.substr(pos, line.find("</hkparam>", pos) - pos)));
	}
	else if (line.find("<hkparam name=\"payload\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"payload\">") + 24;
		curTrigger->event.payload->ID = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"relativeToEndOfClip\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"relativeToEndOfClip\">") + 36;
		curTrigger->relativetoend = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"acyclic\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"acyclic\">") + 24;
		curTrigger->acyclic = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"isAnnotation\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"isAnnotation\">") + 29;
		curTrigger->isannotation = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
}

bool matchScoring(vector<shared_ptr<triggerinfo>>& ori, vector<shared_ptr<triggerinfo>>& edit, string id)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: hkbClipTriggerArray empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 1;
	map<int, map<int, double>> scorelist;
	map<int, bool> taken;
	map<int, int> oriRank;
	map<int, int> newRank;
	multimap<double, int> proxyRank;

	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		proxyRank.insert(make_pair(stod(ori[i]->localtime), i));
	}
	
	for (auto& rank : proxyRank)
	{
		oriRank[rank.second] = counter;
		++counter;
	}

	proxyRank.clear();
	counter = 1;

	for (unsigned int i = 0; i < edit.size(); ++i)
	{
		proxyRank.insert(make_pair(stod(edit[i]->localtime), i));
	}

	for (auto& rank : proxyRank)
	{
		newRank[rank.second] = counter;
		++counter;
	}

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->relativetoend == edit[j]->relativetoend)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i]->event.id == edit[j]->event.id)
			{
				scorelist[i][j] += 4;
			}

			if (matchIDCompare(ori[i]->event.payload, edit[j]->event.payload))
			{
				scorelist[i][j] += 3;
			}

			if (ori[i]->localtime == edit[j]->localtime)
			{
				scorelist[i][j] += 2.002;
			}
			else
			{
				double num1 = oriRank[i];
				double num2 = newRank[j];
				double num = 1 - (max(num1, num2) - min(num1, num2)) / max(num1, num2);
				scorelist[i][j] += num * 2;
			}

			if (i == j)
			{
				scorelist[i][j] += 0.001;
			}

			if (ori[i]->acyclic == edit[j]->acyclic)
			{
				++scorelist[i][j];
			}

			if (ori[i]->isannotation == edit[j]->isannotation)
			{
				++scorelist[i][j];
			}
		}
	}
	
	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<triggerinfo>> newOri;
	vector<shared_ptr<triggerinfo>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<triggerinfo>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<triggerinfo>());
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

void inputTrigger(vecstr& input, shared_ptr<triggerinfo> trigger)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"localTime\">" + trigger->localtime + "</hkparam>");
	input.push_back("					<hkparam name=\"event\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"id\">" + trigger->event.id.getID() + "</hkparam>");
	input.push_back("							<hkparam name=\"payload\">" + trigger->event.payload->ID + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(trigger->relativetoend) + "</hkparam>");
	input.push_back("					<hkparam name=\"acyclic\">" + from_bool(trigger->acyclic) + "</hkparam>");
	input.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(trigger->isannotation) + "</hkparam>");
	input.push_back("				</hkobject>");
}

namespace keepsake
{
	void hkbClipTriggerArrayExport(string id)
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
			cout << "ERROR: Edit hkbClipTriggerArray Input Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (ID: " << id << ")" << endl;
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
								if ((storeline2[i].find("<hkparam name=\"localTime\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
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
						if (storeline2[i].find("<hkparam name=\"localTime\">", 0) != string::npos)
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
								if (storeline2[i].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}

								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}
								}
							}
						}
						else
						{
							if (storeline2[i].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
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

		NemesisReaderFormat(stoi(id.substr(1)), storeline2);

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
					if (output[i].find("<hkparam name=\"localTime\">", 0) != string::npos)
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
					else if (output[i].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
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
				cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (File: " << filename << ")" << endl;
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
};
