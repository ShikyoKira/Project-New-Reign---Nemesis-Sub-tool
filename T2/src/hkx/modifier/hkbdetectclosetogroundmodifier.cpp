#include <boost\thread.hpp>
#include "hkbdetectclosetogroundmodifier.h"

using namespace std;

namespace detectclosetogroundmodifier
{
	const string key = "bu";
	const string classname = "hkbDetectCloseToGroundModifier";
	const string signature = "0x981687b2";
}

string hkbdetectclosetogroundmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbdetectclosetogroundmodifier>> hkbdetectclosetogroundmodifierList;
safeStringUMap<shared_ptr<hkbdetectclosetogroundmodifier>> hkbdetectclosetogroundmodifierList_E;

void hkbdetectclosetogroundmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbdetectclosetogroundmodifierList_E[id] = shared_from_this() : hkbdetectclosetogroundmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbdetectclosetogroundmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("id", line, closeToGroundEvent.id)) ++type;

					break;
				}
				case 5:
				{
					string output;

					if (readParam("payload", line, output))
					{
						closeToGroundEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("closeToGroundHeight", line, closeToGroundHeight)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("raycastDistanceDown", line, raycastDistanceDown)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("collisionFilterInfo", line, collisionFilterInfo)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("boneIndex", line, boneIndex)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("animBoneIndex", line, animBoneIndex)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << detectclosetogroundmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbdetectclosetogroundmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + detectclosetogroundmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	closeToGroundEvent.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (closeToGroundEvent.payload) threadedNextNode(closeToGroundEvent.payload, filepath, curadd, functionlayer, graphroot);
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
				hkbdetectclosetogroundmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbdetectclosetogroundmodifierList_E[ID] = protect;
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
				hkbdetectclosetogroundmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbdetectclosetogroundmodifierList_E[ID] = protect;
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

string hkbdetectclosetogroundmodifier::getClassCode()
{
	return detectclosetogroundmodifier::key;
}

void hkbdetectclosetogroundmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(12);
	output.reserve(12);
	usize base = 2;
	hkbdetectclosetogroundmodifier* ctrpart = static_cast<hkbdetectclosetogroundmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, detectclosetogroundmodifier::classname, detectclosetogroundmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "closeToGroundEvent"));		// 3
	output.push_back(openObject(base));		// 4
	paramMatch("id", closeToGroundEvent.id, ctrpart->closeToGroundEvent.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", closeToGroundEvent.payload, ctrpart->closeToGroundEvent.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 4
	output.push_back(closeParam(base));		// 3
	paramMatch("closeToGroundHeight", closeToGroundHeight, ctrpart->closeToGroundHeight, output, storeline, base, false, open, isEdited);
	paramMatch("raycastDistanceDown", raycastDistanceDown, ctrpart->raycastDistanceDown, output, storeline, base, false, open, isEdited);
	paramMatch("collisionFilterInfo", collisionFilterInfo, ctrpart->collisionFilterInfo, output, storeline, base, false, open, isEdited);
	paramMatch("boneIndex", boneIndex, ctrpart->boneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("animBoneIndex", animBoneIndex, ctrpart->animBoneIndex, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", detectclosetogroundmodifier::classname, output, isEdited);
}

void hkbdetectclosetogroundmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(12);

	output.push_back(openObject(base, ID, detectclosetogroundmodifier::classname, detectclosetogroundmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "closeToGroundEvent"));		// 3
	output.push_back(openObject(base));		// 4
	output.push_back(autoParam(base, "id", closeToGroundEvent.id));
	output.push_back(autoParam(base, "payload", closeToGroundEvent.payload));
	output.push_back(closeObject(base));		// 4
	output.push_back(closeParam(base));		// 3
	output.push_back(autoParam(base, "closeToGroundHeight", closeToGroundHeight));
	output.push_back(autoParam(base, "raycastDistanceDown", raycastDistanceDown));
	output.push_back(autoParam(base, "collisionFilterInfo", collisionFilterInfo));
	output.push_back(autoParam(base, "boneIndex", boneIndex));
	output.push_back(autoParam(base, "animBoneIndex", animBoneIndex));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, detectclosetogroundmodifier::classname, output, true);
}

void hkbdetectclosetogroundmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (closeToGroundEvent.payload) hkb_parent[closeToGroundEvent.payload] = shared_from_this();
}

void hkbdetectclosetogroundmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (closeToGroundEvent.payload)
	{
		parentRefresh();
		closeToGroundEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbdetectclosetogroundmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
