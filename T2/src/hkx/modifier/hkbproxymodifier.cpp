#include "hkbproxymodifier.h"

using namespace std;

namespace proxymodifier
{
	string key = "ai";
	string classname = "hkbProxyModifier";
	string signature = "0x8a41554f";
}

hkbproxymodifier::hkbproxymodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + proxymodifier::key + to_string(functionlayer) + ">";

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

void hkbproxymodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbProxyModifier(ID: " << id << ") has been initialized!" << endl;
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
		}
	}
	else
	{
		cout << "ERROR: hkbProxyModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbProxyModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbproxymodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbProxyModifier(ID: " << id << ") has been initialized!" << endl;
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

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbProxyModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbProxyModifier(newID: " << id << ") with hkbProxyModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
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
			cout << "Comparing hkbProxyModifier(newID: " << id << ") with hkbProxyModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbProxyModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbproxymodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbProxyModifier(ID: " << id << ") has been initialized!" << endl;
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

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbProxyModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbProxyModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbproxymodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbproxymodifier::IsBindingNull()
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

string hkbproxymodifier::GetAddress()
{
	return address;
}

bool hkbproxymodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbproxymodifier>> hkbproxymodifierList;
safeStringUMap<shared_ptr<hkbproxymodifier>> hkbproxymodifierList_E;

void hkbproxymodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbproxymodifierList_E[id] = shared_from_this() : hkbproxymodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbproxymodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("dynamicFriction", line, proxyInfo.dynamicFriction)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("staticFriction", line, proxyInfo.staticFriction)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("keepContactTolerance", line, proxyInfo.keepContactTolerance)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("up", line, proxyInfo.up)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("keepDistance", line, proxyInfo.keepDistance)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("contactAngleSensitivity", line, proxyInfo.contactAngleSensitivity)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("userPlanes", line, proxyInfo.userPlanes)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("maxCharacterSpeedForSolver", line, proxyInfo.maxCharacterSpeedForSolver)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("characterStrength", line, proxyInfo.characterStrength)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("characterMass", line, proxyInfo.characterMass)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("maxSlope", line, proxyInfo.maxSlope)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("penetrationRecoverySpeed", line, proxyInfo.penetrationRecoverySpeed)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("maxCastIterations", line, proxyInfo.maxCastIterations)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("refreshManifoldInCheckSupport", line, proxyInfo.refreshManifoldInCheckSupport)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("linearVelocity", line, linearVelocity)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("horizontalGain", line, horizontalGain)) ++type;

					break;
				}
				case 20:
				{
					if (readParam("verticalGain", line, verticalGain)) ++type;

					break;
				}
				case 21:
				{
					if (readParam("maxHorizontalSeparation", line, maxHorizontalSeparation)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("limitHeadingDegrees", line, limitHeadingDegrees)) ++type;

					break;
				}
				case 23:
				{
					if (readParam("maxVerticalSeparation", line, maxVerticalSeparation)) ++type;

					break;
				}
				case 24:
				{
					if (readParam("verticalDisplacementError", line, verticalDisplacementError)) ++type;

					break;
				}
				case 25:
				{
					if (readParam("verticalDisplacementErrorGain", line, verticalDisplacementErrorGain)) ++type;

					break;
				}
				case 26:
				{
					if (readParam("maxVerticalDisplacement", line, maxVerticalDisplacement)) ++type;

					break;
				}
				case 27:
				{
					if (readParam("minVerticalDisplacement", line, minVerticalDisplacement)) ++type;

					break;
				}
				case 28:
				{
					if (readParam("capsuleHeight", line, capsuleHeight)) ++type;

					break;
				}
				case 29:
				{
					if (readParam("capsuleRadius", line, capsuleRadius)) ++type;

					break;
				}
				case 30:
				{
					if (readParam("maxSlopeForRotation", line, maxSlopeForRotation)) ++type;

					break;
				}
				case 31:
				{
					if (readParam("collisionFilterInfo", line, collisionFilterInfo)) ++type;

					break;
				}
				case 32:
				{
					string output;

					if (readParam("phantomType", line, output))
					{
						phantomType = output == "PHANTOM_TYPE_SIMPLE" ? PHANTOM_TYPE_SIMPLE : PHANTOM_TYPE_CACHING;
						++type;
					}
				}
				case 33:
				{
					string output;

					if (readParam("linearVelocityMode", line, output))
					{
						linearVelocityMode = output == "LINEAR_VELOCITY_MODE_WORLD" ? LINEAR_VELOCITY_MODE_WORLD : LINEAR_VELOCITY_MODE_MODEL;
						++type;
					}
				}
				case 34:
				{
					if (readParam("ignoreIncomingRotation", line, ignoreIncomingRotation)) ++type;

					break;
				}
				case 35:
				{
					if (readParam("ignoreCollisionDuringRotation", line, ignoreCollisionDuringRotation)) ++type;

					break;
				}
				case 36:
				{
					if (readParam("ignoreIncomingTranslation", line, ignoreIncomingTranslation)) ++type;

					break;
				}
				case 37:
				{
					if (readParam("includeDownwardMomentum", line, includeDownwardMomentum)) ++type;

					break;
				}
				case 38:
				{
					if (readParam("followWorldFromModel", line, followWorldFromModel)) ++type;

					break;
				}
				case 39:
				{
					if (readParam("isTouchingGround", line, isTouchingGround)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << proxymodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbproxymodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + proxymodifier::key + to_string(functionlayer) + ">";
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
			if (IsOldFunction(filepath, shared_from_this(), address))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbproxymodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbproxymodifierList_E[ID] = protect;
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
				hkbproxymodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbproxymodifierList_E[ID] = protect;
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

string hkbproxymodifier::getClassCode()
{
	return proxymodifier::key;
}

void hkbproxymodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(45);
	usize base = 2;
	hkbproxymodifier* ctrpart = static_cast<hkbproxymodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, proxymodifier::classname, proxymodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "proxyInfo"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("dynamicFriction", proxyInfo.dynamicFriction, ctrpart->proxyInfo.dynamicFriction, output, storeline, base, false, open, isEdited);
	paramMatch("staticFriction", proxyInfo.staticFriction, ctrpart->proxyInfo.staticFriction, output, storeline, base, false, open, isEdited);
	paramMatch("keepContactTolerance", proxyInfo.keepContactTolerance, ctrpart->proxyInfo.keepContactTolerance, output, storeline, base, false, open, isEdited);
	paramMatch("up", proxyInfo.up, ctrpart->proxyInfo.up, output, storeline, base, false, open, isEdited);
	paramMatch("keepDistance", proxyInfo.keepDistance, ctrpart->proxyInfo.keepDistance, output, storeline, base, false, open, isEdited);
	paramMatch("contactAngleSensitivity", proxyInfo.contactAngleSensitivity, ctrpart->proxyInfo.contactAngleSensitivity, output, storeline, base, false, open, isEdited);
	paramMatch("userPlanes", proxyInfo.userPlanes, ctrpart->proxyInfo.userPlanes, output, storeline, base, false, open, isEdited);
	paramMatch("maxCharacterSpeedForSolver", proxyInfo.maxCharacterSpeedForSolver, ctrpart->proxyInfo.maxCharacterSpeedForSolver, output, storeline, base, false, open, isEdited);
	paramMatch("characterStrength", proxyInfo.characterStrength, ctrpart->proxyInfo.characterStrength, output, storeline, base, false, open, isEdited);
	paramMatch("characterMass", proxyInfo.characterMass, ctrpart->proxyInfo.characterMass, output, storeline, base, false, open, isEdited);
	paramMatch("maxSlope", proxyInfo.maxSlope, ctrpart->proxyInfo.maxSlope, output, storeline, base, false, open, isEdited);
	paramMatch("penetrationRecoverySpeed", proxyInfo.penetrationRecoverySpeed, ctrpart->proxyInfo.penetrationRecoverySpeed, output, storeline, base, false, open, isEdited);
	paramMatch("maxCastIterations", proxyInfo.maxCastIterations, ctrpart->proxyInfo.maxCastIterations, output, storeline, base, false, open, isEdited);
	paramMatch("characterMass", proxyInfo.characterMass, ctrpart->proxyInfo.characterMass, output, storeline, base, false, open, isEdited);
	paramMatch("refreshManifoldInCheckSupport", proxyInfo.refreshManifoldInCheckSupport, ctrpart->proxyInfo.refreshManifoldInCheckSupport, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("linearVelocity", linearVelocity, ctrpart->linearVelocity, output, storeline, base, false, open, isEdited);
	paramMatch("horizontalGain", horizontalGain, ctrpart->horizontalGain, output, storeline, base, false, open, isEdited);
	paramMatch("verticalGain", verticalGain, ctrpart->verticalGain, output, storeline, base, false, open, isEdited);
	paramMatch("maxHorizontalSeparation", maxHorizontalSeparation, ctrpart->maxHorizontalSeparation, output, storeline, base, false, open, isEdited);
	paramMatch("limitHeadingDegrees", limitHeadingDegrees, ctrpart->limitHeadingDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("maxVerticalSeparation", maxVerticalSeparation, ctrpart->maxVerticalSeparation, output, storeline, base, false, open, isEdited);
	paramMatch("verticalDisplacementError", verticalDisplacementError, ctrpart->verticalDisplacementError, output, storeline, base, false, open, isEdited);
	paramMatch("verticalDisplacementErrorGain", verticalDisplacementErrorGain, ctrpart->verticalDisplacementErrorGain, output, storeline, base, false, open, isEdited);
	paramMatch("maxVerticalDisplacement", maxVerticalDisplacement, ctrpart->maxVerticalDisplacement, output, storeline, base, false, open, isEdited);
	paramMatch("minVerticalDisplacement", minVerticalDisplacement, ctrpart->minVerticalDisplacement, output, storeline, base, false, open, isEdited);
	paramMatch("capsuleHeight", capsuleHeight, ctrpart->capsuleHeight, output, storeline, base, false, open, isEdited);
	paramMatch("capsuleRadius", capsuleRadius, ctrpart->capsuleRadius, output, storeline, base, false, open, isEdited);
	paramMatch("maxSlopeForRotation", maxSlopeForRotation, ctrpart->maxSlopeForRotation, output, storeline, base, false, open, isEdited);
	paramMatch("collisionFilterInfo", collisionFilterInfo, ctrpart->collisionFilterInfo, output, storeline, base, false, open, isEdited);
	paramMatch("phantomType", getPhantomType(), ctrpart->getPhantomType(), output, storeline, base, false, open, isEdited);
	paramMatch("linearVelocityMode", linearVelocityMode, ctrpart->linearVelocityMode, output, storeline, base, false, open, isEdited);
	paramMatch("ignoreCollisionDuringRotation", ignoreCollisionDuringRotation, ctrpart->ignoreCollisionDuringRotation, output, storeline, base, false, open, isEdited);
	paramMatch("ignoreIncomingTranslation", ignoreIncomingTranslation, ctrpart->ignoreIncomingTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("includeDownwardMomentum", includeDownwardMomentum, ctrpart->includeDownwardMomentum, output, storeline, base, false, open, isEdited);
	paramMatch("followWorldFromModel", followWorldFromModel, ctrpart->followWorldFromModel, output, storeline, base, false, open, isEdited);
	paramMatch("isTouchingGround", isTouchingGround, ctrpart->isTouchingGround, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", proxymodifier::classname, output, isEdited);
}

void hkbproxymodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(45);

	output.push_back(openObject(base, ID, proxymodifier::classname, proxymodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "proxyInfo"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "dynamicFriction", proxyInfo.dynamicFriction));
	output.push_back(autoParam(base, "staticFriction", proxyInfo.staticFriction));
	output.push_back(autoParam(base, "keepContactTolerance", proxyInfo.keepContactTolerance));
	output.push_back(autoParam(base, "up", proxyInfo.up));
	output.push_back(autoParam(base, "keepDistance", proxyInfo.keepDistance));
	output.push_back(autoParam(base, "contactAngleSensitivity", proxyInfo.contactAngleSensitivity));
	output.push_back(autoParam(base, "userPlanes", proxyInfo.userPlanes));
	output.push_back(autoParam(base, "maxCharacterSpeedForSolver", proxyInfo.maxCharacterSpeedForSolver));
	output.push_back(autoParam(base, "characterStrength", proxyInfo.characterStrength));
	output.push_back(autoParam(base, "characterMass", proxyInfo.characterMass));
	output.push_back(autoParam(base, "maxSlope", proxyInfo.maxSlope));
	output.push_back(autoParam(base, "penetrationRecoverySpeed", proxyInfo.penetrationRecoverySpeed));
	output.push_back(autoParam(base, "maxCastIterations", proxyInfo.maxCastIterations));
	output.push_back(autoParam(base, "refreshManifoldInCheckSupport", proxyInfo.refreshManifoldInCheckSupport));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "linearVelocity", linearVelocity));
	output.push_back(autoParam(base, "horizontalGain", horizontalGain));
	output.push_back(autoParam(base, "verticalGain", verticalGain));
	output.push_back(autoParam(base, "maxHorizontalSeparation", maxHorizontalSeparation));
	output.push_back(autoParam(base, "limitHeadingDegrees", limitHeadingDegrees));
	output.push_back(autoParam(base, "maxVerticalSeparation", maxVerticalSeparation));
	output.push_back(autoParam(base, "verticalDisplacementError", verticalDisplacementError));
	output.push_back(autoParam(base, "verticalDisplacementErrorGain", verticalDisplacementErrorGain));
	output.push_back(autoParam(base, "maxVerticalDisplacement", maxVerticalDisplacement));
	output.push_back(autoParam(base, "minVerticalDisplacement", minVerticalDisplacement));
	output.push_back(autoParam(base, "capsuleHeight", capsuleHeight));
	output.push_back(autoParam(base, "capsuleRadius", capsuleRadius));
	output.push_back(autoParam(base, "maxSlopeForRotation", maxSlopeForRotation));
	output.push_back(autoParam(base, "collisionFilterInfo", collisionFilterInfo));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, proxymodifier::classname, output, true);
}

void hkbproxymodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbproxymodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

string hkbproxymodifier::getPhantomType()
{
	switch (phantomType)
	{
		case PHANTOM_TYPE_SIMPLE: return "PHANTOM_TYPE_SIMPLE";
		case PHANTOM_TYPE_CACHING: return "PHANTOM_TYPE_CACHING";
		default: return "PHANTOM_TYPE_SIMPLE";
	}
}

string hkbproxymodifier::getLinearVelocityMode()
{
	switch (linearVelocityMode)
	{
		case LINEAR_VELOCITY_MODE_WORLD: return "LINEAR_VELOCITY_MODE_WORLD";
		case LINEAR_VELOCITY_MODE_MODEL: return "LINEAR_VELOCITY_MODE_MODEL";
		default: return "LINEAR_VELOCITY_MODE_WORLD";
	}
}

void hkbProxyModifierExport(string id)
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
		cout << "ERROR: Edit hkbProxyModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbProxyModifier Output Not Found (File: " << filename << ")" << endl;
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