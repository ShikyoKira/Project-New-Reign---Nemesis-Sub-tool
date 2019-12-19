#include <boost\thread.hpp>
#include "bslookatmodifier.h"
#include "highestscore.h"

using namespace std;

namespace lookatmodifier_b
{
	const string key = "bv";
	const string classname = "BSLookAtModifier";
	const string signature = "0xd756fc25";
}

string bslookatmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bslookatmodifier>> bslookatmodifierList;
safeStringUMap<shared_ptr<bslookatmodifier>> bslookatmodifierList_E;

void bslookatmodifier::regis(string id, bool isEdited)
{
	isEdited ? bslookatmodifierList_E[id] = shared_from_this() : bslookatmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bslookatmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	vector<bone>* curbone = &bones;

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
					if (readParam("lookAtTarget", line, lookAtTarget)) ++type;

					break;
				}
				case 5:
				{
					usize numelement;

					if (readEleParam("bones", line, numelement))
					{
						bones.reserve(numelement);
						++type;
					}

					break;
				}
				case 6:
				{
					int output;

					if (readParam("index", line, output))
					{
						curbone->push_back(output);
						++type;
						break;
					}

					usize numelement;

					if (readEleParam("eyeBones", line, numelement))
					{
						eyeBones.reserve(numelement);
						curbone = &eyeBones;
						break;
					}
					
					if (readParam("limitAngleDegrees", line, limitAngleDegrees)) type = 12;

					break;
				}
				case 7:
				{
					if (readParam("fwdAxisLS", line, curbone->back().fwdAxisLS)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("limitAngleDegrees", line, curbone->back().limitAngleDegrees)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("onGain", line, curbone->back().onGain)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("offGain", line, curbone->back().offGain)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("enabled", line, curbone->back().enabled)) type = 6;

					break;
				}
				case 12:
				{
					if (readParam("limitAngleThresholdDegrees", line, limitAngleThresholdDegrees)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("continueLookOutsideOfLimit", line, continueLookOutsideOfLimit)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("onGain", line, onGain)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("offGain", line, offGain)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("useBoneGains", line, useBoneGains)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("targetLocation", line, targetLocation)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("targetOutsideLimits", line, targetOutsideLimits)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("id", line, targetOutOfLimitEvent.id)) ++type;

					break;
				}
				case 20:
				{
					string output;

					if (readParam("payload", line, output))
					{
						targetOutOfLimitEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 21:
				{
					if (readParam("lookAtCamera", line, lookAtCamera)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("lookAtCameraX", line, lookAtCameraX)) ++type;

					break;
				}
				case 23:
				{
					if (readParam("lookAtCameraY", line, lookAtCameraY)) ++type;

					break;
				}
				case 24:
				{
					if (readParam("lookAtCameraZ", line, lookAtCameraZ)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << lookatmodifier_b::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bslookatmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + lookatmodifier_b::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	targetOutOfLimitEvent.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (targetOutOfLimitEvent.payload) threadedNextNode(targetOutOfLimitEvent.payload, filepath, curadd, functionlayer, graphroot);
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
				bslookatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bslookatmodifierList_E[ID] = protect;
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
				bslookatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bslookatmodifierList_E[ID] = protect;
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

string bslookatmodifier::getClassCode()
{
	return lookatmodifier_b::key;
}

void bslookatmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(100);
	usize base = 2;
	bslookatmodifier* ctrpart = static_cast<bslookatmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, lookatmodifier_b::classname, lookatmodifier_b::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtTarget", lookAtTarget, ctrpart->lookAtTarget, output, storeline, base, true, open, isEdited);

	elementCompare(bones, ctrpart->bones, output, storeline, "bones", base, open, isEdited);
	elementCompare(eyeBones, ctrpart->eyeBones, output, storeline, "eyeBones", base, open, isEdited);

	paramMatch("limitAngleDegrees", limitAngleDegrees, ctrpart->limitAngleDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleThresholdDegrees", limitAngleThresholdDegrees, ctrpart->limitAngleThresholdDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("continueLookOutsideOfLimit", continueLookOutsideOfLimit, ctrpart->continueLookOutsideOfLimit, output, storeline, base, false, open, isEdited);
	paramMatch("onGain", onGain, ctrpart->onGain, output, storeline, base, false, open, isEdited);
	paramMatch("offGain", offGain, ctrpart->offGain, output, storeline, base, false, open, isEdited);
	paramMatch("useBoneGains", useBoneGains, ctrpart->useBoneGains, output, storeline, base, false, open, isEdited);
	paramMatch("targetLocation", targetLocation, ctrpart->targetLocation, output, storeline, base, false, open, isEdited);
	paramMatch("targetOutsideLimits", targetOutsideLimits, ctrpart->targetOutsideLimits, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "targetOutOfLimitEvent"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", targetOutOfLimitEvent.id, ctrpart->targetOutOfLimitEvent.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", targetOutOfLimitEvent.payload, ctrpart->targetOutOfLimitEvent.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("lookAtCamera", lookAtCamera, ctrpart->lookAtCamera, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtCameraX", lookAtCameraX, ctrpart->lookAtCameraX, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtCameraY", lookAtCameraY, ctrpart->lookAtCameraY, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtCameraZ", lookAtCameraZ, ctrpart->lookAtCameraZ, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", lookatmodifier_b::classname, output, isEdited);
}

void bslookatmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, lookatmodifier_b::classname, lookatmodifier_b::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "lookAtTarget", lookAtTarget));

	usize size = bones.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "bones", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bones", size));		// 2

	for (auto& bone : bones)
	{
		elementNew(output, bone, base);
	}
	
	if (size > 0) output.push_back(closeParam(base));		// 2

	size = eyeBones.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "eyeBones", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "eyeBones", size));		// 2

	for (auto& bone : eyeBones)
	{
		elementNew(output, bone, base);
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "limitAngleDegrees", limitAngleDegrees));
	output.push_back(autoParam(base, "limitAngleThresholdDegrees", limitAngleThresholdDegrees));
	output.push_back(autoParam(base, "continueLookOutsideOfLimit", continueLookOutsideOfLimit));
	output.push_back(autoParam(base, "onGain", onGain));
	output.push_back(autoParam(base, "offGain", offGain));
	output.push_back(autoParam(base, "useBoneGains", useBoneGains));
	output.push_back(autoParam(base, "targetLocation", targetLocation));
	output.push_back(autoParam(base, "targetOutsideLimits", targetOutsideLimits));
	output.push_back(openParam(base, "targetOutOfLimitEvent"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", targetOutOfLimitEvent.id));
	output.push_back(autoParam(base, "payload", targetOutOfLimitEvent.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "lookAtCamera", lookAtCamera));
	output.push_back(autoParam(base, "lookAtCameraX", lookAtCameraX));
	output.push_back(autoParam(base, "lookAtCameraY", lookAtCameraY));
	output.push_back(autoParam(base, "lookAtCameraZ", lookAtCameraZ));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, lookatmodifier_b::classname, output, true);
}

void bslookatmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (targetOutOfLimitEvent.payload) hkb_parent[targetOutOfLimitEvent.payload] = shared_from_this();
}

void bslookatmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (targetOutOfLimitEvent.payload)
	{
		parentRefresh();
		targetOutOfLimitEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bslookatmodifier::matchScoring(vector<bone>& ori, vector<bone>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<bone> newOri;
		vector<bone> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(bone());
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
			
			if (ori[i].index == edit[j].index)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].fwdAxisLS == edit[j].fwdAxisLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].limitAngleDegrees == edit[j].limitAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].onGain == edit[j].onGain)
			{
				++scorelist[i][j];
			}

			if (ori[i].offGain == edit[j].offGain)
			{
				++scorelist[i][j];
			}

			if (ori[i].enabled == edit[j].enabled)
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
	vector<bone> newOri;
	vector<bone> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(bone());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(bone());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void bslookatmodifier::elementCompare(vector<bone>& ori, vector<bone>& edit, vecstr& output, vecstr& storeline, string classname, usize& base, bool& open, bool& isEdited)
{
	usize size = ori.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, classname, size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, classname, size));		// 2

	matchScoring(ori, edit, ID);
	size = ori.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (ori[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				elementNew(output, edit[i], base);
				++i;
			}
		}
		// deleted existing data
		else if (edit[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!edit[i].proxy)
				{
					--i;
					break;
				}

				elementNew(storeline, ori[i], base);
				++i;
				usize spaces = 0;

				while (spaces++ < 8)
				{
					output.push_back("");	// 8 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("index", ori[i].index, edit[i].index, output, storeline, base, false, open, isEdited);
			paramMatch("fwdAxisLS", ori[i].fwdAxisLS, edit[i].fwdAxisLS, output, storeline, base, false, open, isEdited);
			paramMatch("limitAngleDegrees", ori[i].limitAngleDegrees, edit[i].limitAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("onGain", ori[i].onGain, edit[i].onGain, output, storeline, base, false, open, isEdited);
			paramMatch("offGain", ori[i].offGain, edit[i].offGain, output, storeline, base, false, open, isEdited);
			paramMatch("enabled", ori[i].enabled, edit[i].enabled, output, storeline, base, true, open, isEdited);
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

}

void bslookatmodifier::elementNew(vecstr& output, bone& bone, usize& base)
{
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "index", bone.index));
	output.push_back(autoParam(base, "fwdAxisLS", bone.fwdAxisLS));
	output.push_back(autoParam(base, "limitAngleDegrees", bone.limitAngleDegrees));
	output.push_back(autoParam(base, "onGain", bone.onGain));
	output.push_back(autoParam(base, "offGain", bone.offGain));
	output.push_back(autoParam(base, "enabled", bone.enabled));
	output.push_back(closeObject(base));		// 3
}

void bslookatmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
