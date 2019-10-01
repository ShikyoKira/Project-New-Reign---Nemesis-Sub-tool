#include <boost\thread.hpp>
#include "hkbrigidbodyragdollcontrolsmodifier.h"

using namespace std;

namespace rigidbodyragdollcontrolsmodifier
{
	string key = "bs";
	string classname = "hkbRigidBodyRagdollControlsModifier";
	string signature = "0xaa87d1eb";
}

hkbrigidbodyragdollcontrolsmodifier::hkbrigidbodyragdollcontrolsmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + rigidbodyragdollcontrolsmodifier::key + to_string(functionlayer) + ">";

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

void hkbrigidbodyragdollcontrolsmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbRigidBodyRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
	}

	vecstr storeline;
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
				s_bones = line.substr(25, line.find("</hkparam>") - 25);

				if (s_bones != "null")
				{
					referencingIDs[s_bones].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbRigidBodyRagdollControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbRigidBodyRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbrigidbodyragdollcontrolsmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbRigidBodyRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
				s_bones = line.substr(25, line.find("</hkparam>") - 25);

				if (s_bones != "null")
				{
					if (!exchangeID[s_bones].empty())
					{
						int tempint = line.find(s_bones);
						s_bones = exchangeID[s_bones];
						line.replace(tempint, line.find("</hkparam>") - tempint, s_bones);
					}

					parent[s_bones] = id;
					referencingIDs[s_bones].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbRigidBodyRagdollControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbRigidBodyRagdollControlsModifier(newID: " << id << ") with hkbRigidBodyRagdollControlsModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (s_bones != "null")
		{
			referencingIDs[s_bones].pop_back();
			referencingIDs[s_bones].push_back(tempid);
			parent[s_bones] = tempid;
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
			cout << "Comparing hkbRigidBodyRagdollControlsModifier(newID: " << id << ") with hkbRigidBodyRagdollControlsModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbRigidBodyRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbrigidbodyragdollcontrolsmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbRigidBodyRagdollControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
				s_bones = line.substr(25, line.find("</hkparam>") - 25);

				if (s_bones != "null")
				{
					if (!exchangeID[s_bones].empty())
					{
						s_bones = exchangeID[s_bones];
					}

					parent[s_bones] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbRigidBodyRagdollControlsModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbRigidBodyRagdollControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbrigidbodyragdollcontrolsmodifier::GetBones()
{
	return "#" + boost::regex_replace(string(s_bones), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbrigidbodyragdollcontrolsmodifier::IsBonesNull()
{
	if (s_bones.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbrigidbodyragdollcontrolsmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbrigidbodyragdollcontrolsmodifier::IsBindingNull()
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

string hkbrigidbodyragdollcontrolsmodifier::GetAddress()
{
	return address;
}

bool hkbrigidbodyragdollcontrolsmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbrigidbodyragdollcontrolsmodifier>> hkbrigidbodyragdollcontrolsmodifierList;
safeStringUMap<shared_ptr<hkbrigidbodyragdollcontrolsmodifier>> hkbrigidbodyragdollcontrolsmodifierList_E;

void hkbrigidbodyragdollcontrolsmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbrigidbodyragdollcontrolsmodifierList_E[id] = shared_from_this() : hkbrigidbodyragdollcontrolsmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbrigidbodyragdollcontrolsmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("hierarchyGain", line, controlData.keyframehierarchycontroldata.hierarchyGain)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("velocityDamping", line, controlData.keyframehierarchycontroldata.velocityDamping)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("accelerationGain", line, controlData.keyframehierarchycontroldata.accelerationGain)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("velocityGain", line, controlData.keyframehierarchycontroldata.velocityGain)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("positionGain", line, controlData.keyframehierarchycontroldata.positionGain)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("positionMaxLinearVelocity", line, controlData.keyframehierarchycontroldata.positionMaxLinearVelocity)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("positionMaxAngularVelocity", line, controlData.keyframehierarchycontroldata.positionMaxAngularVelocity)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("snapGain", line, controlData.keyframehierarchycontroldata.snapGain)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("snapMaxLinearVelocity", line, controlData.keyframehierarchycontroldata.snapMaxLinearVelocity)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("snapMaxAngularVelocity", line, controlData.keyframehierarchycontroldata.snapMaxAngularVelocity)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("snapMaxLinearDistance", line, controlData.keyframehierarchycontroldata.snapMaxLinearDistance)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("snapMaxAngularDistance", line, controlData.keyframehierarchycontroldata.snapMaxAngularDistance)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("durationToBlend", line, controlData.durationToBlend)) ++type;

					break;
				}
				case 17:
				{
					string output;

					if (readParam("bones", line, output))
					{
						bones = (isEdited ? hkbboneindexarrayList_E : hkbboneindexarrayList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << rigidbodyragdollcontrolsmodifier::classname +" (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbrigidbodyragdollcontrolsmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + rigidbodyragdollcontrolsmodifier::key + to_string(functionlayer) + ">";
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
				hkbrigidbodyragdollcontrolsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbrigidbodyragdollcontrolsmodifierList_E[ID] = protect;
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
				hkbrigidbodyragdollcontrolsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbrigidbodyragdollcontrolsmodifierList_E[ID] = protect;
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

string hkbrigidbodyragdollcontrolsmodifier::getClassCode()
{
	return rigidbodyragdollcontrolsmodifier::key;
}

void hkbrigidbodyragdollcontrolsmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(28);
	usize base = 2;
	hkbrigidbodyragdollcontrolsmodifier* ctrpart = static_cast<hkbrigidbodyragdollcontrolsmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, rigidbodyragdollcontrolsmodifier::classname, rigidbodyragdollcontrolsmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "controlData"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(openParam(base, "keyFrameHierarchyControlData"));		// 4
	output.push_back(openObject(base));		// 5
	paramMatch("hierarchyGain", controlData.keyframehierarchycontroldata.hierarchyGain, ctrpart->controlData.keyframehierarchycontroldata.hierarchyGain, output, storeline, base, false, open, isEdited);
	paramMatch("velocityDamping", controlData.keyframehierarchycontroldata.velocityDamping, ctrpart->controlData.keyframehierarchycontroldata.velocityDamping, output, storeline, base, false, open, isEdited);
	paramMatch("accelerationGain", controlData.keyframehierarchycontroldata.accelerationGain, ctrpart->controlData.keyframehierarchycontroldata.accelerationGain, output, storeline, base, false, open, isEdited);
	paramMatch("velocityGain", controlData.keyframehierarchycontroldata.velocityGain, ctrpart->controlData.keyframehierarchycontroldata.velocityGain, output, storeline, base, false, open, isEdited);
	paramMatch("positionGain", controlData.keyframehierarchycontroldata.positionGain, ctrpart->controlData.keyframehierarchycontroldata.positionGain, output, storeline, base, false, open, isEdited);
	paramMatch("positionMaxLinearVelocity", controlData.keyframehierarchycontroldata.positionMaxLinearVelocity, ctrpart->controlData.keyframehierarchycontroldata.positionMaxLinearVelocity, output, storeline, base, false, open, isEdited);
	paramMatch("positionMaxAngularVelocity", controlData.keyframehierarchycontroldata.positionMaxAngularVelocity, ctrpart->controlData.keyframehierarchycontroldata.positionMaxAngularVelocity, output, storeline, base, false, open, isEdited);
	paramMatch("snapGain", controlData.keyframehierarchycontroldata.snapGain, ctrpart->controlData.keyframehierarchycontroldata.snapGain, output, storeline, base, false, open, isEdited);
	paramMatch("snapMaxLinearVelocity", controlData.keyframehierarchycontroldata.snapMaxLinearVelocity, ctrpart->controlData.keyframehierarchycontroldata.snapMaxLinearVelocity, output, storeline, base, false, open, isEdited);
	paramMatch("snapMaxAngularVelocity", controlData.keyframehierarchycontroldata.snapMaxAngularVelocity, ctrpart->controlData.keyframehierarchycontroldata.snapMaxAngularVelocity, output, storeline, base, false, open, isEdited);
	paramMatch("snapMaxLinearDistance", controlData.keyframehierarchycontroldata.snapMaxLinearDistance, ctrpart->controlData.keyframehierarchycontroldata.snapMaxLinearDistance, output, storeline, base, false, open, isEdited);
	paramMatch("snapMaxAngularDistance", controlData.keyframehierarchycontroldata.snapMaxAngularDistance, ctrpart->controlData.keyframehierarchycontroldata.snapMaxAngularDistance, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 5
	output.push_back(closeParam(base));		// 4
	paramMatch("durationToBlend", controlData.durationToBlend, ctrpart->controlData.durationToBlend, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("axisOfRotation", bones, ctrpart->bones, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", rigidbodyragdollcontrolsmodifier::classname, output, isEdited);
}

void hkbrigidbodyragdollcontrolsmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(28);

	output.push_back(openObject(base, ID, rigidbodyragdollcontrolsmodifier::classname, rigidbodyragdollcontrolsmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "controlData"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(openParam(base, "keyFrameHierarchyControlData"));		// 4
	output.push_back(openObject(base));		// 5
	output.push_back(autoParam(base, "hierarchyGain", controlData.keyframehierarchycontroldata.hierarchyGain));
	output.push_back(autoParam(base, "velocityDamping", controlData.keyframehierarchycontroldata.velocityDamping));
	output.push_back(autoParam(base, "accelerationGain", controlData.keyframehierarchycontroldata.accelerationGain));
	output.push_back(autoParam(base, "velocityGain", controlData.keyframehierarchycontroldata.velocityGain));
	output.push_back(autoParam(base, "positionGain", controlData.keyframehierarchycontroldata.positionGain));
	output.push_back(autoParam(base, "positionMaxLinearVelocity", controlData.keyframehierarchycontroldata.positionMaxLinearVelocity));
	output.push_back(autoParam(base, "positionMaxAngularVelocity", controlData.keyframehierarchycontroldata.positionMaxAngularVelocity));
	output.push_back(autoParam(base, "snapGain", controlData.keyframehierarchycontroldata.snapGain));
	output.push_back(autoParam(base, "snapMaxLinearVelocity", controlData.keyframehierarchycontroldata.snapMaxLinearVelocity));
	output.push_back(autoParam(base, "snapMaxAngularVelocity", controlData.keyframehierarchycontroldata.snapMaxAngularVelocity));
	output.push_back(autoParam(base, "snapMaxLinearDistance", controlData.keyframehierarchycontroldata.snapMaxLinearDistance));
	output.push_back(autoParam(base, "snapMaxAngularDistance", controlData.keyframehierarchycontroldata.snapMaxAngularDistance));
	output.push_back(closeObject(base));		// 5
	output.push_back(closeParam(base));		// 4
	output.push_back(autoParam(base, "durationToBlend", controlData.durationToBlend));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "bones", bones));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, rigidbodyragdollcontrolsmodifier::classname, output, true);
}

void hkbrigidbodyragdollcontrolsmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (bones) hkb_parent[bones] = shared_from_this();
}

void hkbrigidbodyragdollcontrolsmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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
}

void hkbrigidbodyragdollcontrolsmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbRigidBodyRagdollControlsModifierExport(string id)
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
		cout << "ERROR: Edit hkbRigidBodyRagdollControlsModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbRigidBodyRagdollControlsModifier Output Not Found (File: " << filename << ")" << endl;
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