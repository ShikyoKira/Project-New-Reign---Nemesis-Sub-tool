#include <boost\thread.hpp>
#include "hkbgeneratortransitioneffect.h"

using namespace std;

namespace generatortransitioneffect
{
	const string key = "cn";
	const string classname = "hkbGeneratorTransitionEffect";
	const string signature = "0x5f771b12";
}

safeStringUMap<shared_ptr<hkbgeneratortransitioneffect>> hkbgeneratortransitioneffectList;
safeStringUMap<shared_ptr<hkbgeneratortransitioneffect>> hkbgeneratortransitioneffectList_E;

void hkbgeneratortransitioneffect::regis(string id, bool isEdited)
{
	isEdited ? hkbgeneratortransitioneffectList_E[id] = shared_from_this() : hkbgeneratortransitioneffectList[id] = shared_from_this();
	isEdited ? hkbtransitioneffectList_E[id] = shared_from_this() : hkbtransitioneffectList[id] = shared_from_this();
	ID = id;
}

void hkbgeneratortransitioneffect::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string data;

					if (readParam("selfTransitionMode", line, data))
					{
						if (data == "SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC") selfTransitionMode = SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC;
						else if (data == "SELF_TRANSITION_MODE_CONTINUE") selfTransitionMode = SELF_TRANSITION_MODE_CONTINUE;
						else if (data == "SELF_TRANSITION_MODE_RESET") selfTransitionMode = SELF_TRANSITION_MODE_RESET;
						else selfTransitionMode = SELF_TRANSITION_MODE_BLEND;

						++type;
					}

					break;
				}
				case 4:
				{
					string data;

					if (readParam("eventMode", line, data))
					{
						if (data == "EVENT_MODE_DEFAULT") eventMode = EVENT_MODE_DEFAULT;
						else if (data == "EVENT_MODE_PROCESS_ALL") eventMode = EVENT_MODE_PROCESS_ALL;
						else if (data == "EVENT_MODE_IGNORE_FROM_GENERATOR") eventMode = EVENT_MODE_IGNORE_FROM_GENERATOR;
						else eventMode = EVENT_MODE_IGNORE_TO_GENERATOR;

						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("transitionGenerator", line, output))
					{
						transitionGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("blendInDuration", line, blendInDuration)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("blendOutDuration", line, blendOutDuration)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("syncToGeneratorStartTime", line, syncToGeneratorStartTime)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("timeInTransition", line, timeInTransition)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("duration", line, duration)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("effectiveBlendInDuration", line, effectiveBlendInDuration)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("effectiveBlendOutDuration", line, effectiveBlendOutDuration)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("toGeneratorState", line, toGeneratorState)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("echoTransitionGenerator", line, echoTransitionGenerator)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("echoToGenerator", line, echoToGenerator)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("justActivated", line, justActivated)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("updateActiveNodes", line, updateActiveNodes)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("stage", line, stage)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << generatortransitioneffect::key + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbgeneratortransitioneffect::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + generatortransitioneffect::key + to_string(functionlayer) + ">";
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

			if (transitionGenerator) threadedNextNode(transitionGenerator, filepath, curadd, functionlayer, graphroot);
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
				hkbgeneratortransitioneffectList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbgeneratortransitioneffectList_E[ID] = protect;
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
				hkbgeneratortransitioneffectList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbgeneratortransitioneffectList_E[ID] = protect;
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

string hkbgeneratortransitioneffect::getClassCode()
{
	return generatortransitioneffect::key;
}

void hkbgeneratortransitioneffect::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbgeneratortransitioneffect* ctrpart = static_cast<hkbgeneratortransitioneffect*>(counterpart.get());

	output.push_back(openObject(base, ID, generatortransitioneffect::classname, generatortransitioneffect::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("selfTransitionMode", getSelfTransitionMode(), ctrpart->getSelfTransitionMode(), output, storeline, base, false, open, isEdited);
	paramMatch("eventMode", getEventMode(), ctrpart->getEventMode(), output, storeline, base, false, open, isEdited);
	paramMatch("transitionGenerator", transitionGenerator, ctrpart->transitionGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("blendInDuration", blendInDuration, ctrpart->blendInDuration, output, storeline, base, false, open, isEdited);
	paramMatch("blendOutDuration", blendOutDuration, ctrpart->blendOutDuration, output, storeline, base, false, open, isEdited);
	paramMatch("syncToGeneratorStartTime", syncToGeneratorStartTime, ctrpart->syncToGeneratorStartTime, output, storeline, base, false, open, isEdited);
	paramMatch("timeInTransition", timeInTransition, ctrpart->timeInTransition, output, storeline, base, false, open, isEdited);
	paramMatch("duration", duration, ctrpart->duration, output, storeline, base, false, open, isEdited);
	paramMatch("effectiveBlendInDuration", effectiveBlendInDuration, ctrpart->effectiveBlendInDuration, output, storeline, base, false, open, isEdited);
	paramMatch("effectiveBlendOutDuration", effectiveBlendOutDuration, ctrpart->effectiveBlendOutDuration, output, storeline, base, false, open, isEdited);
	paramMatch("toGeneratorState", toGeneratorState, ctrpart->toGeneratorState, output, storeline, base, false, open, isEdited);
	paramMatch("echoTransitionGenerator", echoTransitionGenerator, ctrpart->echoTransitionGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("echoToGenerator", echoToGenerator, ctrpart->echoToGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("justActivated", justActivated, ctrpart->justActivated, output, storeline, base, false, open, isEdited);
	paramMatch("updateActiveNodes", updateActiveNodes, ctrpart->updateActiveNodes, output, storeline, base, false, open, isEdited);
	paramMatch("stage", stage, ctrpart->stage, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", generatortransitioneffect::classname, output, isEdited);
}

void hkbgeneratortransitioneffect::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(21);

	output.push_back(openObject(base, ID, generatortransitioneffect::classname, generatortransitioneffect::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "selfTransitionMode", getSelfTransitionMode()));
	output.push_back(autoParam(base, "eventMode", getEventMode()));
	output.push_back(autoParam(base, "transitionGenerator", transitionGenerator));
	output.push_back(autoParam(base, "blendInDuration", blendInDuration));
	output.push_back(autoParam(base, "blendOutDuration", blendOutDuration));
	output.push_back(autoParam(base, "syncToGeneratorStartTime", syncToGeneratorStartTime));
	output.push_back(autoParam(base, "timeInTransition", timeInTransition));
	output.push_back(autoParam(base, "duration", duration));
	output.push_back(autoParam(base, "effectiveBlendInDuration", effectiveBlendInDuration));
	output.push_back(autoParam(base, "effectiveBlendOutDuration", effectiveBlendOutDuration));
	output.push_back(autoParam(base, "toGeneratorState", toGeneratorState));
	output.push_back(autoParam(base, "echoTransitionGenerator", echoTransitionGenerator));
	output.push_back(autoParam(base, "echoToGenerator", echoToGenerator));
	output.push_back(autoParam(base, "justActivated", justActivated));
	output.push_back(autoParam(base, "updateActiveNodes", updateActiveNodes));
	output.push_back(autoParam(base, "stage", stage));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, generatortransitioneffect::classname, output, true);
}

void hkbgeneratortransitioneffect::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (transitionGenerator) hkb_parent[transitionGenerator] = shared_from_this();
}

void hkbgeneratortransitioneffect::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (transitionGenerator)
	{
		parentRefresh();
		transitionGenerator->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbgeneratortransitioneffect::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
