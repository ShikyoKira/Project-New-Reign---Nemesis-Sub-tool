#include <boost\thread.hpp>
#include "hkbhandikcontrolsmodifier.h"
#include "highestscore.h"

using namespace std;

namespace handikcontrolsmodifier
{
	const string key = "by";
	const string classname = "hkbHandIkControlsModifier";
	const string signature = "0x9f0488bb";
}

string hkbhandikcontrolsmodifier::GetAddress()
{
	return address;
}

string hkbhandikcontrolsmodifier::hkhand::controldata::getHandleChangeMode()
{
	switch (handleChangeMode)
	{
	case HANDLE_CHANGE_MODE_ABRUPT: return "HANDLE_CHANGE_MODE_ABRUPT";
	case HANDLE_CHANGE_MODE_CONSTANT_VELOCITY: return "HANDLE_CHANGE_MODE_CONSTANT_VELOCITY";
	default: return "HANDLE_CHANGE_MODE_ABRUPT";
	}
}

safeStringUMap<shared_ptr<hkbhandikcontrolsmodifier>> hkbhandikcontrolsmodifierList;
safeStringUMap<shared_ptr<hkbhandikcontrolsmodifier>> hkbhandikcontrolsmodifierList_E;

void hkbhandikcontrolsmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbhandikcontrolsmodifierList_E[id] = shared_from_this() : hkbhandikcontrolsmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbhandikcontrolsmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					usize numelement;

					if (readEleParam("hands", line, numelement))
					{
						hands.reserve(numelement);
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("targetPosition", line, output))
					{
						hands.push_back(output);
						++type;
						break;
					}
				}
				case 6:
				{
					if (readParam("targetRotation", line, hands.back().controlData.targetRotation)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("targetNormal", line, hands.back().controlData.targetNormal)) ++type;

					break;
				}
				case 8:
				{
					string output;

					if (readParam("targetHandle", line, output))
					{
						hands.back().controlData.targetHandle = (isEdited ? hkbhandleList_E : hkbhandleList)[output];
						++type;
					}

					break;
				}
				case 9:
				{
					if (readParam("transformOnFraction", line, hands.back().controlData.transformOnFraction)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("normalOnFraction", line, hands.back().controlData.normalOnFraction)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("fadeInDuration", line, hands.back().controlData.fadeInDuration)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("fadeOutDuration", line, hands.back().controlData.fadeOutDuration)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("extrapolationTimeStep", line, hands.back().controlData.extrapolationTimeStep)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("handleChangeSpeed", line, hands.back().controlData.handleChangeSpeed)) ++type;

					break;
				}
				case 15:
				{
					string output;

					if (readParam("handleChangeMode", line, output))
					{
						hands.back().controlData.handleChangeMode = output == "HANDLE_CHANGE_MODE_ABRUPT" ? hkhand::controldata::HANDLE_CHANGE_MODE_ABRUPT :
							hkhand::controldata::HANDLE_CHANGE_MODE_CONSTANT_VELOCITY;
						++type;
					}

					break;
				}
				case 16:
				{
					if (readParam("fixUp", line, hands.back().controlData.fixUp)) type = 5;

					break;
				}
				case 17:
				{
					if (readParam("handIndex", line, hands.back().handIndex)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("enable", line, hands.back().enable)) type = 5;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << handikcontrolsmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbhandikcontrolsmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + handikcontrolsmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

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

			for (auto& hand : hands)
			{
				if (hand.controlData.targetHandle) threadedNextNode(hand.controlData.targetHandle, filepath, curadd + to_string(tempint++), functionlayer, graphroot);
			}
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
				hkbhandikcontrolsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbhandikcontrolsmodifierList_E[ID] = protect;
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
				hkbhandikcontrolsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbhandikcontrolsmodifierList_E[ID] = protect;
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

string hkbhandikcontrolsmodifier::getClassCode()
{
	return handikcontrolsmodifier::key;
}

void hkbhandikcontrolsmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbhandikcontrolsmodifier* ctrpart = static_cast<hkbhandikcontrolsmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, handikcontrolsmodifier::classname, handikcontrolsmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);

	usize size = hands.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "hands", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "hands", size));		// 2

	matchScoring(hands, ctrpart->hands, ID);
	size = hands.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (hands[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(openParam(base, "controlData"));		// 4
				output.push_back(openObject(base));		// 5
				output.push_back(autoParam(base, "targetPosition", ctrpart->hands[i].controlData.targetPosition));
				output.push_back(autoParam(base, "targetRotation", ctrpart->hands[i].controlData.targetRotation));
				output.push_back(autoParam(base, "targetNormal", ctrpart->hands[i].controlData.targetNormal));
				output.push_back(autoParam(base, "targetHandle", ctrpart->hands[i].controlData.targetHandle));
				output.push_back(autoParam(base, "transformOnFraction", ctrpart->hands[i].controlData.transformOnFraction));
				output.push_back(autoParam(base, "normalOnFraction", ctrpart->hands[i].controlData.normalOnFraction));
				output.push_back(autoParam(base, "fadeInDuration", ctrpart->hands[i].controlData.fadeInDuration));
				output.push_back(autoParam(base, "fadeOutDuration", ctrpart->hands[i].controlData.fadeOutDuration));
				output.push_back(autoParam(base, "extrapolationTimeStep", ctrpart->hands[i].controlData.extrapolationTimeStep));
				output.push_back(autoParam(base, "handleChangeSpeed", ctrpart->hands[i].controlData.handleChangeSpeed));
				output.push_back(autoParam(base, "handleChangeMode", ctrpart->hands[i].controlData.getHandleChangeMode()));
				output.push_back(autoParam(base, "fixUp", ctrpart->hands[i].controlData.fixUp));
				output.push_back(closeObject(base));		// 5
				output.push_back(closeParam(base));		// 4
				output.push_back(autoParam(base, "handIndex", ctrpart->hands[i].handIndex));
				output.push_back(autoParam(base, "enable", ctrpart->hands[i].enable));
				output.push_back(closeObject(base));		// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->hands[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->hands[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(openParam(base, "controlData"));		// 4
				storeline.push_back(openObject(base));		// 5
				storeline.push_back(autoParam(base, "targetPosition", hands[i].controlData.targetPosition));
				storeline.push_back(autoParam(base, "targetRotation", hands[i].controlData.targetRotation));
				storeline.push_back(autoParam(base, "targetNormal", hands[i].controlData.targetNormal));
				storeline.push_back(autoParam(base, "targetHandle", hands[i].controlData.targetHandle));
				storeline.push_back(autoParam(base, "transformOnFraction", hands[i].controlData.transformOnFraction));
				storeline.push_back(autoParam(base, "normalOnFraction", hands[i].controlData.normalOnFraction));
				storeline.push_back(autoParam(base, "fadeInDuration", hands[i].controlData.fadeInDuration));
				storeline.push_back(autoParam(base, "fadeOutDuration", hands[i].controlData.fadeOutDuration));
				storeline.push_back(autoParam(base, "extrapolationTimeStep", hands[i].controlData.extrapolationTimeStep));
				storeline.push_back(autoParam(base, "handleChangeSpeed", hands[i].controlData.handleChangeSpeed));
				storeline.push_back(autoParam(base, "handleChangeMode", hands[i].controlData.getHandleChangeMode()));
				storeline.push_back(autoParam(base, "fixUp", hands[i].controlData.fixUp));
				storeline.push_back(closeObject(base));		// 5
				storeline.push_back(closeParam(base));		// 4
				storeline.push_back(autoParam(base, "handIndex", hands[i].handIndex));
				storeline.push_back(autoParam(base, "enable", hands[i].enable));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 20)
				{
					output.push_back("");	// 20 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			output.push_back(openParam(base, "controlData"));		// 4
			output.push_back(openObject(base));		// 5
			paramMatch("targetPosition", hands[i].controlData.targetPosition, ctrpart->hands[i].controlData.targetPosition, output, storeline, base, false, open, isEdited);
			paramMatch("targetRotation", hands[i].controlData.targetRotation, ctrpart->hands[i].controlData.targetRotation, output, storeline, base, false, open, isEdited);
			paramMatch("targetNormal", hands[i].controlData.targetNormal, ctrpart->hands[i].controlData.targetNormal, output, storeline, base, false, open, isEdited);
			paramMatch("targetHandle", hands[i].controlData.targetHandle, ctrpart->hands[i].controlData.targetHandle, output, storeline, base, false, open, isEdited);
			paramMatch("transformOnFraction", hands[i].controlData.transformOnFraction, ctrpart->hands[i].controlData.transformOnFraction, output, storeline, base, false, open, isEdited);
			paramMatch("normalOnFraction", hands[i].controlData.normalOnFraction, ctrpart->hands[i].controlData.normalOnFraction, output, storeline, base, false, open, isEdited);
			paramMatch("fadeInDuration", hands[i].controlData.fadeInDuration, ctrpart->hands[i].controlData.fadeInDuration, output, storeline, base, false, open, isEdited);
			paramMatch("fadeOutDuration", hands[i].controlData.fadeOutDuration, ctrpart->hands[i].controlData.fadeOutDuration, output, storeline, base, false, open, isEdited);
			paramMatch("extrapolationTimeStep", hands[i].controlData.extrapolationTimeStep, ctrpart->hands[i].controlData.extrapolationTimeStep, output, storeline, base, false, open, isEdited);
			paramMatch("handleChangeSpeed", hands[i].controlData.handleChangeSpeed, ctrpart->hands[i].controlData.handleChangeSpeed, output, storeline, base, false, open, isEdited);
			paramMatch("handleChangeMode", hands[i].controlData.getHandleChangeMode(), ctrpart->hands[i].controlData.getHandleChangeMode(), output, storeline, base, false, open, isEdited);
			paramMatch("fixUp", hands[i].controlData.fixUp, ctrpart->hands[i].controlData.fixUp, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));		// 5
			output.push_back(closeParam(base));		// 4
			paramMatch("handIndex", hands[i].handIndex, ctrpart->hands[i].handIndex, output, storeline, base, false, open, isEdited);
			paramMatch("enable", hands[i].enable, ctrpart->hands[i].enable, output, storeline, base, true, open, isEdited);
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

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", handikcontrolsmodifier::classname, output, isEdited);
}

void hkbhandikcontrolsmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = hands.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, handikcontrolsmodifier::classname, handikcontrolsmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));

	if (size == 0)
	{
		output.push_back(openParam(base, "hands", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "hands", size));		// 2

	for (auto& hand : hands)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(openParam(base, "controlData"));		// 4
		output.push_back(openObject(base));		// 5
		output.push_back(autoParam(base, "targetPosition", hand.controlData.targetPosition));
		output.push_back(autoParam(base, "targetRotation", hand.controlData.targetRotation));
		output.push_back(autoParam(base, "targetNormal", hand.controlData.targetNormal));
		output.push_back(autoParam(base, "targetHandle", hand.controlData.targetHandle));
		output.push_back(autoParam(base, "transformOnFraction", hand.controlData.transformOnFraction));
		output.push_back(autoParam(base, "normalOnFraction", hand.controlData.normalOnFraction));
		output.push_back(autoParam(base, "fadeInDuration", hand.controlData.fadeInDuration));
		output.push_back(autoParam(base, "fadeOutDuration", hand.controlData.fadeOutDuration));
		output.push_back(autoParam(base, "extrapolationTimeStep", hand.controlData.extrapolationTimeStep));
		output.push_back(autoParam(base, "handleChangeSpeed", hand.controlData.handleChangeSpeed));
		output.push_back(autoParam(base, "handleChangeMode", hand.controlData.getHandleChangeMode()));
		output.push_back(autoParam(base, "fixUp", hand.controlData.fixUp));
		output.push_back(closeObject(base));		// 5
		output.push_back(closeParam(base));		// 4
		output.push_back(autoParam(base, "handIndex", hand.handIndex));
		output.push_back(autoParam(base, "enable", hand.enable));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, handikcontrolsmodifier::classname, output, true);
}

void hkbhandikcontrolsmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& hand : hands)
	{
		if (hand.controlData.targetHandle) hkb_parent[hand.controlData.targetHandle] = shared_from_this();
	}
}

void hkbhandikcontrolsmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto& hand : hands)
		{
			if (hand.controlData.targetHandle)
			{
				parentRefresh();
				hand.controlData.targetHandle->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(hand.controlData.targetHandle->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& hand : hands)
		{
			if (hand.controlData.targetHandle)
			{
				parentRefresh();
				hand.controlData.targetHandle->connect(filepath, address, functionlayer, true, graphroot);
			}
		}
	}
}

void hkbhandikcontrolsmodifier::matchScoring(vector<hkhand>& ori, vector<hkhand>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<hkhand> newOri;
		vector<hkhand> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(hkhand());
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

			if (ori[i].controlData.targetPosition == edit[j].controlData.targetPosition)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.targetRotation == edit[j].controlData.targetRotation)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.targetNormal == edit[j].controlData.targetNormal)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.targetHandle->ID == edit[j].controlData.targetHandle->ID)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].controlData.transformOnFraction == edit[j].controlData.transformOnFraction)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.normalOnFraction == edit[j].controlData.normalOnFraction)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.fadeInDuration == edit[j].controlData.fadeInDuration)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.fadeOutDuration == edit[j].controlData.fadeOutDuration)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.extrapolationTimeStep == edit[j].controlData.extrapolationTimeStep)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.handleChangeSpeed == edit[j].controlData.handleChangeSpeed)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.handleChangeMode == edit[j].controlData.handleChangeMode)
			{
				++scorelist[i][j];
			}

			if (ori[i].controlData.fixUp == edit[j].controlData.fixUp)
			{
				++scorelist[i][j];
			}

			if (ori[i].handIndex == edit[j].handIndex)
			{
				scorelist[i][j] += 8;
			}

			if (ori[i].enable == edit[j].enable)
			{
				scorelist[i][j] += 5;
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
	vector<hkhand> newOri;
	vector<hkhand> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(hkhand());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(hkhand());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbhandikcontrolsmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
