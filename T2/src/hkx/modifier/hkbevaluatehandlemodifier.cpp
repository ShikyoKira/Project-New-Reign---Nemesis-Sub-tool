#include <boost/thread.hpp>

#include "hkbevaluatehandlemodifier.h"
#include "src/utilities/hkMap.h"

using namespace std;

namespace evaluatehandlemodifier
{
	const string key = "ao";
	const string classname = "hkbEvaluateHandleModifier";
	const string signature = "0x79757102";

	hkMap<string, hkbevaluatehandlemodifier::handlechangemode> modeMap
	{
		{ "HANDLE_CHANGE_MODE_ABRUPT",  hkbevaluatehandlemodifier::HANDLE_CHANGE_MODE_ABRUPT},
		{ "HANDLE_CHANGE_MODE_CONSTANT_VELOCITY",  hkbevaluatehandlemodifier::HANDLE_CHANGE_MODE_CONSTANT_VELOCITY},
	};
}

string hkbevaluatehandlemodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbevaluatehandlemodifier>> hkbevaluatehandlemodifierList;
safeStringUMap<shared_ptr<hkbevaluatehandlemodifier>> hkbevaluatehandlemodifierList_E;

void hkbevaluatehandlemodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbevaluatehandlemodifierList_E[id] = shared_from_this() : hkbevaluatehandlemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbevaluatehandlemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("handle", line, output))
					{
						handle = (isEdited ? hkbhandleList_E : hkbhandleList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					if (readParam("handlePositionOut", line, handlePositionOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("handleRotationOut", line, handleRotationOut)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("isValidOut", line, isValidOut)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("extrapolationTimeStep", line, extrapolationTimeStep)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("handleChangeSpeed", line, handleChangeSpeed)) ++type;

					break;
				}
				case 10:
				{
					string output;

					if (readParam("handleChangeMode", line, output))
					{
						handleChangeMode = evaluatehandlemodifier::modeMap[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << evaluatehandlemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbevaluatehandlemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + evaluatehandlemodifier::key + to_string(functionlayer) + ">";
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

			if (handle) threadedNextNode(handle, filepath, curadd, functionlayer, graphroot);
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
				hkbevaluatehandlemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbevaluatehandlemodifierList_E[ID] = protect;
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
				hkbevaluatehandlemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbevaluatehandlemodifierList_E[ID] = protect;
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

string hkbevaluatehandlemodifier::getClassCode()
{
	return evaluatehandlemodifier::key;
}

void hkbevaluatehandlemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(13);
	output.reserve(13);
	usize base = 2;
	hkbevaluatehandlemodifier* ctrpart = static_cast<hkbevaluatehandlemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, evaluatehandlemodifier::classname, evaluatehandlemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("handle", handle, ctrpart->handle, output, storeline, base, false, open, isEdited);
	paramMatch("handlePositionOut", handlePositionOut, ctrpart->handlePositionOut, output, storeline, base, false, open, isEdited);
	paramMatch("handleRotationOut", handleRotationOut, ctrpart->handleRotationOut, output, storeline, base, false, open, isEdited);
	paramMatch("isValidOut", isValidOut, ctrpart->isValidOut, output, storeline, base, false, open, isEdited);
	paramMatch("extrapolationTimeStep", extrapolationTimeStep, ctrpart->extrapolationTimeStep, output, storeline, base, false, open, isEdited);
	paramMatch("handleChangeSpeed", handleChangeSpeed, ctrpart->handleChangeSpeed, output, storeline, base, false, open, isEdited);
	paramMatch("handleChangeMode", getHandleChangeMode(), ctrpart->getHandleChangeMode(), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", evaluatehandlemodifier::classname, output, isEdited);
}

void hkbevaluatehandlemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(13);

	output.push_back(openObject(base, ID, evaluatehandlemodifier::classname, evaluatehandlemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "handle", handle));
	output.push_back(autoParam(base, "handlePositionOut", handlePositionOut));
	output.push_back(autoParam(base, "handleRotationOut", handleRotationOut));
	output.push_back(autoParam(base, "isValidOut", isValidOut));
	output.push_back(autoParam(base, "extrapolationTimeStep", extrapolationTimeStep));
	output.push_back(autoParam(base, "handleChangeSpeed", handleChangeSpeed));
	output.push_back(autoParam(base, "handleChangeMode", getHandleChangeMode()));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, evaluatehandlemodifier::classname, output, true);
}

void hkbevaluatehandlemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (handle) hkb_parent[handle] = shared_from_this();
}

void hkbevaluatehandlemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (handle)
	{
		parentRefresh();
		handle->connect(filepath, address, functionlayer, true, graphroot);
	}
}

string hkbevaluatehandlemodifier::getHandleChangeMode()
{
	return evaluatehandlemodifier::modeMap[handleChangeMode];
}

void hkbevaluatehandlemodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
