#include <boost/thread.hpp>

#include "hkbbehaviorgraph.h"
#include "src/utilities/hkMap.h"

using namespace std;

boost::atomic_flag tracklock = BOOST_ATOMIC_FLAG_INIT;

namespace behaviorgraph
{
	const string key = "g";
	const string classname = "hkbBehaviorGraph";
	const string signature = "0xb1218f86";

	hkMap<string, hkbbehaviorgraph::variablemode> modeMap =
	{
		{ "VARIABLE_MODE_DISCARD_WHEN_INACTIVE", hkbbehaviorgraph::VARIABLE_MODE_DISCARD_WHEN_INACTIVE },
		{ "VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE", hkbbehaviorgraph::VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE },
	};
}

string hkbbehaviorgraph::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbbehaviorgraph>> hkbbehaviorgraphList;
safeStringUMap<shared_ptr<hkbbehaviorgraph>> hkbbehaviorgraphList_E;

void hkbbehaviorgraph::regis(string id, bool isEdited)
{
	isEdited ? hkbbehaviorgraphList_E[id] = shared_from_this() : hkbbehaviorgraphList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();

	while (tracklock.test_and_set(boost::memory_order_acquire));
	datapacktracker.push_back(stoi(id.substr(1)));
	tracklock.clear(boost::memory_order_release);

	ID = id;
}

void hkbbehaviorgraph::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("variableMode", line, output))
					{
						variableMode = behaviorgraph::modeMap[output];
					}

					break;
				}
				case 4:
				{
					string output;

					if (readParam("rootGenerator", line, output))
					{
						rootGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("data", line, output))
					{
						data = (isEdited ? hkbbehaviorgraphdataList_E : hkbbehaviorgraphdataList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << behaviorgraph::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbbehaviorgraph::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + behaviorgraph::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (rootGenerator) threadedNextNode(rootGenerator, filepath, curadd, functionlayer, graphroot);

			if (data) threadedNextNode(data, filepath, curadd, functionlayer, graphroot);
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
				hkbbehaviorgraphList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);

				while (tracklock.test_and_set(boost::memory_order_acquire));
				vector<usize> newdatapacktracker;
				newdatapacktracker.reserve(datapacktracker.size() - 1);

				{
					usize tempIntID = stoi(ID.substr(1));

					for (auto& tracker : datapacktracker)
					{
						if (tracker != tempIntID) newdatapacktracker.push_back(tracker);
					}
				}

				datapacktracker = newdatapacktracker;
				tracklock.clear(boost::memory_order_release);

				ID = addressID[address];
				hkbbehaviorgraphList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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
				hkbbehaviorgraphList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbbehaviorgraphList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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

string hkbbehaviorgraph::getClassCode()
{
	return behaviorgraph::key;
}

void hkbbehaviorgraph::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(8);
	output.reserve(8);
	usize base = 2;
	hkbbehaviorgraph* ctrpart = static_cast<hkbbehaviorgraph*>(counterpart.get());

	output.push_back(openObject(base, ID, behaviorgraph::classname, behaviorgraph::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("variableMode", getVariableMode(), ctrpart->getVariableMode(), output, storeline, base, false, open, isEdited);
	paramMatch("rootGenerator", rootGenerator, ctrpart->rootGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("data", data, ctrpart->data, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", behaviorgraph::classname, output, isEdited);
}

void hkbbehaviorgraph::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(8);

	output.push_back(openObject(base, ID, behaviorgraph::classname, behaviorgraph::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "variableMode", getVariableMode()));
	output.push_back(autoParam(base, "rootGenerator", rootGenerator));
	output.push_back(autoParam(base, "data", data));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, behaviorgraph::classname, output, true);
}

void hkbbehaviorgraph::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (rootGenerator) hkb_parent[rootGenerator] = shared_from_this();
	if (data) hkb_parent[data] = shared_from_this();
}

void hkbbehaviorgraph::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, this);
	}

	if (rootGenerator)
	{
		parentRefresh();
		rootGenerator->connect(filepath, address, functionlayer, true, this);
	}

	if (data)
	{
		parentRefresh();
		data->connect(filepath, address, functionlayer, true, this);
	}
}

string hkbbehaviorgraph::getVariableMode()
{
	return behaviorgraph::modeMap[variableMode];
}

void hkbbehaviorgraph::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, this);
}
