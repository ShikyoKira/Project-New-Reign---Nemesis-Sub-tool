#include <boost\thread.hpp>
#include "hkbfootikmodifier.h"
#include "boolstring.h"
#include "highestscore.h"

using namespace std;

namespace footikmodifier
{
	const string key = "cu";
	const string classname = "hkbFootIkModifier";
	const string signature = "0xed8966c0";
}

safeStringUMap<shared_ptr<hkbfootikmodifier>> hkbfootikmodifierList;
safeStringUMap<shared_ptr<hkbfootikmodifier>> hkbfootikmodifierList_E;

void hkbfootikmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbfootikmodifierList_E[id] = shared_from_this() : hkbfootikmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbfootikmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("onOffGain", line, gains.onOffGain)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("groundAscendingGain", line, gains.groundAscendingGain)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("groundDescendingGain", line, gains.groundDescendingGain)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("footPlantedGain", line, gains.footPlantedGain)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("footRaisedGain", line, gains.footRaisedGain)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("footUnlockGain", line, gains.footUnlockGain)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("worldFromModelFeedbackGain", line, gains.worldFromModelFeedbackGain)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("errorUpDownBias", line, gains.errorUpDownBias)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("alignWorldFromModelGain", line, gains.alignWorldFromModelGain)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("hipOrientationGain", line, gains.hipOrientationGain)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("maxKneeAngleDifference", line, gains.maxKneeAngleDifference)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("ankleOrientationGain", line, gains.ankleOrientationGain)) ++type;

					break;
				}
				case 16:
				{
					usize numelement;

					if (readEleParam("legs", line, numelement))
					{
						legs.reserve(numelement);
						++type;
					}

					break;
				}
				case 17:
				{
					string output;

					if (readParam("originalAnkleTransformMS", line, output))
					{
						vecstr tokens;
						boost::trim_if(output, boost::is_any_of("\t ()"));
						boost::split(tokens, output, boost::is_any_of("\t ()"), boost::token_compress_on);
						legs.push_back(qstransform(stod(tokens[0]), stod(tokens[1]), stod(tokens[2]), stod(tokens[3]), stod(tokens[4]), stod(tokens[5]), stod(tokens[6]), stod(tokens[7]),
							stod(tokens[8]), stod(tokens[9])));

						++type;
						break;
					}

					if (readParam("raycastDistanceUp", line, raycastDistanceUp)) type = 38;

					break;
				}
				case 18:
				{
					if (readParam("kneeAxisLS", line, legs.back().kneeAxisLS)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("footEndLS", line, legs.back().footEndLS)) ++type;

					break;
				}
				case 20:
				{
					if (readParam("id", line, legs.back().ungroundedEvent.id)) ++type;

					break;
				}
				case 21:
				{
					string output;

					if (readParam("payload", line, output))
					{
						legs.back().ungroundedEvent.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 24:
				{
					if (readParam("footPlantedAnkleHeightMS", line, legs.back().footPlantedAnkleHeightMS)) ++type;

					break;
				}
				case 25:
				{
					if (readParam("footRaisedAnkleHeightMS", line, legs.back().footRaisedAnkleHeightMS)) ++type;

					break;
				}
				case 26:
				{
					if (readParam("maxAnkleHeightMS", line, legs.back().maxAnkleHeightMS)) ++type;

					break;
				}
				case 27:
				{
					if (readParam("minAnkleHeightMS", line, legs.back().minAnkleHeightMS)) ++type;

					break;
				}
				case 28:
				{
					if (readParam("maxKneeAngleDegrees", line, legs.back().maxKneeAngleDegrees)) ++type;

					break;
				}
				case 29:
				{
					if (readParam("minKneeAngleDegrees", line, legs.back().minKneeAngleDegrees)) ++type;

					break;
				}
				case 30:
				{
					if (readParam("verticalError", line, legs.back().verticalError)) ++type;

					break;
				}
				case 31:
				{
					if (readParam("maxAnkleAngleDegrees", line, legs.back().maxAnkleAngleDegrees)) ++type;

					break;
				}
				case 32:
				{
					if (readParam("hipIndex", line, legs.back().hipIndex)) ++type;

					break;
				}
				case 33:
				{
					if (readParam("kneeIndex", line, legs.back().kneeIndex)) ++type;

					break;
				}
				case 34:
				{
					if (readParam("ankleIndex", line, legs.back().ankleIndex)) ++type;

					break;
				}
				case 35:
				{
					if (readParam("hitSomething", line, legs.back().hitSomething)) ++type;

					break;
				}
				case 36:
				{
					if (readParam("isPlantedMS", line, legs.back().isPlantedMS)) ++type;

					break;
				}
				case 37:
				{
					if (readParam("isOriginalAnkleTransformMSSet", line, legs.back().isOriginalAnkleTransformMSSet)) type = 17;

					break;
				}
				case 38:
				{
					if (readParam("raycastDistanceDown", line, raycastDistanceDown)) ++type;
				}
				case 39:
				{
					if (readParam("originalGroundHeightMS", line, originalGroundHeightMS)) ++type;
				}
				case 40:
				{
					if (readParam("errorOut", line, errorOut)) ++type;
				}
				case 41:
				{
					if (readParam("errorOutTranslation", line, errorOutTranslation)) ++type;
				}
				case 42:
				{
					if (readParam("alignWithGroundRotation", line, alignWithGroundRotation)) ++type;
				}
				case 43:
				{
					if (readParam("verticalOffset", line, verticalOffset)) ++type;
				}
				case 44:
				{
					if (readParam("collisionFilterInfo", line, collisionFilterInfo)) ++type;
				}
				case 45:
				{
					if (readParam("forwardAlignFraction", line, forwardAlignFraction)) ++type;
				}
				case 46:
				{
					if (readParam("sidewaysAlignFraction", line, sidewaysAlignFraction)) ++type;
				}
				case 47:
				{
					if (readParam("sidewaysSampleWidth", line, sidewaysSampleWidth)) ++type;
				}
				case 48:
				{
					if (readParam("useTrackData", line, useTrackData)) ++type;
				}
				case 49:
				{
					if (readParam("lockFeetWhenPlanted", line, lockFeetWhenPlanted)) ++type;
				}
				case 50:
				{
					if (readParam("useCharacterUpVector", line, useCharacterUpVector)) ++type;
				}
				case 51:
				{
					string output;

					if (readParam("alignMode", line, output))
					{
						if (output == "ALIGN_MODE_FORWARD_RIGHT") alignMode = ALIGN_MODE_FORWARD_RIGHT;
						else alignMode = ALIGN_MODE_FORWARD;

						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << footikmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbfootikmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + footikmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	for (auto& leg : legs)
	{
		leg.ungroundedEvent.id.connectEventInfo(ID, graphroot);
	}

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
			if (addressID.find(address) != addressID.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbfootikmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbfootikmodifierList_E[ID] = protect;
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
				hkbfootikmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbfootikmodifierList_E[ID] = protect;
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

string hkbfootikmodifier::getClassCode()
{
	return footikmodifier::key;
}

void hkbfootikmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = legs.size();
	usize orisize = size;
	hkbfootikmodifier* ctrpart = static_cast<hkbfootikmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, footikmodifier::classname, footikmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "gains"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("onOffGain", gains.onOffGain, ctrpart->gains.onOffGain, output, storeline, base, false, open, isEdited);
	paramMatch("groundAscendingGain", gains.groundAscendingGain, ctrpart->gains.groundAscendingGain, output, storeline, base, false, open, isEdited);
	paramMatch("groundDescendingGain", gains.groundDescendingGain, ctrpart->gains.groundDescendingGain, output, storeline, base, false, open, isEdited);
	paramMatch("footPlantedGain", gains.footPlantedGain, ctrpart->gains.footPlantedGain, output, storeline, base, false, open, isEdited);
	paramMatch("footRaisedGain", gains.footRaisedGain, ctrpart->gains.footRaisedGain, output, storeline, base, false, open, isEdited);
	paramMatch("footUnlockGain", gains.footUnlockGain, ctrpart->gains.footUnlockGain, output, storeline, base, false, open, isEdited);
	paramMatch("worldFromModelFeedbackGain", gains.worldFromModelFeedbackGain, ctrpart->gains.worldFromModelFeedbackGain, output, storeline, base, false, open, isEdited);
	paramMatch("errorUpDownBias", gains.errorUpDownBias, ctrpart->gains.errorUpDownBias, output, storeline, base, false, open, isEdited);
	paramMatch("alignWorldFromModelGain", gains.alignWorldFromModelGain, ctrpart->gains.alignWorldFromModelGain, output, storeline, base, false, open, isEdited);
	paramMatch("hipOrientationGain", gains.hipOrientationGain, ctrpart->gains.hipOrientationGain, output, storeline, base, false, open, isEdited);
	paramMatch("maxKneeAngleDifference", gains.maxKneeAngleDifference, ctrpart->gains.maxKneeAngleDifference, output, storeline, base, false, open, isEdited);
	paramMatch("ankleOrientationGain", gains.ankleOrientationGain, ctrpart->gains.ankleOrientationGain, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2

	if (size == 0)
	{
		output.push_back(openParam(base, "legs", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "legs", size));		// 2

	matchScoring(legs, ctrpart->legs, ID);

	for (usize i = 0; i < legs.size(); ++i)
	{
		// newly created data
		if (legs[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "originalAnkleTransformMS", ctrpart->legs[i].originalAnkleTransformMS));
				output.push_back(autoParam(base, "kneeAxisLS", ctrpart->legs[i].kneeAxisLS));
				output.push_back(autoParam(base, "footEndLS", ctrpart->legs[i].footEndLS));
				output.push_back(openParam(base, "ungroundedEvent"));		// 4
				output.push_back(openObject(base));		// 5
				output.push_back(autoParam(base, "id", legs[i].ungroundedEvent.id));
				output.push_back(autoParam(base, "payload", legs[i].ungroundedEvent.payload));
				output.push_back(closeObject(base));	// 5
				output.push_back(closeParam(base));		// 4
				output.push_back(autoParam(base, "footPlantedAnkleHeightMS", ctrpart->legs[i].footPlantedAnkleHeightMS));
				output.push_back(autoParam(base, "footRaisedAnkleHeightMS", ctrpart->legs[i].footRaisedAnkleHeightMS));
				output.push_back(autoParam(base, "maxAnkleHeightMS", ctrpart->legs[i].maxAnkleHeightMS));
				output.push_back(autoParam(base, "minAnkleHeightMS", ctrpart->legs[i].minAnkleHeightMS));
				output.push_back(autoParam(base, "maxKneeAngleDegrees", ctrpart->legs[i].maxKneeAngleDegrees));
				output.push_back(autoParam(base, "minKneeAngleDegrees", ctrpart->legs[i].minKneeAngleDegrees));
				output.push_back(autoParam(base, "maxAnkleAngleDegrees", ctrpart->legs[i].maxAnkleAngleDegrees));
				output.push_back(autoParam(base, "hipIndex", ctrpart->legs[i].hipIndex));
				output.push_back(autoParam(base, "kneeIndex", ctrpart->legs[i].kneeIndex));
				output.push_back(autoParam(base, "ankleIndex", ctrpart->legs[i].ankleIndex));
				output.push_back(autoParam(base, "hitSomething", ctrpart->legs[i].hitSomething));
				output.push_back(autoParam(base, "isPlantedMS", ctrpart->legs[i].isPlantedMS));
				output.push_back(autoParam(base, "isOriginalAnkleTransformMSSet", ctrpart->legs[i].isOriginalAnkleTransformMSSet));
				output.push_back(closeObject(base));	// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->legs[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->legs[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "originalAnkleTransformMS", ctrpart->legs[i].originalAnkleTransformMS));
				storeline.push_back(autoParam(base, "kneeAxisLS", legs[i].kneeAxisLS.getString()));
				storeline.push_back(autoParam(base, "footEndLS", legs[i].footEndLS.getString()));
				storeline.push_back(openParam(base, "ungroundedEvent"));		// 4
				storeline.push_back(openObject(base));		// 5
				storeline.push_back(autoParam(base, "id", legs[i].ungroundedEvent.id));
				storeline.push_back(autoParam(base, "payload", legs[i].ungroundedEvent.payload));
				storeline.push_back(closeObject(base));		// 5
				storeline.push_back(closeParam(base));		// 4
				storeline.push_back(autoParam(base, "footPlantedAnkleHeightMS", legs[i].footPlantedAnkleHeightMS));
				storeline.push_back(autoParam(base, "footRaisedAnkleHeightMS", legs[i].footRaisedAnkleHeightMS));
				storeline.push_back(autoParam(base, "maxAnkleHeightMS", legs[i].maxAnkleHeightMS));
				storeline.push_back(autoParam(base, "minAnkleHeightMS", legs[i].minAnkleHeightMS));
				storeline.push_back(autoParam(base, "maxKneeAngleDegrees", legs[i].maxKneeAngleDegrees));
				storeline.push_back(autoParam(base, "minKneeAngleDegrees", legs[i].minKneeAngleDegrees));
				storeline.push_back(autoParam(base, "maxAnkleAngleDegrees", legs[i].maxAnkleAngleDegrees));
				storeline.push_back(autoParam(base, "hipIndex", legs[i].hipIndex));
				storeline.push_back(autoParam(base, "kneeIndex", legs[i].kneeIndex));
				storeline.push_back(autoParam(base, "ankleIndex", legs[i].ankleIndex));
				storeline.push_back(autoParam(base, "hitSomething", legs[i].hitSomething));
				storeline.push_back(autoParam(base, "isPlantedMS", legs[i].isPlantedMS));
				storeline.push_back(autoParam(base, "isOriginalAnkleTransformMSSet", legs[i].isOriginalAnkleTransformMSSet));
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 24)
				{
					output.push_back("");	// 24 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("originalAnkleTransformMS", legs[i].originalAnkleTransformMS, ctrpart->legs[i].originalAnkleTransformMS, output, storeline, base, false, open, isEdited);
			paramMatch("kneeAxisLS", legs[i].kneeAxisLS.getString(), ctrpart->legs[i].kneeAxisLS.getString(), output, storeline, base, false, open, isEdited);
			paramMatch("footEndLS", legs[i].footEndLS.getString(), ctrpart->legs[i].footEndLS.getString(), output, storeline, base, true, open, isEdited);
			output.push_back(openParam(base, "ungroundedEvent"));		// 4
			output.push_back(openObject(base));		// 5
			paramMatch("id", legs[i].ungroundedEvent.id, ctrpart->legs[i].ungroundedEvent.id, output, storeline, base, false, open, isEdited);
			paramMatch("payload", legs[i].ungroundedEvent.payload, ctrpart->legs[i].ungroundedEvent.payload, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));	// 5
			output.push_back(closeParam(base));		// 4
			paramMatch("footPlantedAnkleHeightMS", legs[i].footPlantedAnkleHeightMS, ctrpart->legs[i].footPlantedAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("footRaisedAnkleHeightMS", legs[i].footRaisedAnkleHeightMS, ctrpart->legs[i].footRaisedAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("maxAnkleHeightMS", legs[i].maxAnkleHeightMS, ctrpart->legs[i].maxAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("minAnkleHeightMS", legs[i].minAnkleHeightMS, ctrpart->legs[i].minAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("maxKneeAngleDegrees", legs[i].maxKneeAngleDegrees, ctrpart->legs[i].maxKneeAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("minKneeAngleDegrees", legs[i].minKneeAngleDegrees, ctrpart->legs[i].minKneeAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("maxAnkleAngleDegrees", legs[i].maxAnkleAngleDegrees, ctrpart->legs[i].maxAnkleAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("hipIndex", legs[i].hipIndex, ctrpart->legs[i].hipIndex, output, storeline, base, false, open, isEdited);
			paramMatch("kneeIndex", legs[i].kneeIndex, ctrpart->legs[i].kneeIndex, output, storeline, base, false, open, isEdited);
			paramMatch("ankleIndex", legs[i].ankleIndex, ctrpart->legs[i].ankleIndex, output, storeline, base, false, open, isEdited);
			paramMatch("hitSomething", legs[i].hitSomething, ctrpart->legs[i].hitSomething, output, storeline, base, false, open, isEdited);
			paramMatch("isPlantedMS", legs[i].isPlantedMS, ctrpart->legs[i].isPlantedMS, output, storeline, base, false, open, isEdited);
			paramMatch("isOriginalAnkleTransformMSSet", legs[i].isOriginalAnkleTransformMSSet, ctrpart->legs[i].isOriginalAnkleTransformMSSet, output, storeline, base, true, open, isEdited);
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

	paramMatch("raycastDistanceUp", raycastDistanceUp, ctrpart->raycastDistanceUp, output, storeline, base, false, open, isEdited);
	paramMatch("raycastDistanceDown", raycastDistanceDown, ctrpart->raycastDistanceDown, output, storeline, base, false, open, isEdited);
	paramMatch("originalGroundHeightMS", originalGroundHeightMS, ctrpart->originalGroundHeightMS, output, storeline, base, false, open, isEdited);
	paramMatch("errorOut", errorOut, ctrpart->errorOut, output, storeline, base, false, open, isEdited);
	paramMatch("errorOutTranslation", errorOutTranslation, ctrpart->errorOutTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("alignWithGroundRotation", alignWithGroundRotation, ctrpart->alignWithGroundRotation, output, storeline, base, false, open, isEdited);
	paramMatch("verticalOffset", verticalOffset, ctrpart->verticalOffset, output, storeline, base, false, open, isEdited);
	paramMatch("collisionFilterInfo", collisionFilterInfo, ctrpart->collisionFilterInfo, output, storeline, base, false, open, isEdited);
	paramMatch("forwardAlignFraction", forwardAlignFraction, ctrpart->forwardAlignFraction, output, storeline, base, false, open, isEdited);
	paramMatch("sidewaysAlignFraction", sidewaysAlignFraction, ctrpart->sidewaysAlignFraction, output, storeline, base, false, open, isEdited);
	paramMatch("sidewaysSampleWidth", sidewaysSampleWidth, ctrpart->sidewaysSampleWidth, output, storeline, base, false, open, isEdited);
	paramMatch("useTrackData", useTrackData, ctrpart->useTrackData, output, storeline, base, false, open, isEdited);
	paramMatch("lockFeetWhenPlanted", lockFeetWhenPlanted, ctrpart->lockFeetWhenPlanted, output, storeline, base, false, open, isEdited);
	paramMatch("useCharacterUpVector", useCharacterUpVector, ctrpart->useCharacterUpVector, output, storeline, base, false, open, isEdited);
	paramMatch("alignMode", getMode(), ctrpart->getMode(), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", footikmodifier::classname, output, isEdited);
}

void hkbfootikmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = legs.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, footikmodifier::classname, footikmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "gains"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "onOffGain", gains.onOffGain));
	output.push_back(autoParam(base, "groundAscendingGain", gains.groundAscendingGain));
	output.push_back(autoParam(base, "groundDescendingGain", gains.groundDescendingGain));
	output.push_back(autoParam(base, "footPlantedGain", gains.footPlantedGain));
	output.push_back(autoParam(base, "footRaisedGain", gains.footRaisedGain));
	output.push_back(autoParam(base, "footUnlockGain", gains.footUnlockGain));
	output.push_back(autoParam(base, "worldFromModelFeedbackGain", gains.worldFromModelFeedbackGain));
	output.push_back(autoParam(base, "errorUpDownBias", gains.errorUpDownBias));
	output.push_back(autoParam(base, "alignWorldFromModelGain", gains.alignWorldFromModelGain));
	output.push_back(autoParam(base, "hipOrientationGain", gains.hipOrientationGain));
	output.push_back(autoParam(base, "maxKneeAngleDifference", gains.maxKneeAngleDifference));
	output.push_back(autoParam(base, "ankleOrientationGain", gains.ankleOrientationGain));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2

	if (size == 0)
	{
		output.push_back(openParam(base, "legs", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "legs", size));		// 2

	for (auto& leg : legs)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "originalAnkleTransformMS", leg.originalAnkleTransformMS));
		output.push_back(autoParam(base, "kneeAxisLS", leg.kneeAxisLS.getString()));
		output.push_back(autoParam(base, "footEndLS", leg.footEndLS.getString()));
		output.push_back(openParam(base, "ungroundedEvent"));		// 4
		output.push_back(openObject(base));		// 5
		output.push_back(autoParam(base, "id", leg.ungroundedEvent.id));
		output.push_back(autoParam(base, "payload", leg.ungroundedEvent.payload));
		output.push_back(closeObject(base));	// 5
		output.push_back(closeParam(base));		// 4
		output.push_back(autoParam(base, "footPlantedAnkleHeightMS", leg.footPlantedAnkleHeightMS));
		output.push_back(autoParam(base, "footRaisedAnkleHeightMS", leg.footRaisedAnkleHeightMS));
		output.push_back(autoParam(base, "maxAnkleHeightMS", leg.maxAnkleHeightMS));
		output.push_back(autoParam(base, "minAnkleHeightMS", leg.minAnkleHeightMS));
		output.push_back(autoParam(base, "maxKneeAngleDegrees", leg.maxKneeAngleDegrees));
		output.push_back(autoParam(base, "minKneeAngleDegrees", leg.minKneeAngleDegrees));
		output.push_back(autoParam(base, "maxAnkleAngleDegrees", leg.maxAnkleAngleDegrees));
		output.push_back(autoParam(base, "hipIndex", leg.hipIndex));
		output.push_back(autoParam(base, "kneeIndex", leg.kneeIndex));
		output.push_back(autoParam(base, "ankleIndex", leg.ankleIndex));
		output.push_back(closeObject(base));	// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "raycastDistanceUp", raycastDistanceUp));
	output.push_back(autoParam(base, "raycastDistanceDown", raycastDistanceDown));
	output.push_back(autoParam(base, "originalGroundHeightMS", originalGroundHeightMS));
	output.push_back(autoParam(base, "errorOut", errorOut));
	output.push_back(autoParam(base, "errorOutTranslation", errorOutTranslation));
	output.push_back(autoParam(base, "alignWithGroundRotation", alignWithGroundRotation));
	output.push_back(autoParam(base, "verticalOffset", verticalOffset));
	output.push_back(autoParam(base, "collisionFilterInfo", collisionFilterInfo));
	output.push_back(autoParam(base, "forwardAlignFraction", forwardAlignFraction));
	output.push_back(autoParam(base, "sidewaysAlignFraction", sidewaysAlignFraction));
	output.push_back(autoParam(base, "sidewaysSampleWidth", sidewaysSampleWidth));
	output.push_back(autoParam(base, "useTrackData", useTrackData));
	output.push_back(autoParam(base, "lockFeetWhenPlanted", lockFeetWhenPlanted));
	output.push_back(autoParam(base, "useCharacterUpVector", useCharacterUpVector));
	output.push_back(autoParam(base, "alignMode", alignMode));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, footikmodifier::classname, output, true);
}

void hkbfootikmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbfootikmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto& leg : legs)
		{
			if (leg.ungroundedEvent.payload)
			{
				parentRefresh();
				leg.ungroundedEvent.payload->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

				if (IsForeign.find(leg.ungroundedEvent.payload->ID) == IsForeign.end()) ++tempint;
			}
		}
	}
	else
	{
		for (auto& leg : legs)
		{
			parentRefresh();
			if (leg.ungroundedEvent.payload) leg.ungroundedEvent.payload->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbfootikmodifier::matchScoring(vector<hkbfootikmodifier::leg>& ori, vector<hkbfootikmodifier::leg>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<hkbfootikmodifier::leg> newOri;
		vector<hkbfootikmodifier::leg> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(hkbfootikmodifier::leg());
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

			if (ori[i].originalAnkleTransformMS == edit[j].originalAnkleTransformMS)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i].kneeAxisLS == edit[j].kneeAxisLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].footEndLS == edit[j].footEndLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].ungroundedEvent.id == edit[j].ungroundedEvent.id)
			{
				++scorelist[i][j];
			}

			if (matchIDCompare(ori[i].ungroundedEvent.payload, edit[j].ungroundedEvent.payload))
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].footPlantedAnkleHeightMS == edit[j].footPlantedAnkleHeightMS)
			{
				++scorelist[i][j];
			}

			if (ori[i].footRaisedAnkleHeightMS == edit[j].footRaisedAnkleHeightMS)
			{
				++scorelist[i][j];
			}

			if (ori[i].maxAnkleHeightMS == edit[j].maxAnkleHeightMS)
			{
				++scorelist[i][j];
			}

			if (ori[i].minAnkleHeightMS == edit[j].minAnkleHeightMS)
			{
				++scorelist[i][j];
			}

			if (ori[i].maxKneeAngleDegrees == edit[j].maxKneeAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].minKneeAngleDegrees == edit[j].minKneeAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].verticalError == edit[j].verticalError)
			{
				++scorelist[i][j];
			}

			if (ori[i].maxAnkleAngleDegrees == edit[j].maxAnkleAngleDegrees)
			{
				++scorelist[i][j];
			}

			if (ori[i].hipIndex == edit[j].hipIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].kneeIndex == edit[j].kneeIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].ankleIndex == edit[j].ankleIndex)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i].hitSomething == edit[j].hitSomething)
			{
				++scorelist[i][j];
			}

			if (ori[i].isPlantedMS == edit[j].isPlantedMS)
			{
				++scorelist[i][j];
			}

			if (ori[i].isOriginalAnkleTransformMSSet == edit[j].isOriginalAnkleTransformMSSet)
			{
				++scorelist[i][j];
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
	vector<leg> newOri;
	vector<leg> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(leg());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(leg());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

string hkbfootikmodifier::getMode()
{
	switch (alignMode)
	{
		case ALIGN_MODE_FORWARD_RIGHT: return "ALIGN_MODE_FORWARD_RIGHT";
		case ALIGN_MODE_FORWARD: return "MODE_COUNT";
		default: return "ALIGN_MODE_FORWARD_RIGHT";
	}
}
