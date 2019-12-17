#include <boost\thread.hpp>
#include "hkbmanualselectorgenerator.h"
#include "highestscore.h"

using namespace std;

namespace manualselectorgenerator
{
	const string key = "m";
	const string classname = "hkbManualSelectorGenerator";
	const string signature = "0xd932fab8";
}

string hkbmanualselectorgenerator::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbmanualselectorgenerator>> hkbmanualselectorgeneratorList;
safeStringUMap<shared_ptr<hkbmanualselectorgenerator>> hkbmanualselectorgeneratorList_E;

void hkbmanualselectorgenerator::regis(string id, bool isEdited)
{
	isEdited ? hkbmanualselectorgeneratorList_E[id] = shared_from_this() : hkbmanualselectorgeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbmanualselectorgenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					usize numelement;

					if (readEleParam("states", line, numelement))
					{
						generators.reserve(numelement);
						++type;
					}

					break;
				}
				case 4:
				{
					vecstr reflist;

					if (!getNodeRefList(line, reflist))
					{
						if (readParam("selectedGeneratorIndex", line, selectedGeneratorIndex)) ++type;

						break;
					}

					for (auto& ref : reflist)
					{
						generators.emplace_back(isEdited ? hkbgeneratorList_E[ref] : hkbgeneratorList[ref]);
					}

					break;
				}
				case 5:
				{
					if (readParam("currentGeneratorIndex", line, currentGeneratorIndex)) ++type;
				}
			}
		}
		if (type == 0)
		{
			if (line.find("<hkparam name=\"") != string::npos)
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
					itr != boost::sregex_iterator(); ++itr)
				{
					string header = itr->str(1);

					if (header == "variableBindingSet") variableBindingSet = isEdited ? hkbvariablebindingsetList_E[itr->str(2)] : hkbvariablebindingsetList[itr->str(2)];
					else if (header == "userData") userData = stoi(itr->str(2));
					else if (header == "name")
					{
						++type;
						name = itr->str(2);
					}

					break;
				}
			}
		}
		else if (type == 1)
		{
			if (line.find("<hkparam name=\"generators\" numelements=\"") != string::npos)
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"generators\" numelements=\"([0-9]+)\">(<\\/hkparam>|)"));
					itr != boost::sregex_iterator(); ++itr)
				{
					if (itr->str(2).length() == 0) generators.reserve(stoi(itr->str(1)));

					++type;
					break;
				}
			}
		}
		else
		{
			if (line.find("<hkparam name=\"") != string::npos)
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
					itr != boost::sregex_iterator(); ++itr)
				{
					string header = itr->str(1);

					if (header == "selectedGeneratorIndex") selectedGeneratorIndex = stoi(itr->str(2));
					else if (header == "currentGeneratorIndex") currentGeneratorIndex = stoi(itr->str(2));

					break;
				}
			}
			else
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("(?<!>)(#[0-9]+)")); itr != boost::sregex_iterator(); ++itr)
				{
					generators.emplace_back(isEdited ? hkbgeneratorList_E[itr->str(1)] : hkbgeneratorList[itr->str(1)]);
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << manualselectorgenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + manualselectorgenerator::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + manualselectorgenerator::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			boost::thread_group multithreads;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				for (usize i = 0; i < generators.size(); ++i)
				{
					multithreads.create_thread(boost::bind(&hkbmanualselectorgenerator::threadedNextNode, this, generators[i], filepath, curadd + to_string(i), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
			}
			else
			{
				for (usize i = 0; i < generators.size(); ++i)
				{
					threadedNextNode(generators[i], filepath, curadd + to_string(i), functionlayer, graphroot);
				}
			}

			if (multithreads.size() > 0)
			{
				multithreads.join_all();
				num_thread -= multithreads.size();
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
				hkbmanualselectorgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];

				hkbmanualselectorgeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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
				hkbmanualselectorgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbmanualselectorgeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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

string hkbmanualselectorgenerator::getClassCode()
{
	return manualselectorgenerator::key;
}

void hkbmanualselectorgenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(100);
	usize base = 2;
	hkbmanualselectorgenerator* ctrpart = static_cast<hkbmanualselectorgenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, manualselectorgenerator::classname, manualselectorgenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);

	usize size = generators.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "generators", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "generators", size));		// 2

	matchScoring(generators, ctrpart->generators, ID);
	size = generators.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (generators[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->generators[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->generators[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->generators[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, generators[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(generators[i], ctrpart->generators[i], output, storeline, base, false, open, isEdited);
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

	paramMatch("selectedGeneratorIndex", selectedGeneratorIndex, ctrpart->selectedGeneratorIndex, output, storeline, base, false, open, isEdited);
	paramMatch("currentGeneratorIndex", currentGeneratorIndex, ctrpart->currentGeneratorIndex, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", manualselectorgenerator::classname, output, isEdited);
}

void hkbmanualselectorgenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, manualselectorgenerator::classname, manualselectorgenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));

	usize size = generators.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "generators", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "generators", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(generators[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "selectedGeneratorIndex", selectedGeneratorIndex));
	output.push_back(autoParam(base, "currentGeneratorIndex", currentGeneratorIndex));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, manualselectorgenerator::classname, output, true);
}

void hkbmanualselectorgenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& generator : generators)
	{
		hkb_parent[generator] = shared_from_this();
	}
}

void hkbmanualselectorgenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto& generator : generators)
		{
			parentRefresh();
			generator->connect(filepath, address + to_string(tempint), 0, true, graphroot);

			if (IsForeign.find(generator->ID) == IsForeign.end()) ++tempint;
			else
			{
				auto branch_itr = IsBranchOrigin.find(generator->ID);

				if (branch_itr != IsBranchOrigin.end())
				{
					++tempint;
					IsBranchOrigin.erase(branch_itr);
				}
			}
		}
	}
	else
	{
		for (auto& generator : generators)
		{
			parentRefresh();
			generator->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbmanualselectorgenerator::matchScoring(vector<shared_ptr<hkbgenerator>>& ori, vector<shared_ptr<hkbgenerator>>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<hkbgenerator>> newOri;
		vector<shared_ptr<hkbgenerator>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<hkbgenerator>());
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

			if (i == j)
			{
				scorelist[i][j] += 5;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 5;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<hkbgenerator>> newOri;
	vector<shared_ptr<hkbgenerator>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<hkbgenerator>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<hkbgenerator>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbmanualselectorgenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
