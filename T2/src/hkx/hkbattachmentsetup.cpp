#include "hkbattachmentsetup.h"

using namespace std;

namespace attachmentsetup
{
	const string key = "cq";
	const string classname = "hkbAttachmentSetup";
	const string signature = "0x0774632b";
}

safeStringUMap<shared_ptr<hkbattachmentsetup>> hkbattachmentsetupList;
safeStringUMap<shared_ptr<hkbattachmentsetup>> hkbattachmentsetupList_E;

void hkbattachmentsetup::regis(string id, bool isEdited)
{
	isEdited ? hkbattachmentsetupList_E[id] = shared_from_this() : hkbattachmentsetupList[id] = shared_from_this();
	ID = id;
}

void hkbattachmentsetup::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("blendInTime", line, blendInTime)) ++type;

					break;
				}
				case 1:
				{
					if (readParam("moveAttacherFraction", line, moveAttacherFraction)) ++type;

					break;
				}
				case 2:
				{
					if (readParam("gain", line, gain)) ++type;

					break;
				}
				case 3:
				{
					if (readParam("extrapolationTimeStep", line, extrapolationTimeStep)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("fixUpGain", line, fixUpGain)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("maxLinearDistance", line, maxLinearDistance)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("maxAngularDistance", line, maxAngularDistance)) ++type;

					break;
				}
				case 7:
				{
					string data;

					if (readParam("attachmentType", line, data))
					{
						if (data == "ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY") attachmentType = ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY;
						else if (data == "ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT") attachmentType = ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT;
						else if (data == "ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT") attachmentType = ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT;
						else if (data == "ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL") attachmentType = ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL;
						else attachmentType = ATTACHMENT_TYPE_NONE;

						++type;
					}
				}
			}
		}
		if (line.find("<hkparam name=\"") != string::npos)
		{
			for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
				itr != boost::sregex_iterator(); ++itr)
			{
				string header = itr->str(1);

				if (header == "blendInTime") blendInTime = stod(itr->str(2));
				else if (header == "moveAttacherFraction") moveAttacherFraction = stod(itr->str(2));
				else if (header == "gain") gain = stod(itr->str(2));
				else if (header == "extrapolationTimeStep") extrapolationTimeStep = stod(itr->str(2));
				else if (header == "fixUpGain") fixUpGain = stod(itr->str(2));
				else if (header == "maxLinearDistance") maxLinearDistance = stod(itr->str(2));
				else if (header == "maxAngularDistance") maxAngularDistance = stod(itr->str(2));
				else if (header == "attachmentType")
				{
					string data = itr->str(2);

					if (data == "ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY") attachmentType = ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY;
					else if (data == "ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT") attachmentType = ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT;
					else if (data == "ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT") attachmentType = ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT;
					else if (data == "ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL") attachmentType = ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL;
					else attachmentType = ATTACHMENT_TYPE_NONE;
				}

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << attachmentsetup::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbattachmentsetup::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + attachmentsetup::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			auto parent_itr = hkb_parent.find(shared_from_this());

			// existed
			if (addressID.find(address) != addressID.end() && parent_itr != hkb_parent.end() && IsForeign.find(parent_itr->second->ID) == IsForeign.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbattachmentsetupList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbattachmentsetupList_E[ID] = protect;
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
				hkbattachmentsetupList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbattachmentsetupList_E[ID] = protect;
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

string hkbattachmentsetup::getClassCode()
{
	return attachmentsetup::key;
}

void hkbattachmentsetup::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(11);
	usize base = 2;
	hkbattachmentsetup* ctrpart = static_cast<hkbattachmentsetup*>(counterpart.get());

	output.push_back(openObject(base, ID, attachmentsetup::classname, attachmentsetup::signature));		// 1
	paramMatch("blendInTime", blendInTime, ctrpart->blendInTime, output, storeline, base, false, open, isEdited);
	paramMatch("moveAttacherFraction", moveAttacherFraction, ctrpart->moveAttacherFraction, output, storeline, base, false, open, isEdited);
	paramMatch("gain", gain, ctrpart->gain, output, storeline, base, false, open, isEdited);
	paramMatch("extrapolationTimeStep", extrapolationTimeStep, ctrpart->extrapolationTimeStep, output, storeline, base, false, open, isEdited);
	paramMatch("fixUpGain", fixUpGain, ctrpart->fixUpGain, output, storeline, base, false, open, isEdited);
	paramMatch("maxLinearDistance", maxLinearDistance, ctrpart->maxLinearDistance, output, storeline, base, false, open, isEdited);
	paramMatch("maxAngularDistance", maxAngularDistance, ctrpart->maxAngularDistance, output, storeline, base, false, open, isEdited);
	paramMatch("attachmentType", getAttachmentType(), ctrpart->getAttachmentType(), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", attachmentsetup::classname, output, isEdited);
}

void hkbattachmentsetup::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, attachmentsetup::classname, attachmentsetup::signature));		// 1
	output.push_back(autoParam(base, "blendInTime", blendInTime));
	output.push_back(autoParam(base, "moveAttacherFraction", moveAttacherFraction));
	output.push_back(autoParam(base, "gain", gain));
	output.push_back(autoParam(base, "extrapolationTimeStep", extrapolationTimeStep));
	output.push_back(autoParam(base, "fixUpGain", fixUpGain));
	output.push_back(autoParam(base, "maxLinearDistance", maxLinearDistance));
	output.push_back(autoParam(base, "maxAngularDistance", maxAngularDistance));
	output.push_back(autoParam(base, "attachmentType", getAttachmentType()));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, attachmentsetup::classname, output, true);
}

string hkbattachmentsetup::getAttachmentType()
{
	if (attachmentType == ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY) return "ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY";
	else if (attachmentType == ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT) return "ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT";
	else if (attachmentType == ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT) return "ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT";
	else if (attachmentType == ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL) return "ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL";
	else return "ATTACHMENT_TYPE_NONE";
}
