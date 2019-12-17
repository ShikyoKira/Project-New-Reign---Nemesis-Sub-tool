#include <boost\thread.hpp>
#include "hkbmodifierlist.h"
#include "generatorlines.h"

using namespace std;

namespace modifierlist
{
	const string key = "y";
	const string classname = "hkbModifierList";
	const string signature = "0xa4180ca1";
}

string hkbmodifierlist::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbmodifierlist>> hkbmodifierlistList;
safeStringUMap<shared_ptr<hkbmodifierlist>> hkbmodifierlistList_E;

void hkbmodifierlist::regis(string id, bool isEdited)
{
	isEdited ? hkbmodifierlistList_E[id] = shared_from_this() : hkbmodifierlistList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbmodifierlist::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos || line.find("	#") != string::npos)
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
					usize numelement;

					if (readEleParam("modifiers", line, numelement))
					{
						modifiers.reserve(numelement);
						++type;
					}

					break; 
				}
				case 5:
				{
					vecstr reflist;

					if (getNodeRefList(line, reflist))
					{
						for (auto& ref : reflist)
						{
							modifiers.emplace_back((isEdited ? hkbmodifierList_E : hkbmodifierList)[ref]);
						}
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << modifierlist::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbmodifierlist::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + modifierlist::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + modifierlist::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			boost::thread_group multithreads;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				for (usize i = 0; i < modifiers.size(); ++i)
				{
					multithreads.create_thread(boost::bind(&hkbmodifierlist::threadedNextNode, this, modifiers[i], filepath, curadd + to_string(i), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
				multithreads.join_all();
				num_thread -= multithreads.size();
			}
			else
			{
				for (usize i = 0; i < modifiers.size(); ++i)
				{
					threadedNextNode(modifiers[i], filepath, curadd + to_string(i), functionlayer, graphroot);
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
				hkbmodifierlistList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbmodifierlistList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
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
				hkbmodifierlistList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbmodifierlistList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
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

string hkbmodifierlist::getClassCode()
{
	return modifierlist::key;
}

void hkbmodifierlist::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbmodifierlist* ctrpart = static_cast<hkbmodifierlist*>(counterpart.get());

	output.push_back(openObject(base, ID, modifierlist::classname, modifierlist::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);

	usize size = modifiers.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "modifiers", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "modifiers", size));		// 2

	matchScoring(modifiers, ctrpart->modifiers, ID);
	size = modifiers.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (modifiers[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->modifiers[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->modifiers[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->modifiers[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, modifiers[i]));
				++i;
				output.push_back("");	// 1 space
			}
		}
		// both exist
		else
		{
			voidMatch(modifiers[i], ctrpart->modifiers[i], output, storeline, base, false, open, isEdited);
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", modifierlist::classname, output, isEdited);
}

void hkbmodifierlist::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, modifierlist::classname, modifierlist::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));

	usize size = modifiers.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "modifiers", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "modifiers", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(modifiers[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, modifierlist::classname, output, true);
}

void hkbmodifierlist::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& modifier : modifiers)
	{
		hkb_parent[modifier] = shared_from_this();
	}
}

void hkbmodifierlist::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) functionlayer = 0;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (isOld)
	{
		usize tempint = 0;

		for (auto& modifier : modifiers)
		{
			parentRefresh();
			modifier->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

			if (IsForeign.find(modifier->ID) == IsForeign.end()) ++tempint;
		}
	}
	else
	{
		for (auto& modifier : modifiers)
		{
			parentRefresh();
			modifier->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbmodifierlist::matchScoring(vector<shared_ptr<hkbmodifier>>& ori, vector<shared_ptr<hkbmodifier>>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<hkbmodifier>> newOri;
		vector<shared_ptr<hkbmodifier>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<hkbmodifier>());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}
	else if (edit.size() == 0)
	{
		vector<shared_ptr<hkbmodifier>> newOri;
		vector<shared_ptr<hkbmodifier>> newEdit;
		newOri.reserve(ori.size());
		newEdit.reserve(ori.size());

		for (auto& each : ori)
		{
			newOri.push_back(each);
			newEdit.push_back(make_shared<hkbmodifier>());
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

			if (ori[i]->ID == edit[j]->ID)
			{
				scorelist[i][j] += 10;
			}

			if (i == j)
			{
				scorelist[i][j] += 8;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 8;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<hkbmodifier>> newOri;
	vector<shared_ptr<hkbmodifier>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<hkbmodifier>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<hkbmodifier>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbmodifierlist::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
