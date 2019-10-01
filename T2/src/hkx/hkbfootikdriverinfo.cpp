#include "hkbfootikdriverinfo.h"
#include "highestscore.h"

using namespace std;

namespace footikdriverinfo
{
	string key = "cd";
	string classname = "hkbFootIkDriverInfo";
	string signature = "0xc6a09dbf";
}

hkbfootikdriverinfo::hkbfootikdriverinfo(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + footikdriverinfo::key + to_string(functionlayer) + ">";

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

void hkbfootikdriverinfo::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkDriverInfo(ID: " << id << ") has been initialized!" << endl;
	}

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbFootIkDriverInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkDriverInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikdriverinfo::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkDriverInfo(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbFootIkDriverInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbFootIkDriverInfo(newID: " << id << ") with hkbFootIkDriverInfo(oldID: " << tempid << ")" << endl;
		}

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
		{
			FunctionLineNew[tempid].push_back(FunctionLineEdited[id][i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbFootIkDriverInfo(newID: " << id << ") with hkbFootIkDriverInfo(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = FunctionLineEdited[id];
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkDriverInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikdriverinfo::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbFootIkDriverInfo(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbFootIkDriverInfo Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbFootIkDriverInfo(ID: " << id << ") is complete!" << endl;
	}
}

string hkbfootikdriverinfo::GetAddress()
{
	return address;
}

bool hkbfootikdriverinfo::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbfootikdriverinfo>> hkbfootikdriverinfoList;
safeStringUMap<shared_ptr<hkbfootikdriverinfo>> hkbfootikdriverinfoList_E;

void hkbfootikdriverinfo::regis(string id, bool isEdited)
{
	isEdited ? hkbfootikdriverinfoList_E[id] = shared_from_this() : hkbfootikdriverinfoList[id] = shared_from_this();
	ID = id;
}

void hkbfootikdriverinfo::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos)
		{
			switch(type)
			{
				case 0:
				{
					usize numelement;

					if (readEleParam("legs", line, numelement))
					{
						legs.reserve(numelement);
						++type;
					}
				}
				case 1:
				{
					coordinate output;

					if (readParam("kneeAxisLS", line, output))
					{
						legs.push_back(output);
						++type;
						break;
					}

					if (readParam("raycastDistanceUp", line, raycastDistanceUp)) type = 13;
					
					break;
				}
				case 2:
				{
					if (readParam("footEndLS", line, legs.back().footEndLS)) ++type;

					break;
				}
				case 3:
				{
					if (readParam("footPlantedAnkleHeightMS", line, legs.back().footPlantedAnkleHeightMS)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("footRaisedAnkleHeightMS", line, legs.back().footRaisedAnkleHeightMS)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("maxAnkleHeightMS", line, legs.back().maxAnkleHeightMS)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("minAnkleHeightMS", line, legs.back().minAnkleHeightMS)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("maxKneeAngleDegrees", line, legs.back().maxKneeAngleDegrees)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("minKneeAngleDegrees", line, legs.back().minKneeAngleDegrees)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("maxAnkleAngleDegrees", line, legs.back().maxAnkleAngleDegrees)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("hipIndex", line, legs.back().hipIndex)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("kneeIndex", line, legs.back().kneeIndex)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("ankleIndex", line, legs.back().ankleIndex)) type = 1;

					break;
				}
				case 13:
				{
					if (readParam("raycastDistanceDown", line, raycastDistanceDown)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("originalGroundHeightMS", line, originalGroundHeightMS)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("verticalOffset", line, verticalOffset)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("collisionFilterInfo", line, collisionFilterInfo)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("forwardAlignFraction", line, forwardAlignFraction)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("sidewaysAlignFraction", line, sidewaysAlignFraction)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("sidewaysSampleWidth", line, sidewaysSampleWidth)) ++type;

					break;
				}
				case 20:
				{
					if (readParam("lockFeetWhenPlanted", line, lockFeetWhenPlanted)) ++type;

					break;
				}
				case 21:
				{
					if (readParam("useCharacterUpVector", line, useCharacterUpVector)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("isQuadrupedNarrow", line, isQuadrupedNarrow)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << footikdriverinfo::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbfootikdriverinfo::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + footikdriverinfo::key + to_string(functionlayer) + ">";
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
				hkbfootikdriverinfoList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbfootikdriverinfoList_E[ID] = protect;
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
				hkbfootikdriverinfoList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbfootikdriverinfoList_E[ID] = protect;
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

string hkbfootikdriverinfo::getClassCode()
{
	return footikdriverinfo::key;
}

void hkbfootikdriverinfo::match(shared_ptr<hkbobject> counterpart)
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
	hkbfootikdriverinfo* ctrpart = static_cast<hkbfootikdriverinfo*>(counterpart.get());

	output.push_back(openObject(base, ID, footikdriverinfo::classname, footikdriverinfo::signature));		// 1

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
				output.push_back(autoParam(base, "kneeAxisLS", ctrpart->legs[i].kneeAxisLS));
				output.push_back(autoParam(base, "footEndLS", ctrpart->legs[i].footEndLS));
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
				output.push_back(closeObject(base));		// 3
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
				storeline.push_back(autoParam(base, "kneeAxisLS", legs[i].kneeAxisLS.getString()));
				storeline.push_back(autoParam(base, "footEndLS", legs[i].footEndLS.getString()));
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
				storeline.push_back(closeObject(base));		// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 14)
				{
					output.push_back("");	// 14 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("kneeAxisLS", legs[i].kneeAxisLS.getString(), ctrpart->legs[i].kneeAxisLS.getString(), output, storeline, base, false, open, isEdited);
			paramMatch("footEndLS", legs[i].footEndLS.getString(), ctrpart->legs[i].footEndLS.getString(), output, storeline, base, false, open, isEdited);
			paramMatch("footPlantedAnkleHeightMS", legs[i].footPlantedAnkleHeightMS, ctrpart->legs[i].footPlantedAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("footRaisedAnkleHeightMS", legs[i].footRaisedAnkleHeightMS, ctrpart->legs[i].footRaisedAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("maxAnkleHeightMS", legs[i].maxAnkleHeightMS, ctrpart->legs[i].maxAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("minAnkleHeightMS", legs[i].minAnkleHeightMS, ctrpart->legs[i].minAnkleHeightMS, output, storeline, base, false, open, isEdited);
			paramMatch("maxKneeAngleDegrees", legs[i].maxKneeAngleDegrees, ctrpart->legs[i].maxKneeAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("minKneeAngleDegrees", legs[i].minKneeAngleDegrees, ctrpart->legs[i].minKneeAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("maxAnkleAngleDegrees", legs[i].maxAnkleAngleDegrees, ctrpart->legs[i].maxAnkleAngleDegrees, output, storeline, base, false, open, isEdited);
			paramMatch("hipIndex", legs[i].hipIndex, ctrpart->legs[i].hipIndex, output, storeline, base, false, open, isEdited);
			paramMatch("kneeIndex", legs[i].kneeIndex, ctrpart->legs[i].kneeIndex, output, storeline, base, false, open, isEdited);
			paramMatch("ankleIndex", legs[i].ankleIndex, ctrpart->legs[i].ankleIndex, output, storeline, base, true, open, isEdited);
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
	paramMatch("verticalOffset", verticalOffset, ctrpart->verticalOffset, output, storeline, base, false, open, isEdited);
	paramMatch("collisionFilterInfo", collisionFilterInfo, ctrpart->collisionFilterInfo, output, storeline, base, false, open, isEdited);
	paramMatch("forwardAlignFraction", forwardAlignFraction, ctrpart->forwardAlignFraction, output, storeline, base, false, open, isEdited);
	paramMatch("sidewaysAlignFraction", sidewaysAlignFraction, ctrpart->sidewaysAlignFraction, output, storeline, base, false, open, isEdited);
	paramMatch("sidewaysSampleWidth", sidewaysSampleWidth, ctrpart->sidewaysSampleWidth, output, storeline, base, false, open, isEdited);
	paramMatch("lockFeetWhenPlanted", lockFeetWhenPlanted, ctrpart->lockFeetWhenPlanted, output, storeline, base, false, open, isEdited);
	paramMatch("useCharacterUpVector", useCharacterUpVector, ctrpart->useCharacterUpVector, output, storeline, base, false, open, isEdited);
	paramMatch("isQuadrupedNarrow", isQuadrupedNarrow, ctrpart->isQuadrupedNarrow, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", footikdriverinfo::classname, output, isEdited);
}

void hkbfootikdriverinfo::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = legs.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, footikdriverinfo::classname, footikdriverinfo::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "legs", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "legs", size));		// 2

	for (auto& leg : legs)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "kneeAxisLS", leg.kneeAxisLS.getString()));
		output.push_back(autoParam(base, "footEndLS", leg.footEndLS.getString()));
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
		output.push_back(closeObject(base));		// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "raycastDistanceUp", raycastDistanceUp));
	output.push_back(autoParam(base, "raycastDistanceDown", raycastDistanceDown));
	output.push_back(autoParam(base, "originalGroundHeightMS", originalGroundHeightMS));
	output.push_back(autoParam(base, "verticalOffset", verticalOffset));
	output.push_back(autoParam(base, "collisionFilterInfo", collisionFilterInfo));
	output.push_back(autoParam(base, "forwardAlignFraction", forwardAlignFraction));
	output.push_back(autoParam(base, "sidewaysAlignFraction", sidewaysAlignFraction));
	output.push_back(autoParam(base, "sidewaysSampleWidth", sidewaysSampleWidth));
	output.push_back(autoParam(base, "lockFeetWhenPlanted", lockFeetWhenPlanted));
	output.push_back(autoParam(base, "useCharacterUpVector", useCharacterUpVector));
	output.push_back(autoParam(base, "isQuadrupedNarrow", isQuadrupedNarrow));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, footikdriverinfo::classname, output, true);
}

void hkbfootikdriverinfo::matchScoring(vector<leg>& ori, vector<leg>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<leg> newOri;
		vector<leg> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(leg());
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

			if (ori[i].kneeAxisLS == edit[j].kneeAxisLS)
			{
				++scorelist[i][j];
			}

			if (ori[i].footEndLS == edit[j].footEndLS)
			{
				++scorelist[i][j];
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

void hkbFootIkDriverInfoExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkDriverInfo Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}

		storeline2.shrink_to_fit();
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkDriverInfo Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vecstr output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	bool IsEditedClose = false;
	bool IsEditedOpen = false;
	int openpoint;
	int closepoint;
	int curline = 0;
	int part = 0;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"raycastDistanceUp\">", 0) == string::npos && part == 0) // existing variable value
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
			{
				if (open)
				{
					if (IsChanged)
					{
						closepoint = curline;

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						IsChanged = false;
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = i;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}

			curline++;
			output.push_back(storeline2[i]);

			if (curline != storeline1.size() && storeline1[curline].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos && storeline1[curline].find(storeline2[i + 1], 0) == string::npos)
			{
				output.push_back("				</hkobject>");
			}
		}
		else if (part == 1) // existing leftover settings
		{
			if (storeline2[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos)
			{
				output.push_back("				</hkobject>");
			}

			if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					closepoint = curline;

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}

			curline++;
			output.push_back(storeline2[i]);			
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos) // existing variable value
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}

				output.push_back(storeline2[i]);
				part = 1;
				curline++;
			}
			else
			{
				if (part == 0)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					if (storeline2[i].find("<hkparam name=\"kneeAxisLS\">", 0) != string::npos) // existing variable value
					{
						output.push_back("				<hkobject>");
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos)
					{
						output.push_back("				</hkobject>");

						if (IsChanged)
						{
							if (openpoint != curline)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
								{
									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"ankleIndex\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}
								}
							}

							IsChanged = false;
						}

						output.push_back("<!-- CLOSE -->");
						output.push_back("			</hkparam>");
						open = false;
					}
				}
			}
		}
	}

	if (open) // close unclosed edits
	{
		if (IsChanged)
		{
			closepoint = curline;

			if (closepoint != openpoint)
			{
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
			}

			IsChanged = false;
		}

		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (((output[i].find("OPEN", 0) != string::npos) && (output[i].find("MOD_CODE", 0) != string::npos)))
				{
					if (output[i + 1].find("<hkparam name=\"kneeAxisLS\">", 0) != string::npos && output[i - 1].find("<hkobject6>", 0) == string::npos)
					{
						if (i > 10)
						{
							fwrite << "				</hkobject>" << "\n";
						}

						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i + 1].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos && output[i - 1].find("</hkparam>", 0) == string::npos)
					{
						fwrite << "			</hkparam>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"kneeAxisLS\">", 0) != string::npos)
				{
					if ((output[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos) || (output[i - 1].find("<!-- CLOSE -->", 0) != string::npos))
					{
						if (output[i - 2].find("<hkparam name=\"legs\" numelements=") == string::npos)
						{
							fwrite << "				</hkobject>" << "\n";
						}

						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i - 1].find("<hkparam name=\"legs\" numelements=") != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i - 1].find("</hkobject>") != string::npos && output[i - 2].find("<hkparam name=\"ankleIndex\">") != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
				}
				
				if (output[i].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos)
				{
					if ((output[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos))
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}
					else if ((output[i - 1].find("</hkobject>", 0) != string::npos))
					{
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<!-- CLOSE -->", 0) != string::npos)
				{
					if (output[i - 1].find("<hkparam name=\"ankleIndex\">", 0) != string::npos && i + 1 != output.size() && output[i + 1].find("<hkparam name=\"raycastDistanceUp\">", 0) != string::npos)
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbFootIkDriverInfo Output Not Found (File: " << filename << ")" << endl;
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