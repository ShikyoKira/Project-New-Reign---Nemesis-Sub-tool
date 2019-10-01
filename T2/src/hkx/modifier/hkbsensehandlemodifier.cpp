#include <boost\thread.hpp>
#include "hkbsensehandlemodifier.h"
#include "highestscore.h"

using namespace std;

namespace sensehandlemodifier
{
	string key = "al";
	string classname = "hkbSenseHandleModifier";
	string signature = "0x2a064d99";
}

hkbsensehandlemodifier::hkbsensehandlemodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + sensehandlemodifier::key + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id);
		}
		else
		{
			nonCompare(filepath, id);
		}
	}
	else if (!Error)
	{
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare);

		if (compare)
		{
			if (statusChange)
			{
				Dummy(dummyID);
			}

			if (IsForeign[id])
			{
				address = preaddress;
			}
			else if (!statusChange)
			{
				IsNegated = true;
			}
		}
		else
		{
			IsNegated = true;
		}
	}
	else
	{
		return;
	}
}

void hkbsensehandlemodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbSenseHandleModifier (ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload = line.substr(29, line.find("</hkparam>") - 29);

				if (payload != "null")
				{
					referencingIDs[payload].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbSenseHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbSenseHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbsensehandlemodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbSenseHandleModifier(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						int tempint = line.find(variablebindingset);
						variablebindingset = exchangeID[variablebindingset];
						line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
					}

					parent[variablebindingset] = id;
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload = line.substr(29, line.find("</hkparam>") - 29);

				if (payload != "null")
				{
					if (!exchangeID[payload].empty())
					{
						int tempint = line.find(payload);
						payload = exchangeID[payload];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload);
					}

					parent[payload] = id;
					referencingIDs[payload].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbSenseHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbSenseHandleModifier(newID: " << id << ") with hkbSenseHandleModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (payload != "null")
		{
			referencingIDs[payload].pop_back();
			referencingIDs[payload].push_back(tempid);
			parent[payload] = tempid;
		}

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbSenseHandleModifier(newID: " << id << ") with hkbSenseHandleModifier(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbSenseHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbsensehandlemodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbSenseHandleModifier(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						variablebindingset = exchangeID[variablebindingset];
					}

					parent[variablebindingset] = id;
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload = line.substr(29, line.find("</hkparam>") - 29);

				if (payload != "null")
				{
					if (!exchangeID[payload].empty())
					{
						payload = exchangeID[payload];
					}

					parent[payload] = id;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbSenseHandleModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbSenseHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbsensehandlemodifier::GetPayload()
{
	return "#" + boost::regex_replace(string(payload), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbsensehandlemodifier::IsPayloadNull()
{
	if (payload.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbsensehandlemodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbsensehandlemodifier::IsBindingNull()
{
	if (variablebindingset.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbsensehandlemodifier::GetAddress()
{
	return address;
}

bool hkbsensehandlemodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbsensehandlemodifier>> hkbsensehandlemodifierList;
safeStringUMap<shared_ptr<hkbsensehandlemodifier>> hkbsensehandlemodifierList_E;

void hkbsensehandlemodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbsensehandlemodifierList_E[id] = shared_from_this() : hkbsensehandlemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbsensehandlemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("sensorLocalOffset", line, sensorLocalOffset)) ++type;

					break;
				}
				case 5:
				{
					usize numelement;

					if (readEleParam("ranges", line, numelement))
					{
						ranges.reserve(numelement);
						++type;
					}

					break;
				}
				case 6:
				{
					int output;

					if (readParam("id", line, output))
					{
						ranges.push_back(output);
						++type;
						break;
					}

					string s_output;

					if (readParam("handleOut", line, s_output))
					{
						handleOut = (isEdited ? hkbhandleList_E : hkbhandleList)[s_output];
						type = 11;
					}

					break;
				}
				case 7:
				{
					string output;

					if (readParam("payload", line, output))
					{
						ranges.back().event.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 8:
				{
					if (readParam("minDistance", line, ranges.back().minDistance)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("maxDistance", line, ranges.back().maxDistance)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("ignoreHandle", line, ranges.back().ignoreHandle)) type = 6;

					break;
				}
				case 11:
				{
					string output;

					if (readParam("handleIn", line, output))
					{
						handleIn = (isEdited ? hkbhandleList_E : hkbhandleList)[output];
						++type;
					}

					break;
				}
				case 12:
				{
					if (readParam("localFrameName", line, localFrameName)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("sensorLocalFrameName", line, sensorLocalFrameName)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("minDistance", line, minDistance)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("maxDistance", line, maxDistance)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("distanceOut", line, distanceOut)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("collisionFilterInfo", line, collisionFilterInfo)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("sensorRagdollBoneIndex", line, sensorRagdollBoneIndex)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("sensorAnimationBoneIndex", line, sensorAnimationBoneIndex)) ++type;

					break;
				}
				case 20:
				{
					string data;

					if (readParam("sensingMode", line, data))
					{
						if (data == "SENSE_IN_NEARBY_RIGID_BODIES") sensingMode = SENSE_IN_NEARBY_RIGID_BODIES;
						else if (data == "SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER") sensingMode = SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER;
						else if (data == "SENSE_IN_OTHER_CHARACTER_RIGID_BODIES") sensingMode = SENSE_IN_OTHER_CHARACTER_RIGID_BODIES;
						else if (data == "SENSE_IN_THIS_CHARACTER_RIGID_BODIES") sensingMode = SENSE_IN_THIS_CHARACTER_RIGID_BODIES;
						else if (data == "SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES") sensingMode = SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES;
						else if (data == "SENSE_IN_GIVEN_RIGID_BODY") sensingMode = SENSE_IN_GIVEN_RIGID_BODY;
						else if (data == "SENSE_IN_OTHER_CHARACTER_SKELETON") sensingMode = SENSE_IN_OTHER_CHARACTER_SKELETON;
						else if (data == "SENSE_IN_THIS_CHARACTER_SKELETON") sensingMode = SENSE_IN_THIS_CHARACTER_SKELETON;
						else if (data == "SENSE_IN_GIVEN_CHARACTER_SKELETON") sensingMode = SENSE_IN_GIVEN_CHARACTER_SKELETON;
						else sensingMode = SENSE_IN_GIVEN_LOCAL_FRAME_GROUP;

						++type;
					}

					break;
				}
				case 21:
				{
					if (readParam("extrapolateSensorPosition", line, extrapolateSensorPosition)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("keepFirstSensedHandle", line, keepFirstSensedHandle)) ++type;

					break;
				}
				case 23:
				{
					if (readParam("foundHandleOut", line, foundHandleOut)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << sensehandlemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbsensehandlemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + sensehandlemodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& range : ranges)
	{
		range.event.id.connectEventInfo(ID, graphroot);
	}

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

			for (auto& range : ranges)
			{
				if (range.event.payload) threadedNextNode(range.event.payload, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
			}

			if (handleOut) threadedNextNode(handleOut, filepath, curadd, functionlayer, graphroot);

			if (handleIn) threadedNextNode(handleIn, filepath, curadd, functionlayer, graphroot);
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
				hkbsensehandlemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbsensehandlemodifierList_E[ID] = protect;
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
				hkbsensehandlemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbsensehandlemodifierList_E[ID] = protect;
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

string hkbsensehandlemodifier::getClassCode()
{
	return sensehandlemodifier::key;
}

void hkbsensehandlemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbsensehandlemodifier* ctrpart = static_cast<hkbsensehandlemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, sensehandlemodifier::classname, sensehandlemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("sensorLocalOffset", sensorLocalOffset, ctrpart->sensorLocalOffset, output, storeline, base, false, open, isEdited);

	usize size = ranges.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "ranges", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "ranges", size));		// 2

	matchScoring(ranges, ctrpart->ranges, ID);
	size = ranges.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (ranges[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(openParam(base, "event"));
				output.push_back(openObject(base));		// 4
				output.push_back(autoParam(base, "id", ctrpart->ranges[i].event.id));
				output.push_back(autoParam(base, "payload", ctrpart->ranges[i].event.payload));
				output.push_back(closeObject(base));		// 4
				output.push_back(autoParam(base, "minDistance", ctrpart->ranges[i].minDistance));
				output.push_back(autoParam(base, "maxDistance", ctrpart->ranges[i].maxDistance));
				output.push_back(autoParam(base, "ignoreHandle", ctrpart->ranges[i].ignoreHandle));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->ranges[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->ranges[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(openParam(base, "event"));
				storeline.push_back(openObject(base));		// 4
				storeline.push_back(autoParam(base, "id", ranges[i].event.id));
				storeline.push_back(autoParam(base, "payload", ranges[i].event.payload));
				storeline.push_back(closeObject(base));		// 4
				storeline.push_back(autoParam(base, "minDistance", ranges[i].minDistance));
				storeline.push_back(autoParam(base, "maxDistance", ranges[i].maxDistance));
				storeline.push_back(autoParam(base, "ignoreHandle", ranges[i].ignoreHandle));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 10)
				{
					output.push_back("");	// 10 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			output.push_back(openParam(base, "event"));
			output.push_back(openObject(base));		// 4
			paramMatch("id", ranges[i].event.id, ctrpart->ranges[i].event.id, output, storeline, base, false, open, isEdited);
			paramMatch("payload", ranges[i].event.payload, ctrpart->ranges[i].event.payload, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 4
			paramMatch("minDistance", ranges[i].minDistance, ctrpart->ranges[i].minDistance, output, storeline, base, false, open, isEdited);
			paramMatch("maxDistance", ranges[i].maxDistance, ctrpart->ranges[i].maxDistance, output, storeline, base, false, open, isEdited);
			paramMatch("ignoreHandle", ranges[i].ignoreHandle, ctrpart->ranges[i].ignoreHandle, output, storeline, base, true, open, isEdited);
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


	paramMatch("handleOut", handleOut, ctrpart->handleOut, output, storeline, base, false, open, isEdited);
	paramMatch("handleIn", handleIn, ctrpart->handleIn, output, storeline, base, false, open, isEdited);
	paramMatch("localFrameName", localFrameName, ctrpart->localFrameName, output, storeline, base, false, open, isEdited);
	paramMatch("sensorLocalFrameName", sensorLocalFrameName, ctrpart->sensorLocalFrameName, output, storeline, base, false, open, isEdited);
	paramMatch("minDistance", minDistance, ctrpart->minDistance, output, storeline, base, false, open, isEdited);
	paramMatch("maxDistance", maxDistance, ctrpart->maxDistance, output, storeline, base, false, open, isEdited);
	paramMatch("distanceOut", distanceOut, ctrpart->distanceOut, output, storeline, base, false, open, isEdited);
	paramMatch("collisionFilterInfo", collisionFilterInfo, ctrpart->collisionFilterInfo, output, storeline, base, false, open, isEdited);
	paramMatch("sensorRagdollBoneIndex", sensorRagdollBoneIndex, ctrpart->sensorRagdollBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("sensorAnimationBoneIndex", sensorAnimationBoneIndex, ctrpart->sensorAnimationBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("sensingMode", getSensingMode(), ctrpart->getSensingMode(), output, storeline, base, false, open, isEdited);
	paramMatch("extrapolateSensorPosition", extrapolateSensorPosition, ctrpart->extrapolateSensorPosition, output, storeline, base, false, open, isEdited);
	paramMatch("keepFirstSensedHandle", keepFirstSensedHandle, ctrpart->keepFirstSensedHandle, output, storeline, base, false, open, isEdited);
	paramMatch("foundHandleOut", foundHandleOut, ctrpart->foundHandleOut, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", sensehandlemodifier::classname, output, isEdited);
}

void hkbsensehandlemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, sensehandlemodifier::classname, sensehandlemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "sensorLocalOffset", sensorLocalOffset));

	usize size = ranges.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "ranges", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "ranges", size));		// 2

	for (auto& range : ranges)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(openParam(base, "event"));
		output.push_back(openObject(base));		// 4
		output.push_back(autoParam(base, "id", range.event.id));
		output.push_back(autoParam(base, "payload", range.event.payload));
		output.push_back(closeObject(base));		// 4
		output.push_back(autoParam(base, "minDistance", range.minDistance));
		output.push_back(autoParam(base, "maxDistance", range.maxDistance));
		output.push_back(autoParam(base, "ignoreHandle", range.ignoreHandle));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "handleOut", handleOut));
	output.push_back(autoParam(base, "handleIn", handleIn));
	output.push_back(autoParam(base, "localFrameName", localFrameName));
	output.push_back(autoParam(base, "sensorLocalFrameName", sensorLocalFrameName));
	output.push_back(autoParam(base, "minDistance", minDistance));
	output.push_back(autoParam(base, "maxDistance", maxDistance));
	output.push_back(autoParam(base, "distanceOut", distanceOut));
	output.push_back(autoParam(base, "collisionFilterInfo", collisionFilterInfo));
	output.push_back(autoParam(base, "sensorRagdollBoneIndex", sensorRagdollBoneIndex));
	output.push_back(autoParam(base, "sensorAnimationBoneIndex", sensorAnimationBoneIndex));
	output.push_back(autoParam(base, "sensingMode", getSensingMode()));
	output.push_back(autoParam(base, "extrapolateSensorPosition", extrapolateSensorPosition));
	output.push_back(autoParam(base, "keepFirstSensedHandle", keepFirstSensedHandle));
	output.push_back(autoParam(base, "foundHandleOut", foundHandleOut));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, sensehandlemodifier::classname, output, true);
}

void hkbsensehandlemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& range : ranges)
	{
		if (range.event.payload) hkb_parent[range.event.payload] = shared_from_this();
	}

	if (handleOut) hkb_parent[handleOut] = shared_from_this();
	if (handleIn) hkb_parent[handleIn] = shared_from_this();
}

void hkbsensehandlemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto& range : ranges)
		{
			if (range.event.payload)
			{
				parentRefresh();
				range.event.payload->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(range.event.payload->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& range : ranges)
		{
			if (range.event.payload)
			{
				parentRefresh();
				range.event.payload->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}

	if (handleOut)
	{
		parentRefresh();
		handleOut->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (handleIn)
	{
		parentRefresh();
		handleIn->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbsensehandlemodifier::matchScoring(vector<range>& ori, vector<range>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<range> newOri;
		vector<range> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(range());
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

			if (ori[i].event.id == edit[j].event.id)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].event.payload->ID == edit[j].event.payload->ID)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].minDistance == edit[j].minDistance)
			{
				++scorelist[i][j];
			}

			if (ori[i].maxDistance == edit[j].maxDistance)
			{
				++scorelist[i][j];
			}

			if (ori[i].ignoreHandle == edit[j].ignoreHandle)
			{
				++scorelist[i][j];
			}

			if (i == j)
			{
				scorelist[i][j] += 2;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 2;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<range> newOri;
	vector<range> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(range());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(range());
		else newEdit.push_back(edit[order.edited]);
	}

	ori = newOri;
	edit = newEdit;
}

string hkbsensehandlemodifier::getSensingMode()
{
	switch (sensingMode)
	{
		case SENSE_IN_NEARBY_RIGID_BODIES: return "SENSE_IN_NEARBY_RIGID_BODIES";
		case SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER: return "SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER";
		case SENSE_IN_OTHER_CHARACTER_RIGID_BODIES: return "SENSE_IN_OTHER_CHARACTER_RIGID_BODIES";
		case SENSE_IN_THIS_CHARACTER_RIGID_BODIES: return "SENSE_IN_THIS_CHARACTER_RIGID_BODIES";
		case SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES: return "SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES";
		case SENSE_IN_GIVEN_RIGID_BODY: return "SENSE_IN_GIVEN_RIGID_BODY";
		case SENSE_IN_OTHER_CHARACTER_SKELETON: return "SENSE_IN_OTHER_CHARACTER_SKELETON";
		case SENSE_IN_THIS_CHARACTER_SKELETON: return "SENSE_IN_THIS_CHARACTER_SKELETON";
		case SENSE_IN_GIVEN_CHARACTER_SKELETON: return "SENSE_IN_GIVEN_CHARACTER_SKELETON";
		case SENSE_IN_GIVEN_LOCAL_FRAME_GROUP: return "SENSE_IN_GIVEN_LOCAL_FRAME_GROUP";
		default: return "SENSE_IN_NEARBY_RIGID_BODIES";
	}
}

void hkbsensehandlemodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbSenseHandleModifierExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbSenseHandleModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbSenseHandleModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 identifying edits
	vecstr output;
	bool IsEdited = false;
	bool IsChanged = false;
	bool open = false;
	int curline = 0;
	int part = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if ((storeline1[curline].find("<hkparam name=\"ranges\" numelements=", 0) == string::npos) && (part == 0)) // pre ranges info
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}

				output.push_back(storeline2[i]);
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}

				output.push_back(storeline2[i]);
			}

			curline++;

			if (storeline1[curline].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos)
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
		}
		else if ((storeline1[curline].find("<hkparam name=\"handleOut\">", 0) == string::npos) && (part == 1))
		{
			if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkparam name=\"handleOut\">", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}
			}
			else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
			{
				if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					output.push_back("				<hkobject>");
				}

				if (open)
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}

				output.push_back(storeline2[i]);
			}
			else
			{
				output.push_back(storeline2[i]);
			}

			curline++;

			if ((open) && (storeline2[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos))
			{
				closepoint = curline;
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			if (storeline2[i].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
			{
				output.push_back("				</hkobject>");
			}
		}
		else if (part == 2) // existing leftover settings
		{
			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}

				output.push_back(storeline2[i]);
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}

				output.push_back(storeline2[i]);
			}

			curline++;
		}
		else // added variable value
		{
			if (storeline2[i].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

					if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("handleOut", 0) == string::npos))
					{
						IsChanged = true;
						openpoint = curline;
						open = true;
					}
					else
					{
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(storeline1[curline]);
						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					IsEdited = true;
				}
				else
				{
					output.push_back(storeline2[i]);
				}

				part = 1;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"handleOut\">", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}

				output.push_back(storeline2[i]);
				part = 2;
				curline++;
			}
			else
			{
				if (part == 1)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					if (storeline2[i].find("<hkparam name=\"id\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
						output.push_back(storeline2[i]);
					}
					else if (storeline2[i].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
					{
						output.push_back(storeline2[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					if (storeline2[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos)
					{
						if (IsChanged)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < curline; j++)
							{
								if ((storeline1[j].find("<hkparam name=\"id\">", 0) != string::npos) && (output.back().find("ORIGINAL", 0) == string::npos))
								{
									output.push_back("				<hkobject>");
								}

								output.push_back(storeline1[j]);

								if (storeline1[j].find("<hkparam name=\"ignoreHandle\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}

							}

							IsChanged = false;
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}
		}
	}

	if (open) // close unclosed edits
	{
		if (IsChanged)
		{
			closepoint = curline;

			if (closepoint != openpoint)
			{
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
			}

			IsChanged = false;
		}

		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	bool closeOri = false;
	bool closeEdit = false;

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);
			part = 0;

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (i < output.size() - 1)
				{
					if ((output[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
				}

				if (part == 0)
				{
					if (output[i].find("<hkparam name=\"ranges\" numelements=", 0) != string::npos)
					{
						if (output[i - 1].find("OPEN", 0) != string::npos)
						{
							if (output[i].find("</hkparam>", 0) != string::npos)
							{
								closeEdit = true;
							}
							else
							{
								closeEdit = false;
							}
						}
						else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
						{
							if (output[i].find("</hkparam>", 0) != string::npos)
							{
								closeOri = true;
							}
							else
							{
								closeOri = false;
							}
						}
						else
						{
							if (output[i].find("</hkparam>", 0) != string::npos)
							{
								closeEdit = true;
								closeOri = true;
							}
							else
							{
								closeEdit = false;
								closeOri = false;
							}
						}
					}

					fwrite << output[i] << "\n";

					if (output[i + 1].find("<hkparam name=\"handleOut\">", 0) != string::npos)
					{
						part = 1;
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("handleOut", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
					}
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbSenseHandleModifier Output Not Found (File: " << filename << ")" << endl;
			Error = true;
		}
	}
	else
	{
		if (IsFileExist(filename))
		{
			if (remove(filename.c_str()) != 0)
			{
				perror("Error deleting file");
				Error = true;
			}
		}
	}
}