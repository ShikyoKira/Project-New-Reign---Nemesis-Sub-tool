#include <boost\thread.hpp>
#include "bsistatemanagermodifier.h"
#include "highestscore.h"

using namespace std;

namespace istatemanagermodifier
{
	const string key = "bj";
	const string classname = "BSIStateManagerModifier";
	const string signature = "0x6cb24f2e";
}

string bsistatemanagermodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsistatemanagermodifier>> bsistatemanagermodifierList;
safeStringUMap<shared_ptr<bsistatemanagermodifier>> bsistatemanagermodifierList_E;

void bsistatemanagermodifier::regis(string id, bool isEdited)
{
	isEdited ? bsistatemanagermodifierList_E[id] = shared_from_this() : bsistatemanagermodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsistatemanagermodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("iStateVar", line, iStateVar)) ++type;

					break;
				}
				case 5:
				{
					usize numelement;

					if (readEleParam("stateData", line, numelement))
					{
						stateData.reserve(numelement);
						++type;
					}

					break;
				}
				case 6:
				{
					string output;

					if (readParam("pStateMachine", line, output))
					{
						stateData.push_back((isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output]);
						++type;
					}

					break;
				}
				case 7:
				{
					if (readParam("StateID", line, stateData.back().StateID)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("iStateToSetAs", line, stateData.back().iStateToSetAs)) type = 6;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << istatemanagermodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsistatemanagermodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + istatemanagermodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			usize tempint = 0;
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				boost::thread_group multithreads;

				for (auto& state : stateData)
				{
					if (state.pStateMachine) multithreads.create_thread(boost::bind(&bsistatemanagermodifier::threadedNextNode, this, state.pStateMachine, filepath,
						curadd + to_string(tempint++), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
				multithreads.join_all();
				num_thread -= multithreads.size();
			}
			else
			{
				for (auto& state : stateData)
				{
					if (state.pStateMachine) threadedNextNode(state.pStateMachine, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
				}
			}
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
				bsistatemanagermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsistatemanagermodifierList_E[ID] = protect;
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
				bsistatemanagermodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsistatemanagermodifierList_E[ID] = protect;
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

string bsistatemanagermodifier::getClassCode()
{
	return istatemanagermodifier::key;
}

void bsistatemanagermodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(100);
	usize base = 2;
	bsistatemanagermodifier* ctrpart = static_cast<bsistatemanagermodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, istatemanagermodifier::classname, istatemanagermodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("iStateVar", iStateVar, ctrpart->iStateVar, output, storeline, base, true, open, isEdited);

	usize size = stateData.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "stateData", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "stateData", size));		// 2

	matchScoring(stateData, ctrpart->stateData, ID);
	size = stateData.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (stateData[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "pStateMachine", ctrpart->stateData[i].pStateMachine));
				output.push_back(autoParam(base, "StateID", ctrpart->stateData[i].StateID));
				output.push_back(autoParam(base, "iStateToSetAs", ctrpart->stateData[i].iStateToSetAs));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->stateData[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->stateData[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "pStateMachine", stateData[i].pStateMachine));
				storeline.push_back(autoParam(base, "StateID", stateData[i].StateID));
				storeline.push_back(autoParam(base, "iStateToSetAs", stateData[i].iStateToSetAs));
				storeline.push_back(closeObject(base));		// 3
				++i;
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");	// 5 spaces
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("pStateMachine", stateData[i].pStateMachine, ctrpart->stateData[i].pStateMachine, output, storeline, base, false, open, isEdited);
			paramMatch("StateID", stateData[i].StateID, ctrpart->stateData[i].StateID, output, storeline, base, false, open, isEdited);
			paramMatch("iStateToSetAs", stateData[i].iStateToSetAs, ctrpart->stateData[i].iStateToSetAs, output, storeline, base, true, open, isEdited);
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", istatemanagermodifier::classname, output, isEdited);
}

void bsistatemanagermodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, istatemanagermodifier::classname, istatemanagermodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "iStateVar", iStateVar));

	usize size = stateData.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "stateData", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "stateData", size));		// 2

	for (auto& state : stateData)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "pStateMachine", state.pStateMachine));
		output.push_back(autoParam(base, "StateID", state.StateID));
		output.push_back(autoParam(base, "iStateToSetAs", state.iStateToSetAs));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, istatemanagermodifier::classname, output, true);
}

void bsistatemanagermodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& state : stateData)
	{
		if (state.pStateMachine) hkb_parent[state.pStateMachine] = shared_from_this();
	}
}

void bsistatemanagermodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto& state : stateData)
		{
			if (state.pStateMachine)
			{
				parentRefresh();
				state.pStateMachine->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(state.pStateMachine->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& state : stateData)
		{
			if (state.pStateMachine)
			{
				parentRefresh();
				state.pStateMachine->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}
}

void bsistatemanagermodifier::matchScoring(vector<statedata>& ori, vector<statedata>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<statedata> newOri;
		vector<statedata> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(statedata());
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

			if (ori[i].pStateMachine == edit[j].pStateMachine)
			{
				++scorelist[i][j];
			}

			if (ori[i].StateID == edit[j].StateID)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].iStateToSetAs == edit[j].iStateToSetAs)
			{
				scorelist[i][j] += 3;
			}

			if (i == j)
			{
				++scorelist[i][j];
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<statedata> newOri;
	vector<statedata> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(statedata());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(statedata());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void bsistatemanagermodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
