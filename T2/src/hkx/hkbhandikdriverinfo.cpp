#include <boost\regex.hpp>
#include "hkbhandikdriverinfo.h"
#include "highestscore.h"

using namespace std;

namespace handikdriverinfo
{
	string key = "cr";
	string classname = "hkbHandIkDriverInfo";
	string signature = "0xc299090a";
}

safeStringUMap<shared_ptr<hkbhandikdriverinfo>> hkbhandikdriverinfoList;
safeStringUMap<shared_ptr<hkbhandikdriverinfo>> hkbhandikdriverinfoList_E;

void hkbhandikdriverinfo::regis(string id, bool isEdited)
{
	isEdited ? hkbhandikdriverinfoList_E[id] = shared_from_this() : hkbhandikdriverinfoList[id] = shared_from_this();
	ID = id;
}

void hkbhandikdriverinfo::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("hands", line, numelement))
				{
					hands.reserve(numelement);
					++type;
				}
			}
			case 1:
			{
				coordinate output;

				if (readParam("elbowAxisLS", line, output))
				{
					hands.push_back(output);
					++type;
					break;
				}

				string data;

				if (readParam("fadeInOutCurve", line, data))
				{
					if (data == "BLEND_CURVE_SMOOTH") fadeInOutCurve = BLEND_CURVE_SMOOTH;
					else if (data == "BLEND_CURVE_LINEAR") fadeInOutCurve = BLEND_CURVE_LINEAR;
					else if (data == "BLEND_CURVE_LINEAR_TO_SMOOTH") fadeInOutCurve = BLEND_CURVE_LINEAR_TO_SMOOTH;
					else fadeInOutCurve = BLEND_CURVE_SMOOTH_TO_LINEAR;

						type = 13;
				}
					
				break;
			}
			case 2:
			{
				if (readParam("backHandNormalLS", line, hands.back().backHandNormalLS)) ++type;

				break;
			}
			case 3:
			{
				if (readParam("handOffsetLS", line, hands.back().handOffsetLS)) ++type;

				break;
			}
			case 4:
			{
				if (readParam("handOrienationOffsetLS", line, hands.back().handOrienationOffsetLS)) ++type;

				break;
			}
			case 5:
			{
				if (readParam("maxElbowAngleDegrees", line, hands.back().maxElbowAngleDegrees)) ++type;

				break;
			}
			case 6:
			{
				if (readParam("minElbowAngleDegrees", line, hands.back().minElbowAngleDegrees)) ++type;

				break;
			}
			case 7:
			{
				if (readParam("shoulderIndex", line, hands.back().shoulderIndex)) ++type;

				break;
			}
			case 8:
			{
				if (readParam("shoulderSiblingIndex", line, hands.back().shoulderSiblingIndex)) ++type;

				break;
			}
			case 9:
			{
				if (readParam("elbowIndex", line, hands.back().elbowIndex)) ++type;

				break;
			}
			case 10:
			{
				if (readParam("elbowSiblingIndex", line, hands.back().elbowSiblingIndex)) ++type;

				break;
			}
			case 11:
			{
				if (readParam("wristIndex", line, hands.back().wristIndex)) ++type;

				break;
			}
			case 12:
			{
				if (readParam("enforceEndPosition", line, hands.back().enforceEndPosition)) ++type;

				break;
			}
			case 13:
			{
				if (readParam("enforceEndRotation", line, hands.back().enforceEndRotation)) ++type;

				break;
			}
			case 14:
			{
				if (readParam("localFrameName", line, hands.back().localFrameName)) type = 1;

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << handikdriverinfo::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbhandikdriverinfo::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + handikdriverinfo::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			// existed
			if (addressID.find(address) != addressID.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbhandikdriverinfoList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbhandikdriverinfoList_E[ID] = protect;
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
				hkbhandikdriverinfoList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbhandikdriverinfoList_E[ID] = protect;
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

string hkbhandikdriverinfo::getClassCode()
{
	return handikdriverinfo::key;
}

void hkbhandikdriverinfo::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = hands.size();
	usize orisize = size;
	hkbhandikdriverinfo* ctrpart = static_cast<hkbhandikdriverinfo*>(counterpart.get());

	output.push_back(openObject(base, ID, handikdriverinfo::classname, handikdriverinfo::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "hands", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "hands", size));		// 2

	matchScoring(hands, ctrpart->hands, ID);

	for (usize i = 0; i < hands.size(); ++i)
	{
		// newly created data
		if (hands[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "elbowAxisLS", ctrpart->hands[i].elbowAxisLS));
				output.push_back(autoParam(base, "backHandNormalLS", ctrpart->hands[i].backHandNormalLS));
				output.push_back(autoParam(base, "handOffsetLS", ctrpart->hands[i].handOffsetLS.getString()));
				output.push_back(autoParam(base, "handOrienationOffsetLS", ctrpart->hands[i].handOrienationOffsetLS));
				output.push_back(autoParam(base, "maxElbowAngleDegrees", ctrpart->hands[i].maxElbowAngleDegrees));
				output.push_back(autoParam(base, "minElbowAngleDegrees", ctrpart->hands[i].minElbowAngleDegrees));
				output.push_back(autoParam(base, "shoulderIndex", ctrpart->hands[i].shoulderIndex));
				output.push_back(autoParam(base, "shoulderSiblingIndex", ctrpart->hands[i].shoulderSiblingIndex));
				output.push_back(autoParam(base, "elbowIndex", ctrpart->hands[i].elbowIndex));
				output.push_back(autoParam(base, "elbowSiblingIndex", ctrpart->hands[i].elbowSiblingIndex));
				output.push_back(autoParam(base, "wristIndex", ctrpart->hands[i].wristIndex));
				output.push_back(autoParam(base, "enforceEndPosition", ctrpart->hands[i].enforceEndPosition));
				output.push_back(autoParam(base, "enforceEndRotation", ctrpart->hands[i].enforceEndRotation));
				output.push_back(autoParam(base, "localFrameName", ctrpart->hands[i].localFrameName));
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
				storeline.push_back(autoParam(base, "elbowAxisLS", hands[i].elbowAxisLS.getString()));
				storeline.push_back(autoParam(base, "backHandNormalLS", hands[i].backHandNormalLS.getString()));
				storeline.push_back(autoParam(base, "handOffsetLS", hands[i].handOffsetLS.getString()));
				storeline.push_back(autoParam(base, "handOrienationOffsetLS", hands[i].handOrienationOffsetLS.getString()));
				storeline.push_back(autoParam(base, "maxElbowAngleDegrees", hands[i].maxElbowAngleDegrees));
				storeline.push_back(autoParam(base, "minElbowAngleDegrees", hands[i].minElbowAngleDegrees));
				storeline.push_back(autoParam(base, "shoulderIndex", hands[i].shoulderIndex));
				storeline.push_back(autoParam(base, "shoulderSiblingIndex", hands[i].shoulderSiblingIndex));
				storeline.push_back(autoParam(base, "elbowIndex", hands[i].elbowIndex));
				storeline.push_back(autoParam(base, "elbowSiblingIndex", hands[i].elbowSiblingIndex));
				storeline.push_back(autoParam(base, "wristIndex", hands[i].wristIndex));
				storeline.push_back(autoParam(base, "enforceEndPosition", hands[i].enforceEndPosition));
				storeline.push_back(autoParam(base, "enforceEndRotation", hands[i].enforceEndRotation));
				storeline.push_back(autoParam(base, "localFrameName", hands[i].localFrameName));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 16)
				{
					output.push_back("");	// 16 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("elbowAxisLS", hands[i].elbowAxisLS.getString(), ctrpart->hands[i].elbowAxisLS.getString(), output, storeline, base, false, open, isEdited);
			paramMatch("backHandNormalLS", hands[i].backHandNormalLS.getString(), ctrpart->hands[i].backHandNormalLS.getString(), output, storeline, base, false, open,
				isEdited);
			paramMatch("handOffsetLS", hands[i].handOffsetLS.getString(), ctrpart->hands[i].handOffsetLS.getString(), output, storeline, base, false, open, isEdited);
			paramMatch("handOrienationOffsetLS", hands[i].handOrienationOffsetLS.getString(), ctrpart->hands[i].handOrienationOffsetLS.getString(), output, storeline,
				base, false, open, isEdited);
			paramMatch("maxElbowAngleDegrees", hands[i].maxElbowAngleDegrees, ctrpart->hands[i].maxElbowAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("minElbowAngleDegrees", hands[i].minElbowAngleDegrees, ctrpart->hands[i].minElbowAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("shoulderIndex", hands[i].shoulderIndex, ctrpart->hands[i].shoulderIndex, output, storeline, base, false, open, isEdited);
			paramMatch("shoulderSiblingIndex", hands[i].shoulderSiblingIndex, ctrpart->hands[i].shoulderSiblingIndex, output, storeline, base, false, open, isEdited);
			paramMatch("elbowIndex", hands[i].elbowIndex, ctrpart->hands[i].elbowIndex, output, storeline, base, false, open, isEdited);
			paramMatch("elbowSiblingIndex", hands[i].elbowSiblingIndex, ctrpart->hands[i].elbowSiblingIndex, output, storeline, base, false, open, isEdited);
			paramMatch("wristIndex", hands[i].wristIndex, ctrpart->hands[i].wristIndex, output, storeline, base, false, open, isEdited);
			paramMatch("enforceEndPosition", hands[i].enforceEndPosition, ctrpart->hands[i].enforceEndPosition, output, storeline, base, false, open, isEdited);
			paramMatch("enforceEndRotation", hands[i].enforceEndRotation, ctrpart->hands[i].enforceEndRotation, output, storeline, base, false, open, isEdited);
			paramMatch("localFrameName", hands[i].localFrameName, ctrpart->hands[i].localFrameName, output, storeline, base, true, open, isEdited);
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

	paramMatch("fadeInOutCurve", getBlendCurve(fadeInOutCurve), getBlendCurve(ctrpart->fadeInOutCurve), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", handikdriverinfo::classname, output, isEdited);
}

void hkbhandikdriverinfo::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = hands.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, handikdriverinfo::classname, handikdriverinfo::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "hands", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "hands", size));		// 2

	for (auto& hand : hands)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "elbowAxisLS", hand.elbowAxisLS.getString()));
		output.push_back(autoParam(base, "backHandNormalLS", hand.backHandNormalLS.getString()));
		output.push_back(autoParam(base, "handOffsetLS", hand.handOffsetLS.getString()));
		output.push_back(autoParam(base, "handOrienationOffsetLS", hand.handOrienationOffsetLS.getString()));
		output.push_back(autoParam(base, "maxElbowAngleDegrees", hand.maxElbowAngleDegrees));
		output.push_back(autoParam(base, "minElbowAngleDegrees", hand.minElbowAngleDegrees));
		output.push_back(autoParam(base, "shoulderIndex", hand.shoulderIndex));
		output.push_back(autoParam(base, "shoulderSiblingIndex", hand.shoulderSiblingIndex));
		output.push_back(autoParam(base, "elbowIndex", hand.elbowIndex));
		output.push_back(autoParam(base, "elbowSiblingIndex", hand.elbowSiblingIndex));
		output.push_back(autoParam(base, "wristIndex", hand.wristIndex));
		output.push_back(autoParam(base, "enforceEndPosition", hand.enforceEndPosition));
		output.push_back(autoParam(base, "enforceEndRotation", hand.enforceEndRotation));
		output.push_back(autoParam(base, "localFrameName", hand.localFrameName));
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "fadeInOutCurve", getBlendCurve(fadeInOutCurve)));
	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, handikdriverinfo::classname, output, true);
}

void hkbhandikdriverinfo::matchScoring(vector<hkbhand>& ori, vector<hkbhand>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<hkbhand> newOri;
		vector<hkbhand> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(hkbhand());
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

			if (ori[i].elbowAxisLS == edit[j].elbowAxisLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].backHandNormalLS == edit[j].backHandNormalLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].handOffsetLS == edit[j].handOffsetLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].handOrienationOffsetLS == edit[j].handOrienationOffsetLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].maxElbowAngleDegrees == edit[j].maxElbowAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].minElbowAngleDegrees == edit[j].minElbowAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].shoulderIndex == edit[j].shoulderIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].shoulderSiblingIndex == edit[j].shoulderSiblingIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].elbowIndex == edit[j].elbowIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].elbowSiblingIndex == edit[j].elbowSiblingIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].wristIndex == edit[j].wristIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].enforceEndPosition == edit[j].enforceEndPosition)
			{
				++scorelist[i][j];
			}

			if (ori[i].enforceEndRotation == edit[j].enforceEndRotation)
			{
				++scorelist[i][j];
			}

			if (ori[i].localFrameName == edit[j].localFrameName)
			{
				scorelist[i][j] += 5;
			}

			if (i == j)
			{
				++scorelist[i][j];
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<hkbhand> newOri;
	vector<hkbhand> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(hkbhand());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(hkbhand());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}
