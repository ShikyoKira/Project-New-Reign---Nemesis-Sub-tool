#include <boost\thread.hpp>
#include "hkbpoweredragdollcontrolmodifier.h"

using namespace std;

namespace poweredragdollcontrolmodifier
{
	string key = "bc";
	string classname = "hkbPoweredRagdollControlsModifier";
	string signature = "0x7cb54065";
}

hkbpoweredragdollcontrolmodifier::hkbpoweredragdollcontrolmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + poweredragdollcontrolmodifier::key + to_string(functionlayer) + ">";

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

void hkbpoweredragdollcontrolmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbPoweredRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"bones\">", 0) != string::npos)
			{
				bone = line.substr(25, line.find("</hkparam>") - 25);

				if (bone != "null")
				{
					referencingIDs[bone].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweight = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweight != "null")
				{
					referencingIDs[boneweight].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbPoweredRagdollControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbPoweredRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbpoweredragdollcontrolmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbPoweredRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"bones\">", 0) != string::npos)
			{
				bone = line.substr(25, line.find("</hkparam>") - 25);

				if (bone != "null")
				{
					if (!exchangeID[bone].empty())
					{
						int tempint = line.find(bone);
						bone = exchangeID[bone];
						line.replace(tempint, line.find("</hkparam>") - tempint, bone);
					}

					parent[bone] = id;
					referencingIDs[bone].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweight = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweight != "null")
				{
					if (!exchangeID[boneweight].empty())
					{
						int tempint = line.find(boneweight);
						boneweight = exchangeID[boneweight];
						line.replace(tempint, line.find("</hkparam>") - tempint, boneweight);
					}

					parent[boneweight] = id;
					referencingIDs[boneweight].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbPoweredRagdollControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbPoweredRagdollControlsModifier(newID: " << id << ") with hkbPoweredRagdollControlsModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		referencingIDs[bone].push_back(tempid);
		parent[bone] = tempid;

		referencingIDs[boneweight].push_back(tempid);
		parent[boneweight] = tempid;
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
			cout << "Comparing hkbPoweredRagdollControlsModifier(newID: " << id << ") with hkbPoweredRagdollControlsModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbPoweredRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbpoweredragdollcontrolmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbPoweredRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"bones\">", 0) != string::npos)
			{
				bone = line.substr(25, line.find("</hkparam>") - 25);

				if (bone != "null")
				{
					if (!exchangeID[bone].empty())
					{
						bone = exchangeID[bone];
					}

					parent[bone] = id;
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweight = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweight != "null")
				{
					if (!exchangeID[boneweight].empty())
					{
						boneweight = exchangeID[boneweight];
					}

					parent[boneweight] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbPoweredRagdollControlsModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbPoweredRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbpoweredragdollcontrolmodifier::GetBone()
{
	return "#" + boost::regex_replace(string(bone), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbpoweredragdollcontrolmodifier::IsBoneNull()
{
	if (bone.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbpoweredragdollcontrolmodifier::GetBoneWeight()
{
	return "#" + boost::regex_replace(string(boneweight), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbpoweredragdollcontrolmodifier::IsBoneWeightNull()
{
	if (boneweight.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbpoweredragdollcontrolmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbpoweredragdollcontrolmodifier::IsBindingNull()
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

string hkbpoweredragdollcontrolmodifier::GetAddress()
{
	return address;
}

bool hkbpoweredragdollcontrolmodifier::IsNegate()
{
	return IsNegated;
}

string hkbpoweredragdollcontrolmodifier::worldfrommodelmodedata::getMode()
{
	switch (mode)
	{
	case WORLD_FROM_MODEL_MODE_USE_OLD: return "WORLD_FROM_MODEL_MODE_USE_OLD";
	case WORLD_FROM_MODEL_MODE_USE_INPUT: return "WORLD_FROM_MODEL_MODE_USE_INPUT";
	case WORLD_FROM_MODEL_MODE_COMPUTE: return "WORLD_FROM_MODEL_MODE_COMPUTE";
	case WORLD_FROM_MODEL_MODE_NONE: return "WORLD_FROM_MODEL_MODE_NONE";
	case WORLD_FROM_MODEL_MODE_RAGDOLL: return "WORLD_FROM_MODEL_MODE_RAGDOLL";
	default: return "WORLD_FROM_MODEL_MODE_USE_OLD";
	}
}

safeStringUMap<shared_ptr<hkbpoweredragdollcontrolmodifier>> hkbpoweredragdollcontrolmodifierList;
safeStringUMap<shared_ptr<hkbpoweredragdollcontrolmodifier>> hkbpoweredragdollcontrolmodifierList_E;

void hkbpoweredragdollcontrolmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbpoweredragdollcontrolmodifierList_E[id] = shared_from_this() : hkbpoweredragdollcontrolmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbpoweredragdollcontrolmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("maxForce", line, controlData.maxForce)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("tau", line, controlData.tau)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("damping", line, controlData.damping)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("proportionalRecoveryVelocity", line, controlData.proportionalRecoveryVelocity)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("constantRecoveryVelocity", line, controlData.constantRecoveryVelocity)) ++type;

					break;
				}
				case 9:
				{
					string output;

					if (readParam("bones", line, output))
					{
						bones = (isEdited ? hkbboneindexarrayList_E : hkbboneindexarrayList)[output];
						++type;
					}
				}
				case 10:
				{
					if (readParam("poseMatchingBone0", line, worldFromModelModeData.poseMatchingBone0)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("poseMatchingBone1", line, worldFromModelModeData.poseMatchingBone1)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("poseMatchingBone2", line, worldFromModelModeData.poseMatchingBone2)) ++type;

					break;
				}
				case 13:
				{
					string data;

					if (readParam("mode", line, data))
					{
						if (data == "WORLD_FROM_MODEL_MODE_USE_OLD") worldFromModelModeData.mode = worldfrommodelmodedata::WORLD_FROM_MODEL_MODE_USE_OLD;
						else if (data == "WORLD_FROM_MODEL_MODE_USE_INPUT") worldFromModelModeData.mode = worldfrommodelmodedata::WORLD_FROM_MODEL_MODE_USE_INPUT;
						else if (data == "WORLD_FROM_MODEL_MODE_COMPUTE") worldFromModelModeData.mode = worldfrommodelmodedata::WORLD_FROM_MODEL_MODE_COMPUTE;
						else if (data == "WORLD_FROM_MODEL_MODE_NONE") worldFromModelModeData.mode = worldfrommodelmodedata::WORLD_FROM_MODEL_MODE_NONE;
						else worldFromModelModeData.mode = worldfrommodelmodedata::WORLD_FROM_MODEL_MODE_RAGDOLL;

						++type;
					}
				}
				case 14:
				{
					string output;

					if (readParam("boneWeights", line, output))
					{
						boneWeights = (isEdited ? hkbboneweightarrayList_E : hkbboneweightarrayList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << poweredragdollcontrolmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbpoweredragdollcontrolmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + poweredragdollcontrolmodifier::key + to_string(functionlayer) + ">";
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

			if (bones) threadedNextNode(bones, filepath, curadd, functionlayer, graphroot);

			if (boneWeights) threadedNextNode(boneWeights, filepath, curadd, functionlayer, graphroot);
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
				hkbpoweredragdollcontrolmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbpoweredragdollcontrolmodifierList_E[ID] = protect;
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
				hkbpoweredragdollcontrolmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbpoweredragdollcontrolmodifierList_E[ID] = protect;
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

string hkbpoweredragdollcontrolmodifier::getClassCode()
{
	return poweredragdollcontrolmodifier::key;
}

void hkbpoweredragdollcontrolmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(25);
	usize base = 2;
	hkbpoweredragdollcontrolmodifier* ctrpart = static_cast<hkbpoweredragdollcontrolmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, poweredragdollcontrolmodifier::classname, poweredragdollcontrolmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "controlData"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("maxForce", controlData.maxForce, ctrpart->controlData.maxForce, output, storeline, base, false, open, isEdited);
	paramMatch("tau", controlData.tau, ctrpart->controlData.tau, output, storeline, base, false, open, isEdited);
	paramMatch("damping", controlData.damping, ctrpart->controlData.damping, output, storeline, base, false, open, isEdited);
	paramMatch("proportionalRecoveryVelocity", controlData.proportionalRecoveryVelocity, ctrpart->controlData.proportionalRecoveryVelocity, output, storeline, base, false, open, isEdited);
	paramMatch("constantRecoveryVelocity", controlData.constantRecoveryVelocity, ctrpart->controlData.constantRecoveryVelocity, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("bones", bones, ctrpart->bones, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "worldFromModelModeData"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("poseMatchingBone0", worldFromModelModeData.poseMatchingBone0, ctrpart->worldFromModelModeData.poseMatchingBone0, output, storeline, base, false, open, isEdited);
	paramMatch("poseMatchingBone1", worldFromModelModeData.poseMatchingBone1, ctrpart->worldFromModelModeData.poseMatchingBone1, output, storeline, base, false, open, isEdited);
	paramMatch("poseMatchingBone2", worldFromModelModeData.poseMatchingBone2, ctrpart->worldFromModelModeData.poseMatchingBone2, output, storeline, base, false, open, isEdited);
	paramMatch("mode", worldFromModelModeData.getMode(), ctrpart->worldFromModelModeData.getMode(), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("boneWeights", boneWeights, ctrpart->boneWeights, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", poweredragdollcontrolmodifier::classname, output, isEdited);
}

void hkbpoweredragdollcontrolmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(25);

	output.push_back(openObject(base, ID, poweredragdollcontrolmodifier::classname, poweredragdollcontrolmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "controlData"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "maxForce", controlData.maxForce));
	output.push_back(autoParam(base, "tau", controlData.tau));
	output.push_back(autoParam(base, "damping", controlData.damping));
	output.push_back(autoParam(base, "proportionalRecoveryVelocity", controlData.proportionalRecoveryVelocity));
	output.push_back(autoParam(base, "constantRecoveryVelocity", controlData.constantRecoveryVelocity));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "bones", bones));
	output.push_back(openParam(base, "worldFromModelModeData"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "poseMatchingBone0", worldFromModelModeData.poseMatchingBone0));
	output.push_back(autoParam(base, "poseMatchingBone1", worldFromModelModeData.poseMatchingBone1));
	output.push_back(autoParam(base, "poseMatchingBone2", worldFromModelModeData.poseMatchingBone2));
	output.push_back(autoParam(base, "mode", worldFromModelModeData.getMode()));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "boneWeights", boneWeights));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, poweredragdollcontrolmodifier::classname, output, true);
}

void hkbpoweredragdollcontrolmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (bones) hkb_parent[bones] = shared_from_this();
	if (boneWeights) hkb_parent[boneWeights] = shared_from_this();
}

void hkbpoweredragdollcontrolmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (bones)
	{
		parentRefresh();
		bones->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (boneWeights)
	{
		parentRefresh();
		boneWeights->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbpoweredragdollcontrolmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbPoweredRagdollControlsModifierExport(string id)
{
	//stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find(storeline1[curline], 0) != string::npos) && (line.length() == storeline1[curline].length()))
			{
				if (open)
				{
					closepoint = curline;

					if (closepoint != openpoint)
					{
						storeline2.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							storeline2.push_back(storeline1[j]);
						}
					}

					storeline2.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					storeline2.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					open = true;
				}

				IsEdited = true;
			}

			storeline2.push_back(line);
			curline++;
		}
	}
	else
	{
		cout << "ERROR: Edit hkbPoweredRagdollControlsModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(stoi(id.substr(1)), storeline2);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream output(filename);

		if (output.is_open())
		{
			FunctionWriter fwrite(&output);

			for (unsigned int i = 0; i < storeline2.size(); i++)
			{
				fwrite << storeline2[i] << "\n";
			}

			output.close();
		}
		else
		{
			cout << "ERROR: Edit hkbPoweredRagdollControlsModifier Output Not Found (File: " << filename << ")" << endl;
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
