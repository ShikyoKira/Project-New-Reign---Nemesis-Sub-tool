#include <boost\thread.hpp>
#include "hkbblendergeneratorchild.h"

using namespace std;

namespace blendergeneratorchild
{
	const string key = "h";
	const string classname = "hkbBlenderGeneratorChild";
	const string signature = "0xe2b384b0";
}

string hkbblendergeneratorchild::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbblendergeneratorchild>> hkbblendergeneratorchildList;
safeStringUMap<shared_ptr<hkbblendergeneratorchild>> hkbblendergeneratorchildList_E;

void hkbblendergeneratorchild::regis(string id, bool isEdited)
{
	isEdited ? hkbblendergeneratorchildList_E[id] = shared_from_this() : hkbblendergeneratorchildList[id] = shared_from_this();
	ID = id;
}

void hkbblendergeneratorchild::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("generator", line, output))
					{
						generator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 2:
				{
					string output;

					if (readParam("boneWeights", line, output))
					{
						boneWeights = (isEdited ? hkbboneweightarrayList_E : hkbboneweightarrayList)[output];
						++type;
					}

					break;
				}
				case 3:
				{
					if (readParam("weight", line, weight)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("worldFromModelWeight", line, worldFromModelWeight)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << blendergeneratorchild::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbblendergeneratorchild::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + blendergeneratorchild::key + to_string(functionlayer) + ">";
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

			if (generator) threadedNextNode(generator, filepath, curadd, functionlayer, graphroot);

			if (boneWeights) threadedNextNode(boneWeights, filepath, curadd, functionlayer, graphroot);
		}
		else
		{
			// existed
			// if (IsOldFunction(filepath, shared_from_this(), address)) // is this new function or old
			if (ID != ID) // pre-foreign, will go through IsOldFunction using generator
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbblendergeneratorchildList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbblendergeneratorchildList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				bool isOld = true;

				if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
				{
					isOld = false;
					address = preaddress;
				}

				IsForeign[ID] = true;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, isOld, graphroot);
			}
		}
	}
	else if (!compare)
	{
		CrossReferencing(shared_from_this(), address, functionlayer, compare);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
	{
		bool isOld = true;

		if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
		{
			isOld = false;
			address = preaddress;
		}

		nextNode(filepath, functionlayer, isOld, graphroot);
	}
}

string hkbblendergeneratorchild::getClassCode()
{
	return blendergeneratorchild::key;
}

void hkbblendergeneratorchild::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(6);
	output.reserve(6);
	usize base = 2;
	hkbblendergeneratorchild* ctrpart = static_cast<hkbblendergeneratorchild*>(counterpart.get());

	output.push_back(openObject(base, ID, blendergeneratorchild::classname, blendergeneratorchild::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("generator", generator, ctrpart->generator, output, storeline, base, false, open, isEdited);
	paramMatch("boneWeights", boneWeights, ctrpart->boneWeights, output, storeline, base, false, open, isEdited);
	paramMatch("weight", weight, ctrpart->weight, output, storeline, base, false, open, isEdited);
	paramMatch("worldFromModelWeight", worldFromModelWeight, ctrpart->worldFromModelWeight, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", blendergeneratorchild::classname, output, isEdited);
}

void hkbblendergeneratorchild::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(6);

	output.push_back(openObject(base, ID, blendergeneratorchild::classname, blendergeneratorchild::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "generator", generator));
	output.push_back(autoParam(base, "boneWeights", boneWeights));
	output.push_back(autoParam(base, "weight", weight));
	output.push_back(autoParam(base, "worldFromModelWeight", worldFromModelWeight));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, blendergeneratorchild::classname, output, true);
}

void hkbblendergeneratorchild::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (generator) hkb_parent[generator] = shared_from_this();
	if (boneWeights) hkb_parent[boneWeights] = shared_from_this();
}

void hkbblendergeneratorchild::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (generator)
	{
		if (isOld && IsExist.find(generator->ID) != IsExist.end() && IsForeign.find(generator->ID) == IsForeign.end())
		{
			auto add_ptr = addressID.find(address);

			if (add_ptr != addressID.end())
			{
				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbblendergeneratorchildList_E.erase(ID);
				editedBehavior.erase(ID);
				IsForeign.erase(ID);
				if (IsExist.find(ID) != IsExist.end()) IsExist.erase(ID);
				ID = add_ptr->second;
				hkbblendergeneratorchildList_E[ID] = protect;
				editedBehavior[ID] = protect;
			}
		}

		parentRefresh();
		generator->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (boneWeights)
	{
		parentRefresh();
		boneWeights->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbblendergeneratorchild::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
