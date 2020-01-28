#include "hkbtwistmodifier.h"
#include "src/utilities/hkMap.h"

using namespace std;

namespace twistmodifier
{
	const string key = "z";
	const string classname = "hkbTwistModifier";
	const string signature = "0xb6b76b32";

	hkMap<string, hkbtwistmodifier::setanglemethod> methodMap =
	{
		{ "LINEAR", hkbtwistmodifier::LINEAR },
		{ "RAMPED", hkbtwistmodifier::RAMPED },
	};

	hkMap<string, hkbtwistmodifier::rotationaxiscoordinates> axisMap =
	{
		{ "ROTATION_AXIS_IN_MODEL_COORDINATES", hkbtwistmodifier::ROTATION_AXIS_IN_MODEL_COORDINATES },
		{ "ROTATION_AXIS_IN_LOCAL_COORDINATES", hkbtwistmodifier::ROTATION_AXIS_IN_LOCAL_COORDINATES },
	};
}

string hkbtwistmodifier::getAngleMethod()
{
	return twistmodifier::methodMap[setAngleMethod];
}

string hkbtwistmodifier::getRotationAxis()
{
	return twistmodifier::axisMap[rotationAxisCoordinates];
}

string hkbtwistmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbtwistmodifier>> hkbtwistmodifierList;
safeStringUMap<shared_ptr<hkbtwistmodifier>> hkbtwistmodifierList_E;

void hkbtwistmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbtwistmodifierList_E[id] = shared_from_this() : hkbtwistmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbtwistmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("axisOfRotation", line, axisOfRotation)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("twistAngle", line, twistAngle)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("startBoneIndex", line, startBoneIndex)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("endBoneIndex", line, endBoneIndex)) ++type;

					break;
				}
				case 8:
				{
					string output;

					if (readParam("setAngleMethod", line, output))
					{
						setAngleMethod = twistmodifier::methodMap[output];
						++type;
					}

					break;
				}
				case 9:
				{
					string output;

					if (readParam("rotationAxisCoordinates", line, output))
					{
						rotationAxisCoordinates = twistmodifier::axisMap[output];
						++type;
					}

					break;
				}
				case 10:
				{
					if (readParam("isAdditive", line, isAdditive)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << twistmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbtwistmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + twistmodifier::key + to_string(functionlayer) + ">";
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
				hkbtwistmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbtwistmodifierList_E[ID] = protect;
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
				hkbtwistmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbtwistmodifierList_E[ID] = protect;
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

string hkbtwistmodifier::getClassCode()
{
	return twistmodifier::key;
}

void hkbtwistmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(13);
	usize base = 2;
	hkbtwistmodifier* ctrpart = static_cast<hkbtwistmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, twistmodifier::classname, twistmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("axisOfRotation", axisOfRotation, ctrpart->axisOfRotation, output, storeline, base, false, open, isEdited);
	paramMatch("twistAngle", twistAngle, ctrpart->twistAngle, output, storeline, base, false, open, isEdited);
	paramMatch("startBoneIndex", startBoneIndex, ctrpart->startBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("endBoneIndex", endBoneIndex, ctrpart->endBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("setAngleMethod", getAngleMethod(), ctrpart->getAngleMethod(), output, storeline, base, false, open, isEdited);
	paramMatch("rotationAxisCoordinates", getRotationAxis(), ctrpart->getRotationAxis(), output, storeline, base, false, open, isEdited);
	paramMatch("isAdditive", isAdditive, ctrpart->isAdditive, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", twistmodifier::classname, output, isEdited);
}

void hkbtwistmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(13);

	output.push_back(openObject(base, ID, twistmodifier::classname, twistmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "axisOfRotation", axisOfRotation));
	output.push_back(autoParam(base, "twistAngle", twistAngle));
	output.push_back(autoParam(base, "startBoneIndex", startBoneIndex));
	output.push_back(autoParam(base, "endBoneIndex", endBoneIndex));
	output.push_back(autoParam(base, "setAngleMethod", getAngleMethod()));
	output.push_back(autoParam(base, "rotationAxisCoordinates", getRotationAxis()));
	output.push_back(autoParam(base, "isAdditive", isAdditive));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, twistmodifier::classname, output, true);
}

void hkbtwistmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbtwistmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

string hkbtwistmodifier::getAngleMethod()
{
	return twistmodifier::methodMap[setAngleMethod];
}

string hkbtwistmodifier::getRotationAxis()
{
	return twistmodifier::axisMap[rotationAxisCoordinates];
}
