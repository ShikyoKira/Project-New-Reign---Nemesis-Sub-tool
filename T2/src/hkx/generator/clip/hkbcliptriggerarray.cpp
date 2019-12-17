#include <boost\thread.hpp>
#include "hkbcliptriggerarray.h"
#include "highestscore.h"
#include "boolstring.h"
#include "src\hkx\hkbeventproperty.h"

using namespace std;

namespace cliptriggerarray
{
	const string key = "w";
	const string classname = "hkbClipTriggerArray";
	const string signature = "0x59c23a0f";
}

string hkbcliptriggerarray::GetAddress()
{
	return address;
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
