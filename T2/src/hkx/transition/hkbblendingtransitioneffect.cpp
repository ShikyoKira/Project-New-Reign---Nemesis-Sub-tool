#include <boost\algorithm\string.hpp>
#include "hkbblendingtransitioneffect.h"
#include "blendingOldFunction.h"

using namespace std;

namespace blendingtransitioneffect
{
	const string key = "j";
	const string classname = "hkbBlendingTransitionEffect";
	const string signature = "0xfd8584fe";
}

string hkbblendingtransitioneffect::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbblendingtransitioneffect>> hkbblendingtransitioneffectList;
safeStringUMap<shared_ptr<hkbblendingtransitioneffect>> hkbblendingtransitioneffectList_E;

void hkbblendingtransitioneffect::regis(string id, bool isEdited)
{
	isEdited ? hkbblendingtransitioneffectList_E[id] = shared_from_this() : hkbblendingtransitioneffectList[id] = shared_from_this();
	isEdited ? hkbtransitioneffectList_E[id] = shared_from_this() : hkbtransitioneffectList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbblendingtransitioneffect::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("duration", line, duration)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("toGeneratorStartTimeFraction", line, toGeneratorStartTimeFraction)) ++type;

					break;
				}
				case 7:
				{
					string output;

					if (readParam("flags", line, output))
					{
						vecstr list;
						boost::trim_if(output, boost::is_any_of("\t "));
						boost::split(list, output, boost::is_any_of("|"), boost::token_compress_on);

						for (auto& each : list)
						{
							flags.update(each);
						}

						++type;
					}

					break;
				}
				case 8:
				{
					string data;

					if (readParam("endMode", line, data))
					{
						if (data == "END_MODE_NONE") endMode = END_MODE_NONE;
						else if (data == "END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR") endMode = END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR;
						else endMode = END_MODE_CAP_DURATION_AT_END_OF_FROM_GENERATOR;

						++type;
					}

					break;
				}
				case 9:
				{
					string data;

					if (readParam("blendCurve", line, data))
					{
						if (data == "BLEND_CURVE_SMOOTH") blendCurve = BLEND_CURVE_SMOOTH;
						else if (data == "BLEND_CURVE_LINEAR") blendCurve = BLEND_CURVE_LINEAR;
						else if (data == "BLEND_CURVE_LINEAR_TO_SMOOTH") blendCurve = BLEND_CURVE_LINEAR_TO_SMOOTH;
						else blendCurve = BLEND_CURVE_SMOOTH_TO_LINEAR;

						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << blendingtransitioneffect::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbblendingtransitioneffect::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + blendingtransitioneffect::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) variableBindingSet->connect(filepath, curadd, functionlayer + 1, false, graphroot);
		}
		else
		{
			// existed
			if (blendingOldFunction(shared_from_this(), address, functionlayer))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbblendingtransitioneffectList_E.erase(ID);
				hkbtransitioneffectList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbblendingtransitioneffectList_E[ID] = protect;
				hkbtransitioneffectList_E[ID] = protect;
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
				hkbblendingtransitioneffectList_E.erase(ID);
				hkbtransitioneffectList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbblendingtransitioneffectList_E[ID] = protect;
				hkbtransitioneffectList_E[ID] = protect;
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

string hkbblendingtransitioneffect::getClassCode()
{
	return blendingtransitioneffect::key;
}

void hkbblendingtransitioneffect::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbblendingtransitioneffect* ctrpart = static_cast<hkbblendingtransitioneffect*>(counterpart.get());

	output.push_back(openObject(base, ID, blendingtransitioneffect::classname, blendingtransitioneffect::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("selfTransitionMode", getSelfTransitionMode(), ctrpart->getSelfTransitionMode(), output, storeline, base, false, open, isEdited);
	paramMatch("eventMode", getEventMode(), ctrpart->getEventMode(), output, storeline, base, false, open, isEdited);
	paramMatch("duration", duration, ctrpart->duration, output, storeline, base, false, open, isEdited);
	paramMatch("toGeneratorStartTimeFraction", toGeneratorStartTimeFraction, ctrpart->toGeneratorStartTimeFraction, output, storeline, base, false, open, isEdited);
	paramMatch("flags", flags.getflags(), ctrpart->flags.getflags(), output, storeline, base, false, open, isEdited);
	paramMatch("endMode", getEndMode(), ctrpart->getEndMode(), output, storeline, base, false, open, isEdited);
	paramMatch("blendCurve", getBlendCurve(blendCurve), getBlendCurve(ctrpart->blendCurve), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", blendingtransitioneffect::classname, output, isEdited);
}

void hkbblendingtransitioneffect::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(12);

	output.push_back(openObject(base, ID, blendingtransitioneffect::classname, blendingtransitioneffect::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "selfTransitionMode", getSelfTransitionMode()));
	output.push_back(autoParam(base, "eventMode", getEventMode()));
	output.push_back(autoParam(base, "duration", duration));
	output.push_back(autoParam(base, "toGeneratorStartTimeFraction", toGeneratorStartTimeFraction));
	output.push_back(autoParam(base, "flags", flags.getflags()));
	output.push_back(autoParam(base, "endMode", getEndMode()));
	output.push_back(autoParam(base, "blendCurve", getBlendCurve(blendCurve)));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, blendingtransitioneffect::classname, output, true);
}

void hkbblendingtransitioneffect::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbblendingtransitioneffect::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

string hkbblendingtransitioneffect::getEndMode()
{
	switch (endMode)
	{
		case END_MODE_NONE: return "END_MODE_NONE";
		case END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR: return "END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR";
		case END_MODE_CAP_DURATION_AT_END_OF_FROM_GENERATOR: return "END_MODE_CAP_DURATION_AT_END_OF_FROM_GENERATOR";
		default: return "END_MODE_NONE";
	}
}

string hkbblendingtransitioneffect::flagbits::getflags()
{
	string flags;

	if (FLAG_IGNORE_TO_WORLD_FROM_MODEL) flags.append("FLAG_IGNORE_TO_WORLD_FROM_MODEL|");
	if (FLAG_SYNC) flags.append("FLAG_SYNC|");
	if (FLAG_IGNORE_FROM_WORLD_FROM_MODEL) flags.append("FLAG_IGNORE_FROM_WORLD_FROM_MODEL|");
	if (FLAG_NONE) flags.append("FLAG_NONE|");

	if (flags.length() == 0) return "0";

	flags.pop_back();
	return flags;
}

void hkbblendingtransitioneffect::flagbits::update(string flag)
{
	if (flag == "FLAG_NONE") FLAG_NONE = true;
	else if (flag == "FLAG_IGNORE_FROM_WORLD_FROM_MODEL") FLAG_IGNORE_FROM_WORLD_FROM_MODEL = true;
	else if (flag == "FLAG_SYNC") FLAG_SYNC = true;
	else if (flag == "FLAG_IGNORE_TO_WORLD_FROM_MODEL") FLAG_IGNORE_TO_WORLD_FROM_MODEL = true;
}
