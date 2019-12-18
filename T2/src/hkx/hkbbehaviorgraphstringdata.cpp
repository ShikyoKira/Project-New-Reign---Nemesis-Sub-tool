#include <boost\thread.hpp>

#include "highestscore.h"
#include "hkrootlevelcontainer.h"
#include "hkbbehaviorgraphstringdata.h"


using namespace std;
boost::atomic_flag datalock = BOOST_ATOMIC_FLAG_INIT;

namespace behaviorgraphstringdata
{
	const string key = "b";
	const string classname = "hkbBehaviorGraphStringData";
	string signature = "0xc713064e";
}

string hkbbehaviorgraphstringdata::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbbehaviorgraphstringdata>> hkbbehaviorgraphstringdataList;
safeStringUMap<shared_ptr<hkbbehaviorgraphstringdata>> hkbbehaviorgraphstringdataList_E;

void hkbbehaviorgraphstringdata::regis(string id, bool isEdited)
{
	isEdited ? hkbbehaviorgraphstringdataList_E[id] = shared_from_this() : hkbbehaviorgraphstringdataList[id] = shared_from_this();
	ID = id;
}

void hkbbehaviorgraphstringdata::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	usize counter;
	usize select;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("eventNames", line, numelement))
				{
					eventNames = make_shared<eventinfopack>();

					if (isEdited)
					{
						eventPackNode_E[ID] = eventNames;

						while (datalock.test_and_set(boost::memory_order_acquire));
						eventID.push_back(mapstring(numelement));
						select = eventID.size() - 1;
						datalock.clear(boost::memory_order_release);
					}
					else
					{
						eventPackNode[ID] = eventNames;
						eventCount.push_back(numelement);
					}

					eventNames->reserve(numelement);
					counter = 0;
					++type;
				}

				break;
			}
			case 1:
			{
				usize pos = line.find("<hkcstring>");

				if (pos != string::npos)
				{
					pos += 11;
					usize pos2 = line.find("</hkcstring>", pos);

					if (pos2 != string::npos)
					{
						string name = line.substr(pos, pos2 - pos);
						eventNames->push_back(name);

						if (isEdited)
						{
							while (datalock.test_and_set(boost::memory_order_acquire));
							eventID[select][to_string(counter++)] = name;
							datalock.clear(boost::memory_order_release);
						}
					}
				}
				else
				{
					usize numelement;

					if (readEleParam("attributeNames", line, numelement))
					{
						attributeNames = make_shared<attributeinfopack>();

						if (isEdited)
						{
							attributePackNode_E[ID] = attributeNames;

							while (datalock.test_and_set(boost::memory_order_acquire));
							attributeID.push_back(mapstring(numelement));
							select = attributeID.size() - 1;
							datalock.clear(boost::memory_order_release);
						}
						else
						{
							attributePackNode[ID] = attributeNames;
						}

						attributeNames->reserve(numelement);
						counter = 0;
						++type;
					}
				}

				break;
			}
			case 2:
			{
				usize pos = line.find("<hkcstring>");

				if (pos != string::npos)
				{
					pos += 11;
					usize pos2 = line.find("</hkcstring>", pos);

					if (pos2 != string::npos)
					{
						string name = line.substr(pos, pos2 - pos);
						attributeNames->push_back(name);

						if (isEdited)
						{
							while (datalock.test_and_set(boost::memory_order_acquire));
							attributeID[select][to_string(counter++)] = name;
							datalock.clear(boost::memory_order_release);
						}
					}
				}
				else
				{
					usize numelement;

					if (readEleParam("variableNames", line, numelement))
					{
						variableNames = make_shared<variableinfopack>();
						
						if (isEdited)
						{
							variablePackNode_E[ID] = variableNames;

							while (datalock.test_and_set(boost::memory_order_acquire));
							variableID.push_back(mapstring(numelement));
							select = variableID.size() - 1;
							datalock.clear(boost::memory_order_release);
						}
						else
						{
							variablePackNode[ID] = variableNames;
							varCount.push_back(numelement);
						}

						variableNames->reserve(numelement);
						counter = 0;
						++type;
					}
				}

				break;
			}
			case 3:
			{
				usize pos = line.find("<hkcstring>");

				if (pos != string::npos)
				{
					pos += 11;
					usize pos2 = line.find("</hkcstring>", pos);

					if (pos2 != string::npos)
					{
						string name = line.substr(pos, pos2 - pos);
						variableNames->push_back(name);

						if (isEdited)
						{
							while (datalock.test_and_set(boost::memory_order_acquire));
							variableID[select][to_string(counter++)] = name;
							datalock.clear(boost::memory_order_release);
						}
					}
				}
				else
				{
					usize numelement;

					if (readEleParam("characterPropertyNames", line, numelement))
					{
						characterPropertyNames = make_shared<variableinfopack>();

						if (isEdited)
						{
							variablePackNode_E[ID + "C"] = characterPropertyNames;

							while (datalock.test_and_set(boost::memory_order_acquire));
							characterID.push_back(mapstring(numelement));
							select = characterID.size() - 1;
							datalock.clear(boost::memory_order_release);
						}
						else
						{
							variablePackNode[ID + "C"] = characterPropertyNames;
						}

						characterPropertyNames->reserve(numelement);
						counter = 0;
						++type;
					}
				}

				break;
			}
			case 4:
			{
				usize pos = line.find("<hkcstring>");

				if (pos != string::npos)
				{
					pos += 11;
					usize pos2 = line.find("</hkcstring>", pos);

					if (pos2 != string::npos)
					{
						string name = line.substr(pos, pos2 - pos);
						characterPropertyNames->push_back(name);

						if (isEdited)
						{
							while (datalock.test_and_set(boost::memory_order_acquire));
							characterID[select][to_string(counter++)] = name;
							datalock.clear(boost::memory_order_release);
						}
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << behaviorgraphstringdata::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphstringdata::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + behaviorgraphstringdata::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			// existed
			if (addressID.find(address) != addressID.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				auto protectEvent = eventPackNode_E[ID];
				auto protectAttribute = attributePackNode_E[ID];
				auto protectVariable = variablePackNode_E[ID];
				auto protectCharacter = variablePackNode_E[ID + "C"];
				hkbbehaviorgraphstringdataList_E.erase(ID);
				editedBehavior.erase(ID);
				eventPackNode_E.erase(ID);
				ID = addressID[address];
				hkbbehaviorgraphstringdataList_E[ID] = protect;
				editedBehavior[ID] = protect;
				eventPackNode_E[ID] = protectEvent;
				attributePackNode_E[ID] = protectAttribute;
				variablePackNode_E[ID] = protectVariable;
				variablePackNode_E[ID + "C"] = protectCharacter;
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
				auto protectEvent = eventPackNode_E[ID];
				auto protectAttribute = attributePackNode_E[ID];
				auto protectVariable = variablePackNode_E[ID];
				auto protectCharacter = variablePackNode_E[ID + "C"];
				hkbbehaviorgraphstringdataList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				eventPackNode_E.erase(ID);
				attributePackNode_E.erase(ID);
				variablePackNode_E.erase(ID);
				variablePackNode_E.erase(ID + "C");
				ID = addressID[address];
				hkbbehaviorgraphstringdataList_E[ID] = protect;
				editedBehavior[ID] = protect;
				eventPackNode_E[ID] = protectEvent;
				attributePackNode_E[ID] = protectAttribute;
				variablePackNode_E[ID] = protectVariable;
				variablePackNode_E[ID + "C"] = protectCharacter;
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

string hkbbehaviorgraphstringdata::getClassCode()
{
	return behaviorgraphstringdata::key;
}

void hkbbehaviorgraphstringdata::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(5000);
	usize base = 2;
	hkbbehaviorgraphstringdata* ctrpart = static_cast<hkbbehaviorgraphstringdata*>(counterpart.get());

	output.push_back(openObject(base, ID, behaviorgraphstringdata::classname, behaviorgraphstringdata::signature));	// 1

	// event names
	if (eventNames)
	{
		bool open = false;
		vecstr storeline;
		usize size = eventNames->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "eventNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "eventNames", size));		// 2

		eventfunc::matchScoring(*eventNames, *ctrpart->eventNames, ID, behaviorgraphstringdata::classname);
		size = eventNames->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*eventNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(autoString(base, (*ctrpart->eventNames)[i].name));
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->eventNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->eventNames)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(autoString(base, (*eventNames)[i].name));
					++i;
					output.push_back("");	// 1 spaces
				}
			}
			// both exist
			else
			{
				stringMatch((*eventNames)[i].name, (*ctrpart->eventNames)[i].name, output, storeline, base, false, open, isEdited);
			}
		}

		if (size > 0)
		{
			if (eventNames->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "eventNames", 0) + closeParam());		// 2
		--base;
	}

	// attribute names
	if (attributeNames)
	{
		bool open = false;
		vecstr storeline;
		usize size = attributeNames->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "attributeNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "attributeNames", size));		// 2

		attributefunc::matchScoring(*attributeNames, *ctrpart->attributeNames, ID, behaviorgraphstringdata::classname);
		size = attributeNames->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*attributeNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(autoString(base, (*ctrpart->attributeNames)[i].name));
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->attributeNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->attributeNames)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(autoString(base, (*attributeNames)[i].name));
					++i;
					output.push_back("");	// 1 spaces
				}
			}
			// both exist
			else
			{
				stringMatch((*attributeNames)[i].name, (*ctrpart->attributeNames)[i].name, output, storeline, base, false, open, isEdited);
			}
		}

		if (size > 0)
		{
			if (attributeNames->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "attributeNames", 0) + closeParam());		// 2
		--base;
	}

	// variable names
	if (variableNames)
	{
		bool open = false;
		vecstr storeline;
		usize size = variableNames->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "variableNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "variableNames", size));		// 2

		variablefunc::matchScoring(*variableNames, *ctrpart->variableNames, ID, behaviorgraphstringdata::classname);
		size = variableNames->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*variableNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(autoString(base, (*ctrpart->variableNames)[i].name));
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->variableNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->variableNames)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(autoString(base, (*variableNames)[i].name));
					++i;
					output.push_back("");	// 1 spaces
				}
			}
			// both exist
			else
			{
				stringMatch((*variableNames)[i].name, (*ctrpart->variableNames)[i].name, output, storeline, base, false, open, isEdited);
			}
		}

		if (size > 0)
		{
			if (variableNames->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "variableNames", 0) + closeParam());		// 2
		--base;
	}

	// character property names
	if (characterPropertyNames)
	{
		bool open = false;
		vecstr storeline;
		usize size = characterPropertyNames->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "characterPropertyNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "characterPropertyNames", size));		// 2

		variablefunc::matchScoring(*characterPropertyNames, *ctrpart->characterPropertyNames, ID, behaviorgraphstringdata::classname);
		size = characterPropertyNames->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*characterPropertyNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(autoString(base, (*ctrpart->characterPropertyNames)[i].name));
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->characterPropertyNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->characterPropertyNames)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(autoString(base, (*characterPropertyNames)[i].name));
					++i;
					output.push_back("");	// 1 spaces
				}
			}
			// both exist
			else
			{
				stringMatch((*characterPropertyNames)[i].name, (*ctrpart->characterPropertyNames)[i].name, output, storeline, base, false, open, isEdited);
			}
		}

		if (size > 0)
		{
			if (characterPropertyNames->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "characterPropertyNames", 0) + closeParam());		// 2
		--base;
	}

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", behaviorgraphstringdata::classname, output, isEdited);
}

void hkbbehaviorgraphstringdata::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(5000);

	output.push_back(openObject(base, ID, behaviorgraphstringdata::classname, behaviorgraphstringdata::signature));		// 1

	if (eventNames)
	{
		if (eventNames->scan_done)
		{
			cout << "ERROR: node " << ID << "has been referenced as original and newly created" << endl;
			Error = true;
			throw 5;
		}

		usize size = eventNames->size();

		if (size == 0)
		{
			output.push_back(openParam(base, "eventNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "eventNames", size));		// 2
		
		for (auto& line : (*eventNames))
		{
			output.push_back(autoString(base, line.name));
		}

		if (size > 0) output.push_back(closeParam(base));		// 2
	}
	else
	{
		output.push_back(openParam(base, "eventNames", 0) + closeParam());		// 2
		--base;
	}

	if (attributeNames)
	{
		if (attributeNames->scan_done)
		{
			cout << "ERROR: node " << ID << "has been referenced as original and newly created" << endl;
			Error = true;
			throw 5;
		}

		usize size = attributeNames->size();

		if (size == 0)
		{
			output.push_back(openParam(base, "attributeNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "attributeNames", size));		// 2

		for (auto& line : (*attributeNames))
		{
			output.push_back(autoString(base, line.name));
		}

		if (size > 0) output.push_back(closeParam(base));		// 2
	}
	else
	{
		output.push_back(openParam(base, "attributeNames", 0) + closeParam());		// 2
		--base;
	}

	if (variableNames)
	{
		if (variableNames->scan_done)
		{
			cout << "ERROR: node " << ID << "has been referenced as original and newly created" << endl;
			Error = true;
			throw 5;
		}

		usize size = variableNames->size();

		if (size == 0)
		{
			output.push_back(openParam(base, "variableNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "variableNames", size));		// 2

		for (auto& line : (*variableNames))
		{
			output.push_back(autoString(base, line.name));
		}

		if (size > 0) output.push_back(closeParam(base));		// 2
	}
	else
	{
		output.push_back(openParam(base, "variableNames", 0) + closeParam());		// 2
		--base;
	}

	if (characterPropertyNames)
	{
		if (characterPropertyNames->scan_done)
		{
			cout << "ERROR: node " << ID << "has been referenced as original and newly created" << endl;
			Error = true;
			throw 5;
		}

		usize size = characterPropertyNames->size();

		if (size == 0)
		{
			output.push_back(openParam(base, "characterPropertyNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "characterPropertyNames", size));		// 2

		for (auto& line : (*characterPropertyNames))
		{
			output.push_back(autoString(base, line.name));
		}

		if (size > 0) output.push_back(closeParam(base));		// 2
	}
	else
	{
		output.push_back(openParam(base, "characterPropertyNames", 0) + closeParam());		// 2
		--base;
	}

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, behaviorgraphstringdata::classname, output, true);
}

void hkbbehaviorgraphstringdata::matchScoring(vector<e_datainfo>& ori, vector<e_datainfo>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<e_datainfo> newOri;
		vector<e_datainfo> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(e_datainfo());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	map<int, map<int, double>> scorelist;
	usize avg_threadsize = ori.size() / 5;

	if (avg_threadsize < 100)
	{
		fillScore(ori, edit, 0, ori.size(), scorelist);
	}
	else
	{
		map<int, map<int, double>> scorelist1;
		map<int, map<int, double>> scorelist2;
		map<int, map<int, double>> scorelist3;
		map<int, map<int, double>> scorelist4;
		map<int, map<int, double>> scorelist5;
		boost::thread_group multi_t;

		usize start = 0;
		usize cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbbehaviorgraphstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, scorelist1));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbbehaviorgraphstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, scorelist2));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbbehaviorgraphstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, scorelist3));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbbehaviorgraphstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, scorelist4));

		start = cap;
		cap = ori.size();
		fillScore(ori, edit, start, cap, scorelist5);
		multi_t.join_all();

		scorelist.insert(scorelist1.begin(), scorelist1.end());
		scorelist.insert(scorelist2.begin(), scorelist2.end());
		scorelist.insert(scorelist3.begin(), scorelist3.end());
		scorelist.insert(scorelist4.begin(), scorelist4.end());
		scorelist.insert(scorelist5.begin(), scorelist5.end());
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<e_datainfo> newOri;
	vector<e_datainfo> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(e_datainfo());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(e_datainfo());
		else newEdit.push_back(edit[order.edited]);
	}

	ori = newOri;
	edit = newEdit;
}

void hkbbehaviorgraphstringdata::fillScore(vector<e_datainfo>& ori, vector<e_datainfo>& edit, usize start, usize cap, map<int, map<int, double>>& scorelist)
{
	// match scoring
	for (usize i = start; i < cap; ++i)
	{
		for (usize j = 0; j < edit.size(); ++j)
		{
			double curscore = 0;

			if (i == j)
			{
				curscore += 5;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 5;
				curscore += difference;
			}

			if (ori[i].name == edit[j].name)
			{
				curscore += 10;
				scorelist[i][j] = curscore;

				if (i + 1 == ori.size())
				{
					while (++j < edit.size())
					{
						int oriindex = i + 1;
						int newindex = j + 1;
						double difference = max(oriindex, newindex) - min(oriindex, newindex);
						difference = ((ori.size() - difference) / ori.size()) * 5;
						scorelist[i][j] = difference;
					}
				}

				break;
			}

			scorelist[i][j] = curscore;
		}
	}
}
