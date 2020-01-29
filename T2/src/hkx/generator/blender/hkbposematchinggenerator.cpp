#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>

#include "hkbposematchinggenerator.h"
#include "generatorlines.h"
#include "src/utilities/hkMap.h"

using namespace std;

namespace posematchinggenerator
{
	const string key = "cj";
	const string classname = "hkbPoseMatchingGenerator";
	const string signature = "0x29e271b4";

	hkMap<string, hkbposematchinggenerator::Mode> modeMap
	{
		{ "MODE_MATCH", hkbposematchinggenerator::MODE_MATCH},
		{ "MODE_PLAY", hkbposematchinggenerator::MODE_PLAY},
	};
}

string hkbposematchinggenerator::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbposematchinggenerator>> hkbposematchinggeneratorList;
safeStringUMap<shared_ptr<hkbposematchinggenerator>> hkbposematchinggeneratorList_E;

void hkbposematchinggenerator::regis(string id, bool isEdited)
{
	isEdited ? hkbposematchinggeneratorList_E[id] = shared_from_this() : hkbposematchinggeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbposematchinggenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos || line.find("	#") != string::npos)
		{
			switch(type)
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
					if (readParam("referencePoseWeightThreshold", line, referencePoseWeightThreshold)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("blendParameter", line, blendParameter)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("minCyclicBlendParameter", line, minCyclicBlendParameter)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("maxCyclicBlendParameter", line, maxCyclicBlendParameter)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("indexOfSyncMasterChild", line, indexOfSyncMasterChild)) ++type;

					break;
				}
				case 8:
				{
					int output;

					if (readParam("flags", line, output))
					{
						flags = static_cast<blenderflags>(output);
						++type;
					}

					break;
				}
				case 9:
				{
					if (readParam("subtractLastChild", line, subtractLastChild)) ++type;

					break;
				}
				case 10:
				{
					usize numelement;

					if (readEleParam("children", line, numelement))
					{
						children.reserve(numelement);
						++type;
					}

					break;
				}
				case 11:
				{
					vecstr reflist;

					if (!getNodeRefList(line, reflist))
					{
						if (readParam("worldFromModelRotation", line, worldFromModelRotation)) ++type;

						break;
					}

					for (auto& ref : reflist)
					{
						children.emplace_back((isEdited ? hkbblendergeneratorchildList_E : hkbblendergeneratorchildList)[ref]);
					}

					break;
				}
				case 12:
				{
					if (readParam("blendSpeed", line, blendSpeed)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("minSpeedToSwitch", line, minSpeedToSwitch)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("minSwitchTimeNoError", line, minSwitchTimeNoError)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("minSwitchTimeFullError", line, minSwitchTimeFullError)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("startPlayingEventId", line, startPlayingEventId)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("startMatchingEventId", line, startMatchingEventId)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("rootBoneIndex", line, rootBoneIndex)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("otherBoneIndex", line, otherBoneIndex)) ++type;

					break;
				}
				case 20:
				{
					if (readParam("anotherBoneIndex", line, anotherBoneIndex)) ++type;

					break;
				}
				case 21:
				{
					if (readParam("pelvisIndex", line, pelvisIndex)) ++type;

					break;
				}
				case 22:
				{
					string output;

					if (readParam("mode", line, output))
					{
						mode = output == "MODE_MATCH" ? MODE_MATCH : MODE_PLAY;
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << posematchinggenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbposematchinggenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + posematchinggenerator::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);
	startPlayingEventId.connectEventInfo(ID, graphroot);
	startMatchingEventId.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + posematchinggenerator::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				boost::thread_group multithreads;

				for (usize i = 0; i < children.size(); ++i)
				{
					multithreads.create_thread(boost::bind(&hkbposematchinggenerator::threadedNextNode, this, children[i], filepath, curadd + to_string(i), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
				multithreads.join_all();
				num_thread -= multithreads.size();
			}
			else
			{
				for (usize i = 0; i < children.size(); ++i)
				{
					threadedNextNode(children[i], filepath, curadd + to_string(i), functionlayer, graphroot);
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

				auto protect = shared_from_this();
				hkbposematchinggeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbposematchinggeneratorList_E[ID] = protect;
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

				auto protect = shared_from_this();
				hkbposematchinggeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbposematchinggeneratorList_E[ID] = protect;
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

string hkbposematchinggenerator::getClassCode()
{
	return posematchinggenerator::key;
}

void hkbposematchinggenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(50);
	usize base = 2;
	hkbposematchinggenerator* ctrpart = static_cast<hkbposematchinggenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, posematchinggenerator::classname, posematchinggenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("referencePoseWeightThreshold", referencePoseWeightThreshold, ctrpart->referencePoseWeightThreshold, output, storeline, base, false, open, isEdited);
	paramMatch("blendParameter", blendParameter, ctrpart->blendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("minCyclicBlendParameter", minCyclicBlendParameter, ctrpart->minCyclicBlendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("maxCyclicBlendParameter", maxCyclicBlendParameter, ctrpart->maxCyclicBlendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("indexOfSyncMasterChild", indexOfSyncMasterChild, ctrpart->indexOfSyncMasterChild, output, storeline, base, false, open, isEdited);
	paramMatch("flags", static_cast<int>(flags), static_cast<int>(ctrpart->flags), output, storeline, base, false, open, isEdited);
	paramMatch("subtractLastChild", subtractLastChild, ctrpart->subtractLastChild, output, storeline, base, true, open, isEdited);

	usize size = children.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "children", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "children", size));		// 2

	matchScoring(children, ctrpart->children, ID, posematchinggenerator::classname);
	size = children.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (children[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->children[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->children[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->children[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, children[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(children[i], ctrpart->children[i], output, storeline, base, false, open, isEdited);
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

	paramMatch("worldFromModelRotation", worldFromModelRotation, ctrpart->worldFromModelRotation, output, storeline, base, false, open, isEdited);
	paramMatch("blendSpeed", blendSpeed, ctrpart->blendSpeed, output, storeline, base, false, open, isEdited);
	paramMatch("minSpeedToSwitch", minSpeedToSwitch, ctrpart->minSpeedToSwitch, output, storeline, base, false, open, isEdited);
	paramMatch("minSwitchTimeNoError", minSwitchTimeNoError, ctrpart->minSwitchTimeNoError, output, storeline, base, false, open, isEdited);
	paramMatch("minSwitchTimeFullError", minSwitchTimeFullError, ctrpart->minSwitchTimeFullError, output, storeline, base, false, open, isEdited);
	paramMatch("startPlayingEventId", startPlayingEventId, ctrpart->startPlayingEventId, output, storeline, base, false, open, isEdited);
	paramMatch("startMatchingEventId", startMatchingEventId, ctrpart->startMatchingEventId, output, storeline, base, false, open, isEdited);
	paramMatch("rootBoneIndex", rootBoneIndex, ctrpart->rootBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("otherBoneIndex", otherBoneIndex, ctrpart->otherBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("anotherBoneIndex", anotherBoneIndex, ctrpart->anotherBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("pelvisIndex", pelvisIndex, ctrpart->pelvisIndex, output, storeline, base, false, open, isEdited);
	paramMatch("mode", getMode(), ctrpart->getMode(), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", posematchinggenerator::classname, output, isEdited);
}

void hkbposematchinggenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(50);

	output.push_back(openObject(base, ID, posematchinggenerator::classname, posematchinggenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "referencePoseWeightThreshold", referencePoseWeightThreshold));
	output.push_back(autoParam(base, "blendParameter", blendParameter));
	output.push_back(autoParam(base, "minCyclicBlendParameter", minCyclicBlendParameter));
	output.push_back(autoParam(base, "maxCyclicBlendParameter", maxCyclicBlendParameter));
	output.push_back(autoParam(base, "indexOfSyncMasterChild", indexOfSyncMasterChild));
	output.push_back(autoParam(base, "flags", static_cast<int>(flags)));
	output.push_back(autoParam(base, "subtractLastChild", subtractLastChild));

	usize size = children.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "children", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "children", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(children[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "worldFromModelRotation", worldFromModelRotation));
	output.push_back(autoParam(base, "blendSpeed", blendSpeed));
	output.push_back(autoParam(base, "minSpeedToSwitch", minSpeedToSwitch));
	output.push_back(autoParam(base, "minSwitchTimeNoError", minSwitchTimeNoError));
	output.push_back(autoParam(base, "minSwitchTimeFullError", minSwitchTimeFullError));
	output.push_back(autoParam(base, "startPlayingEventId", startPlayingEventId));
	output.push_back(autoParam(base, "startMatchingEventId", startMatchingEventId));
	output.push_back(autoParam(base, "rootBoneIndex", rootBoneIndex));
	output.push_back(autoParam(base, "otherBoneIndex", otherBoneIndex));
	output.push_back(autoParam(base, "anotherBoneIndex", anotherBoneIndex));
	output.push_back(autoParam(base, "pelvisIndex", pelvisIndex));
	output.push_back(autoParam(base, "mode", getMode()));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, posematchinggenerator::classname, output, true);
}

void hkbposematchinggenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto child : children)
	{
		hkb_parent[child] = shared_from_this();
	}
}

void hkbposematchinggenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto child : children)
		{
			parentRefresh();
			child->connect(filepath, address + to_string(tempint), 0, true, graphroot);

			if (IsForeign.find(child->ID) == IsForeign.end()) ++tempint;
			else
			{
				auto branch_itr = IsBranchOrigin.find(child->ID);

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
		for (auto child : children)
		{
			parentRefresh();
			child->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

string hkbposematchinggenerator::getMode()
{
	return posematchinggenerator::modeMap[mode];
}

void hkbposematchinggenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

