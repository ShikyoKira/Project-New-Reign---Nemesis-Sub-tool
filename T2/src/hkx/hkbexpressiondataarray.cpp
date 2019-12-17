#include "hkbexpressiondataarray.h"
#include "highestscore.h"

using namespace std;
extern vector<usize> datapacktracker;

namespace expressiondataarray
{
	const string key = "an";
	const string classname = "hkbExpressionDataArray";
	const string signature = "0x4b9ee1a2";
}

string hkbexpressiondataarray::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbexpressiondataarray>> hkbexpressiondataarrayList;
safeStringUMap<shared_ptr<hkbexpressiondataarray>> hkbexpressiondataarrayList_E;

void hkbexpressiondataarray::regis(string id, bool isEdited)
{
	isEdited ? hkbexpressiondataarrayList_E[id] = shared_from_this() : hkbexpressiondataarrayList[id] = shared_from_this();
	ID = id;
}

void hkbexpressiondataarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("expressionsData", line, numelement))
				{
					expressionsData.reserve(numelement);
					++type;
				}
			}
			case 1:
			{
				string output;

				if (readParam("expression", line, output))
				{
					expressionsData.push_back(output);
					++type;
				}

				break;
			}
			case 2:
			{
				if (readParam("assignmentVariableIndex", line, expressionsData.back().assignmentVariableIndex)) ++type;

				break;
			}
			case 3:
			{
				if (readParam("assignmentEventIndex", line, expressionsData.back().assignmentEventIndex)) ++type;

				break;
			}
			case 4:
			{
				string data;

				if (readParam("eventMode", line, data))
				{
					if (data == "EVENT_MODE_SEND_ONCE") expressionsData.back().eventMode = expressiondata::EVENT_MODE_SEND_ONCE;
					else if (data == "EVENT_MODE_SEND_ON_TRUE") expressionsData.back().eventMode = expressiondata::EVENT_MODE_SEND_ON_TRUE;
					else if (data == "EVENT_MODE_SEND_ON_FALSE_TO_TRUE") expressionsData.back().eventMode = expressiondata::EVENT_MODE_SEND_ON_FALSE_TO_TRUE;
					else expressionsData.back().eventMode = expressiondata::EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE;

					type = 1;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << expressiondataarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbexpressiondataarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + expressiondataarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	
	for (auto& data : expressionsData)
	{
		data.assignmentEventIndex.connectEventInfo(ID, graphroot);
		data.assignmentVariableIndex.connectVariableInfo(ID, graphroot);
	}

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
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
				hkbexpressiondataarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbexpressiondataarrayList_E[ID] = protect;
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
				hkbexpressiondataarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbexpressiondataarrayList_E[ID] = protect;
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

string hkbexpressiondataarray::getClassCode()
{
	return expressiondataarray::key;
}

void hkbexpressiondataarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = expressionsData.size();
	usize orisize = size;
	hkbexpressiondataarray* ctrpart = static_cast<hkbexpressiondataarray*>(counterpart.get());

	output.push_back(openObject(base, ID, expressiondataarray::classname, expressiondataarray::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "expressionsData", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "expressionsData", size));		// 2

	matchScoring(expressionsData, ctrpart->expressionsData, ID);
	size = expressionsData.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (expressionsData[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "expression", ctrpart->expressionsData[i].expression));
				output.push_back(autoParam(base, "assignmentVariableIndex", ctrpart->expressionsData[i].assignmentVariableIndex));
				output.push_back(autoParam(base, "assignmentEventIndex", ctrpart->expressionsData[i].assignmentEventIndex));
				output.push_back(autoParam(base, "eventMode", ctrpart->expressionsData[i].getEventMode()));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->expressionsData[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->expressionsData[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "expression", expressionsData[i].expression));
				storeline.push_back(autoParam(base, "assignmentVariableIndex", expressionsData[i].assignmentVariableIndex));
				storeline.push_back(autoParam(base, "assignmentEventIndex", expressionsData[i].assignmentEventIndex));
				storeline.push_back(autoParam(base, "eventMode", expressionsData[i].getEventMode()));
				storeline.push_back(closeObject(base));		// 3
				++i;
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");	// 6 spaces
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("expression", expressionsData[i].expression, ctrpart->expressionsData[i].expression, output, storeline, base, false, open, isEdited);
			paramMatch("assignmentVariableIndex", expressionsData[i].assignmentVariableIndex, ctrpart->expressionsData[i].assignmentVariableIndex, output, storeline, base,
				false, open, isEdited);
			paramMatch("assignmentEventIndex", expressionsData[i].assignmentEventIndex, ctrpart->expressionsData[i].assignmentEventIndex, output, storeline, base, false,
				open, isEdited);
			paramMatch("eventMode", expressionsData[i].getEventMode(), ctrpart->expressionsData[i].getEventMode(), output, storeline, base, true, open, isEdited);
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

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", expressiondataarray::classname, output, isEdited);
}

void hkbexpressiondataarray::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = expressionsData.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, expressiondataarray::classname, expressiondataarray::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "expressionsData", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "expressionsData", size));		// 2

	for (auto& data : expressionsData)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "expression", data.expression));
		output.push_back(autoParam(base, "assignmentVariableIndex", data.assignmentVariableIndex));
		output.push_back(autoParam(base, "assignmentEventIndex", data.assignmentEventIndex));
		output.push_back(autoParam(base, "eventMode", data.getEventMode()));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, expressiondataarray::classname, output, true);
}

void hkbexpressiondataarray::matchScoring(vector<expressiondata>& ori, vector<expressiondata>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<expressiondata> newOri;
		vector<expressiondata> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(expressiondata());
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

			if (ori[i].expression == edit[j].expression)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i].assignmentVariableIndex == edit[j].assignmentVariableIndex)
			{
				++scorelist[i][j];
			}

			if (ori[i].assignmentEventIndex == edit[j].assignmentEventIndex)
			{
				++scorelist[i][j];
			}

			if (ori[i].eventMode == edit[j].eventMode)
			{
				++scorelist[i][j];
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
	vector<expressiondata> newOri;
	vector<expressiondata> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(expressiondata());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(expressiondata());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

string hkbexpressiondataarray::expressiondata::getEventMode()
{
	switch (eventMode)
	{
		case EVENT_MODE_SEND_ONCE: return "EVENT_MODE_SEND_ONCE";
		case EVENT_MODE_SEND_ON_TRUE: return "EVENT_MODE_SEND_ON_TRUE";
		case EVENT_MODE_SEND_ON_FALSE_TO_TRUE: return "EVENT_MODE_SEND_ON_FALSE_TO_TRUE";
		case EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE: return "EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE";
		default: return "EVENT_MODE_SEND_ONCE";
	}
}
