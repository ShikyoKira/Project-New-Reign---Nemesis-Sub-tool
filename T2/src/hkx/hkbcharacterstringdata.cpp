#include <boost\thread.hpp>
#include "hkbcharacterstringdata.h"
#include "highestscore.h"
#include "src\utilities\stringdatalock.h"
#include "src\utilities\levenshteindistance.h"

using namespace std;

boost::atomic_flag datalock_c = BOOST_ATOMIC_FLAG_INIT;

namespace characterstringdata
{
	string key = "cc";
	string classname = "hkbCharacterStringData";
	string signature = "0x655b42bc";
}

void stringListCompare(vecstr& ori, usize i, vecstr& edit, map<int, map<int, double>>& score, boost::atomic_flag& lock);

hkbcharacterstringdata::hkbcharacterstringdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + characterstringdata::key + to_string(functionlayer) + ">";

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
}

void hkbcharacterstringdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterStringData(ID: " << id << ") has been initialized!" << endl;
	}

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbCharacterStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbCharacterStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterstringdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterStringData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		newline = FunctionLineEdited[id];
	}
	else
	{
		cout << "ERROR: hkbCharacterStringData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
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
			cout << "Comparing hkbCharacterStringData(newID: " << id << ") with hkbCharacterStringData(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		vecstr storeline = FunctionLineTemp[tempid];

		// stage 3
		int curline = 1;
		bool IsNewChild = false;
		vecstr newstoreline;
		vecstr newchild;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
			if (!IsNewChild)
			{
				if ((newline[i].find("<hkparam name=\"generators\" numelements=", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
				}

				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].length() < 15))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].length() < 15))
					{
						for (unsigned int j = 0; j < newchild.size(); j++)
						{
							newstoreline.push_back(newchild[j]);
						}

						newstoreline.push_back(newline[i]);
						curline++;
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}
				}
				else if (storeline[curline].find(newline[i], 0) == string::npos)
				{
					usize size = count(storeline[curline].begin(), storeline[curline].end(), '#');
					usize size2 = count(newline[i].begin(), newline[i].end(), '#');

					if (size < size2)
					{
						usize position = 0;
						usize tempint = 0;

						for (unsigned int j = 0; j < size + 1; j++)
						{
							position = newline[i].find("#", tempint);
							tempint = newline[i].find("#", position + 1);
						}

						newstoreline.push_back(newline[i].substr(0, position - 1));
						newchild.push_back("				" + newline[i].substr(position, -1));
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}

					curline++;
				}
				else
				{
					newstoreline.push_back(newline[i]);
					curline++;
				}
			}
		}

		FunctionLineNew[tempid] = newstoreline;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbCharacterStringData(newID: " << id << ") with hkbCharacterStringData(oldID: " << tempid << ") is complete!" << endl;
		}

		address = region[tempid];
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
		cout << "hkbCharacterStringData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterstringdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbCharacterStringData(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbCharacterStringData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbCharacterStringData (ID: " << id << ") is complete!" << endl;
	}
}

string hkbcharacterstringdata::GetAddress()
{
	return address;
}

bool hkbcharacterstringdata::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbcharacterstringdata>> hkbcharacterstringdataList;
safeStringUMap<shared_ptr<hkbcharacterstringdata>> hkbcharacterstringdataList_E;

void hkbcharacterstringdata::regis(string id, bool isEdited)
{
	isEdited ? hkbcharacterstringdataList_E[id] = shared_from_this() : hkbcharacterstringdataList[id] = shared_from_this();
	ID = id;
}

void hkbcharacterstringdata::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	usize counter;
	usize select;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("deformableSkinNames", line, numelement))
				{
					deformableSkinNames.reserve(numelement);
					++type;
				}

				break;
			}
			case 1:
			{
				if (!dataNameListEnter(line, deformableSkinNames))
				{
					usize numelement;

					if (readEleParam("rigidSkinNames", line, numelement))
					{
						rigidSkinNames.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 2:
			{
				if (!dataNameListEnter(line, rigidSkinNames))
				{
					usize numelement;

					if (readEleParam("animationNames", line, numelement))
					{
						animationNames.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 3:
			{
				if (!dataNameListEnter(line, animationNames))
				{
					usize numelement;

					if (readEleParam("animationFilenames", line, numelement))
					{
						animationFilenames.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 4:
			{
				if (!dataNameListEnter(line, animationFilenames))
				{
					usize numelement;

					if (readEleParam("characterPropertyNames", line, numelement))
					{
						characterPropertyNames = make_shared<variableinfopack>();

						if (isEdited)
						{
							variablePackNode_E[ID] = characterPropertyNames;

							while (datalock_c.test_and_set(boost::memory_order_acquire));
							characterID.push_back(mapstring(numelement));
							select = characterID.size() - 1;
							datalock_c.clear(boost::memory_order_release);
						}
						else
						{
							variablePackNode[ID] = characterPropertyNames;
						}

						characterPropertyNames->reserve(numelement);
						counter = 0;
						++type;
					}
				}

				break;
			}
			case 5:
			{
				usize pos = line.find("<hkcstring>");

				if (pos != string::npos)
				{
					pos += 11;
					usize pos2 = line.find("</hkcstring>", pos);

					if (pos2 != string::npos)
					{
						string name = line.substr(pos, pos2 - pos);
						characterPropertyNames->push_back(name);

						if (isEdited)
						{
							while (datalock_c.test_and_set(boost::memory_order_acquire));
							characterID[select][to_string(counter++)] = name;
							datalock_c.clear(boost::memory_order_release);
						}
					}
				}
				else
				{
					usize numelement;

					if (readEleParam("retargetingSkeletonMapperFilenames", line, numelement))
					{
						retargetingSkeletonMapperFilenames.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 6:
			{
				if (!dataNameListEnter(line, retargetingSkeletonMapperFilenames))
				{
					usize numelement;

					if (readEleParam("lodNames", line, numelement))
					{
						lodNames.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 7:
			{
				if (!dataNameListEnter(line, lodNames))
				{
					usize numelement;

					if (readEleParam("mirroredSyncPointSubstringsA", line, numelement))
					{
						mirroredSyncPointSubstringsA.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 8:
			{
				if (!dataNameListEnter(line, mirroredSyncPointSubstringsA))
				{
					usize numelement;

					if (readEleParam("mirroredSyncPointSubstringsB", line, numelement))
					{
						mirroredSyncPointSubstringsB.reserve(numelement);
						++type;
					}
				}

				break;
			}
			case 9:
			{
				if (!dataNameListEnter(line, mirroredSyncPointSubstringsB))
				{
					if (readParam("name", line, name)) ++type;
				}

				break;
			}
			case 10:
			{
				if (readParam("rigName", line, rigName)) ++type;

				break;
			}
			case 11:
			{
				if (readParam("ragdollName", line, ragdollName)) ++type;

				break;
			}
			case 12:
			{
				if (readParam("behaviorFilename", line, behaviorFilename)) ++type;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << characterstringdata::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcharacterstringdata::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + characterstringdata::key + to_string(functionlayer) + ">";
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
				auto protectVariable = variablePackNode_E[ID];
				hkbcharacterstringdataList_E.erase(ID);
				editedBehavior.erase(ID);
				variablePackNode_E.erase(ID);
				ID = addressID[address];
				hkbcharacterstringdataList_E[ID] = protect;
				editedBehavior[ID] = protect;
				variablePackNode_E[ID] = protectVariable;
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
				auto protectVariable = variablePackNode_E[ID];
				hkbcharacterstringdataList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				variablePackNode_E.erase(ID);
				ID = addressID[address];
				hkbcharacterstringdataList_E[ID] = protect;
				editedBehavior[ID] = protect;
				variablePackNode_E[ID] = protectVariable;
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

string hkbcharacterstringdata::getClassCode()
{
	return characterstringdata::key;
}

void hkbcharacterstringdata::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(5000);
	usize base = 2;
	hkbcharacterstringdata* ctrpart = static_cast<hkbcharacterstringdata*>(counterpart.get());

	output.push_back(openObject(base, ID, characterstringdata::classname, characterstringdata::signature));	// 1

	// deformable skin names
	usize size = deformableSkinNames.size();
	stringEnter(output, deformableSkinNames, ctrpart->deformableSkinNames, "deformableSkinNames", base, isEdited);

	// rigid skin names
	size = rigidSkinNames.size();
	stringEnter(output, rigidSkinNames, ctrpart->rigidSkinNames, "rigidSkinNames", base, isEdited);

	// animation names
	size = animationNames.size();
	stringEnter(output, animationNames, ctrpart->animationNames, "animationNames", base, isEdited);

	// animation file names
	stringEnter(output, animationFilenames, ctrpart->animationFilenames, "animationFilenames", base, isEdited, false);

	// character property names
	if (characterPropertyNames)
	{
		bool open = false;
		vecstr storeline;
		usize size = characterPropertyNames->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "characterPropertyNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "characterPropertyNames", size));		// 2

		variablefunc::matchScoring(*characterPropertyNames, (*ctrpart->characterPropertyNames), ID, characterstringdata::key);
		size = characterPropertyNames->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*characterPropertyNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(autoString(base, (*ctrpart->characterPropertyNames)[i].name));
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->characterPropertyNames)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->characterPropertyNames)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(autoString(base, (*characterPropertyNames)[i].name));
					++i;
					output.push_back("");	// 1 spaces
				}
			}
			// both exist
			else
			{
				stringMatch((*characterPropertyNames)[i].name, (*ctrpart->characterPropertyNames)[i].name, output, storeline, base, false, open, isEdited);
			}
		}

		nemesis::try_close(open, output, storeline);

		if (size > 0)
		{
			if (characterPropertyNames->d_size() == 0)
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
	else
	{
		output.push_back(openParam(base, "characterPropertyNames", 0) + closeParam());		// 2
		--base;
	}

	// retargeting skeleton mapper file names
	stringEnter(output, retargetingSkeletonMapperFilenames, ctrpart->retargetingSkeletonMapperFilenames, "retargetingSkeletonMapperFilenames", base, isEdited);

	// lod names
	stringEnter(output, lodNames, ctrpart->lodNames, "lodNames", base, isEdited);

	// mirrored sync point A
	stringEnter(output, mirroredSyncPointSubstringsA, ctrpart->mirroredSyncPointSubstringsA, "mirroredSyncPointSubstringsA", base, isEdited);

	// mirrored sync point B
	stringEnter(output, mirroredSyncPointSubstringsB, ctrpart->mirroredSyncPointSubstringsB, "mirroredSyncPointSubstringsB", base, isEdited);

	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("rigName", rigName, ctrpart->rigName, output, storeline, base, false, open, isEdited);
	paramMatch("ragdollName", ragdollName, ctrpart->ragdollName, output, storeline, base, false, open, isEdited);
	paramMatch("behaviorFilename", behaviorFilename, ctrpart->behaviorFilename, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	--num_stringData;
	cont_stringData.notify_all();
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", characterstringdata::classname, output, isEdited);
}

void hkbcharacterstringdata::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(5000);

	output.push_back(openObject(base, ID, characterstringdata::classname, characterstringdata::signature));		// 1
	stringEnterNew(output, deformableSkinNames, "deformableSkinNames", base);
	stringEnterNew(output, rigidSkinNames, "rigidSkinNames", base);
	stringEnterNew(output, animationNames, "animationNames", base);
	stringEnterNew(output, animationFilenames, "animationFilenames", base);

	if (characterPropertyNames)
	{
		usize size = characterPropertyNames->size();

		if (size == 0)
		{
			output.push_back(openParam(base, "characterPropertyNames", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "characterPropertyNames", size));		// 2

		for (auto& line : (*characterPropertyNames))
		{
			output.push_back(autoString(base, line.name));
		}

		if (size > 0) output.push_back(closeParam(base));		// 2
	}
	else
	{
		output.push_back(openParam(base, "characterPropertyNames", 0) + closeParam());		// 2
		--base;
	}

	stringEnterNew(output, retargetingSkeletonMapperFilenames, "retargetingSkeletonMapperFilenames", base);
	stringEnterNew(output, lodNames, "lodNames", base);
	stringEnterNew(output, mirroredSyncPointSubstringsA, "mirroredSyncPointSubstringsA", base);
	stringEnterNew(output, mirroredSyncPointSubstringsB, "mirroredSyncPointSubstringsB", base);

	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "rigName", rigName));
	output.push_back(autoParam(base, "ragdollName", ragdollName));
	output.push_back(autoParam(base, "behaviorFilename", behaviorFilename));
	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	--num_stringData;
	cont_stringData.notify_all();
	outputExtraction(filename, characterstringdata::classname, output, true);
}

void hkbcharacterstringdata::matchScoring(vecstr& ori, vecstr& edit, string id)
{
	if (ori.size() == 0)
	{
		vecstr newOri;
		vecstr newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back("empty!a1s4qq%");
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	usize size = max(ori.size(), edit.size());
	usize avg_threadsize = ori.size() / 5;
	map<int, map<int, double>> scorelist;
	bool dup = false;
	set<string> newname;
	set<string> dupname;
	set<string> dupcheck;

	for (auto& each : edit)
	{
		if (dupcheck.find(each) == dupcheck.end())
		{
			dupcheck.insert(each);
		}
		else
		{
			dup = true;
			dupname.insert(each);
		}

		newname.insert(each);
	}

	if (!dup)
	{
		dupcheck.clear();

		for (auto& each : ori)
		{
			if (dupcheck.find(each) == dupcheck.end())
			{
				dupcheck.insert(each);
			}
			else
			{
				dup = true;
				dupname.insert(each);
			}

			auto i_name = newname.find(each);

			if (i_name != newname.end()) newname.erase(i_name);
		}
	}
	else
	{
		for (auto& each : ori)
		{
			auto i_name = newname.find(each);

			if (i_name != newname.end()) newname.erase(i_name);
		}
	}

	if (avg_threadsize < 100)
	{
		fillScore(ori, edit, 0, ori.size(), scorelist, newname, dup, dupname);
	}
	else
	{
		map<int, map<int, double>> scorelist1;
		map<int, map<int, double>> scorelist2;
		map<int, map<int, double>> scorelist3;
		map<int, map<int, double>> scorelist4;
		map<int, map<int, double>> scorelist5;
		boost::thread_group multi_t;

		usize start = 0;
		usize cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbcharacterstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, boost::ref(scorelist1), newname, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbcharacterstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, boost::ref(scorelist2), newname, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbcharacterstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, boost::ref(scorelist3), newname, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&hkbcharacterstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, boost::ref(scorelist4), newname, dup, dupname));

		start = cap;
		cap = ori.size();
		multi_t.create_thread(boost::bind(&hkbcharacterstringdata::fillScore, this, boost::ref(ori), boost::ref(edit), start, cap, boost::ref(scorelist5), newname, dup, dupname));
		// fillScore(ori, edit, start, cap, scorelist5, dup, dupname);
		multi_t.join_all();

		scorelist.insert(scorelist1.begin(), scorelist1.end());
		scorelist.insert(scorelist2.begin(), scorelist2.end());
		scorelist.insert(scorelist3.begin(), scorelist3.end());
		scorelist.insert(scorelist4.begin(), scorelist4.end());
		scorelist.insert(scorelist5.begin(), scorelist5.end());
	}
	
	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vecstr newOri;
	vecstr newEdit;
	vecstr pushtolast;
	newOri.reserve(size);
	newEdit.reserve(size);

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back("empty!a1s4qq%");
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back("empty!a1s4qq%");
		else newEdit.push_back(edit[order.edited]);
	}

	ori = newOri;
	edit = newEdit;
}

void hkbcharacterstringdata::fillScore(vecstr& ori, vecstr& edit, usize start, usize cap, map<int, map<int, double>>& scorelist, set<string> newname, bool dup, set<string> dupname)
{
	if (dup)
	{
		// match scoring
		for (usize i = start; i < cap; ++i)
		{
			for (usize j = 0; j < edit.size(); ++j)
			{
				double curscore = 0;

				if (i == j)
				{
					curscore += 5;
				}
				else
				{
					int oriindex = i + 1;
					int newindex = j + 1;
					double difference = max(oriindex, newindex) - min(oriindex, newindex);
					difference = ((ori.size() - difference) / ori.size()) * 5;
					curscore += difference;
				}

				if (ori[i] == edit[j])
				{
					curscore += 10;
					scorelist[i][j] = curscore;

					if (i + 1 == ori.size())
					{
						while (++j < edit.size())
						{
							int oriindex = i + 1;
							int newindex = j + 1;
							double difference = max(oriindex, newindex) - min(oriindex, newindex);
							difference = ((ori.size() - difference) / ori.size()) * 5;
							scorelist[i][j] = difference;
						}

						break;
					}

					if (dupname.find(ori[i]) == dupname.end()) break;
				}
				else if (newname.find(edit[j]) != newname.end())
				{
					curscore += (str_similarity(ori[i].substr(13), edit[j].substr(13)) * 8);
				}

				scorelist[i][j] = curscore;
			}
		}
	}
	else
	{
		// match scoring
		for (usize i = start; i < cap; ++i)
		{
			for (usize j = 0; j < edit.size(); ++j)
			{
				double curscore = 0;

				if (i == j)
				{
					curscore += 5;
				}
				else
				{
					int oriindex = i + 1;
					int newindex = j + 1;
					double difference = max(oriindex, newindex) - min(oriindex, newindex);
					difference = ((ori.size() - difference) / ori.size()) * 5;
					curscore += difference;
				}

				if (ori[i] == edit[j])
				{
					curscore += 10;
					scorelist[i][j] = curscore;

					if (i + 1 == ori.size())
					{
						while (++j < edit.size())
						{
							int oriindex = i + 1;
							int newindex = j + 1;
							double difference = max(oriindex, newindex) - min(oriindex, newindex);
							difference = ((ori.size() - difference) / ori.size()) * 5;
							scorelist[i][j] = difference;
						}
					}

					break;
				}
				else if (newname.find(edit[j]) != newname.end())
				{
					curscore += (str_similarity(ori[i].substr(13), edit[j].substr(13)) * 8);
				}

				scorelist[i][j] = curscore;
			}
		}
	}
}

void hkbcharacterstringdata::stringEnter(vecstr& output, vecstr& namelist, vecstr& ctrpart_namelist, string listname, usize& base, bool& isEdited, bool caseSensitive)
{
	bool open = false;
	vecstr storeline;
	usize size = namelist.size();

	if (size == 0)
	{
		output.push_back(openParam(base, listname, size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, listname, size));		// 2

	usize size2 = size;
	matchScoring(namelist, ctrpart_namelist, ID);
	size = namelist.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (namelist[i] == "empty!a1s4qq%")
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(autoString(base, ctrpart_namelist[i]));
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart_namelist[i] == "empty!a1s4qq%")
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart_namelist[i] != "empty!a1s4qq%")
				{
					--i;
					break;
				}

				storeline.push_back(autoString(base, namelist[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			stringMatch(namelist[i], ctrpart_namelist[i], output, storeline, base, false, open, isEdited, caseSensitive);
		}
	}

	nemesis::try_close(open, output, storeline);

	if (size > 0)
	{
		if (size2 == 0)
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

void hkbcharacterstringdata::stringEnterNew(vecstr& output, vecstr namelist, string listname, usize& base)
{
	usize size = namelist.size();

	if (size == 0)
	{
		output.push_back(openParam(base, listname, size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, listname, size));		// 2

	for (auto& line : namelist)
	{
		output.push_back(autoString(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2
}

bool hkbcharacterstringdata::dataNameListEnter(string line, vecstr& input)
{
	usize pos = line.find("<hkcstring>");

	if (pos != string::npos)
	{
		pos += 11;
		usize pos2 = line.find("</hkcstring>", pos);

		if (pos2 != string::npos)
		{
			input.push_back(line.substr(pos, pos2 - pos));
			return true;
		}
	}

	return false;
}

void stringListCompare(vecstr& ori, usize i, vecstr& edit, map<int, map<int, double>>& score, boost::atomic_flag& lock)
{
	for (unsigned int j = 0; j < edit.size(); ++j)
	{
		double curscore = 0;

		if (ori[i] == edit[j])
		{
			curscore += 10;
		}

		if (i == j)
		{
			curscore += 5;
		}
		else
		{
			int oriindex = i + 1;
			int newindex = j + 1;
			double difference = max(oriindex, newindex) - min(oriindex, newindex);
			difference = ((ori.size() - difference) / ori.size()) * 5;
			curscore += difference;
		}

		while (lock.test_and_set(boost::memory_order_acquire));
		score[i][j] = curscore;
		lock.clear(boost::memory_order_release);
	}
}

void hkbCharacterStringDataExport(string id)
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
		cout << "ERROR: Edit hkbCharacterStringData Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

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
	}
	else
	{
		cout << "ERROR: Edit hkbCharacterStringData Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vecstr output;
	bool IsEdited = false;
	bool IsChanged = false;
	bool IsOpenOut = false;
	bool open = false;
	int curline = 2;
	int part = 0;
	int openpoint;
	int closepoint;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[1]);

		if ((storeline1[1].find("</hkparam>", 0) != string::npos) && (storeline2[2].find("<hkcstring>", 0) != string::npos))
		{
			IsChanged = true;
			openpoint = 1;
			open = true;
		}
		else
		{
			IsChanged = false;
			output.push_back("<!-- ORIGINAL -->");
			output.push_back(storeline1[1]);
			output.push_back("<!-- CLOSE -->");
			open = false;
		}

		IsEdited = true;
	}
	else
	{
		output.push_back(storeline2[1]);
	}

	for (unsigned int i = 2; i < storeline2.size(); i++)
	{
		if ((storeline1[curline].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) == string::npos) && (part == 0)) // existing deformableSkinNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"animationNames\" numelements=", 0) == string::npos) && (part == 1)) // existing rigidSkinNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"animationFilenames\" numelements=", 0) == string::npos) && (part == 2)) // existing animationNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) == string::npos) && (part == 3)) // existing animationFilenames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) == string::npos) && (part == 4)) // existing characterPropertyNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"lodNames\" numelements=", 0) == string::npos) && (part == 5)) // existing retargetingSkeletonMapperFilenames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) == string::npos) && (part == 6)) // existing lodNames value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) == string::npos) && (part == 7)) // existing mirroredSyncPointSubstringsA value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((storeline1[curline].find("<hkparam name=\"name\">", 0) == string::npos) && (part == 8)) // existing mirroredSyncPointSubstringsB value
		{
			process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
			curline++;
		}
		else if ((curline != storeline1.size()) && (part == 9))
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < curline; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
			else
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

			output.push_back(storeline2[i]);

			if (curline + 1 != storeline1.size())
			{
				curline++;
			}
		}
		else // added variable value
		{
			if (storeline2[i].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 1;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"animationNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 2;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"animationFilenames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 3;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 4;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 5;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"lodNames\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 6;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 7;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) != string::npos)
			{
				process(storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				part = 8;
				curline++;
			}
			else if (storeline2[i].find("<hkparam name=\"name\">", 0) != string::npos)
			{
				if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < curline; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}
				else
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

				output.push_back(storeline2[i]);
				part = 9;
				curline++;
			}
			else
			{
				if (part == 0)
				{
					postProcess("rigidSkinNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 1)
				{
					postProcess("animationNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 2)
				{
					postProcess("animationFilenames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 3)
				{
					postProcess("characterPropertyNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 4)
				{
					postProcess("retargetingSkeletonMapperFilenames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 5)
				{
					postProcess("lodNames", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 6)
				{
					postProcess("mirroredSyncPointSubstringsA", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 7)
				{
					postProcess("mirroredSyncPointSubstringsB", storeline1, storeline2, curline, i, IsChanged, openpoint, open, IsEdited, output);
				}
				else if (part == 8)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);

					if (storeline2[i + 1].find("<hkparam name=\"name\">", 0) != string::npos)
					{
						if (IsChanged)
						{
							if (openpoint != curline)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
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
			}
		}

		if (i == 1921)
		{
			open = open;
		}
	}

	if (open) // close unclosed edits
	{
		if (IsChanged)
		{
			closepoint = curline + 1;

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

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
	bool closeOri = false;
	bool closeEdit = false;

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				if (i < output.size() - 1)
				{
					if ((output[i + 1].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"deformableSkinNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"rigidSkinNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationFilenames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"animationFilenames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"characterPropertyNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"lodNames\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"retargetingSkeletonMapperFilenames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"lodNames\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsA\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
					else if ((i + 1 == output.size()) && (output[i].find("OPEN", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
					else if ((output[i + 1].find("<hkparam name=\"mirroredSyncPointSubstringsB\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
					{
						if (!closeEdit)
						{
							fwrite << "			</hkparam>" << "\n";
							closeEdit = true;
						}
					}
				}

				if (output[i].find("\" numelements=\"", 0) != string::npos)
				{
					if (output[i - 1].find("OPEN", 0) != string::npos)
					{
						if (output[i].find("</hkparam>", 0) != string::npos)
						{
							closeEdit = true;
						}
						else
						{
							closeEdit = false;
						}
					}
					else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
					{
						if (output[i].find("</hkparam>", 0) != string::npos)
						{
							closeOri = true;
						}
						else
						{
							closeOri = false;
						}
					}
					else
					{
						if (output[i].find("</hkparam>", 0) != string::npos)
						{
							closeEdit = true;
							closeOri = true;
						}
						else
						{
							closeEdit = false;
							closeOri = false;
						}
					}
				}

				fwrite << output[i] << "\n";

				if (i < output.size() - 1)
				{
					if (output[i + 1].find("numelements=", 0) != string::npos) // close outside if both aren't closed
					{
						if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkcstring>", 0) != string::npos))
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
					}
				}
				else if (i + 1 == output.size())
				{
					if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkcstring>", 0) != string::npos))
					{
						if ((!closeOri) && (!closeEdit))
						{
							fwrite << "			</hkparam>" << "\n";
							closeOri = true;
							closeEdit = true;
						}
					}
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbCharacterStringData Output Not Found (File: " << filename << ")" << endl;
			Error = true;
			return;
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

inline void process(vecstr storeline1, vecstr storeline2, int curline, int i, bool& IsChanged, int& openpoint, bool& open, bool& IsEdited, vecstr& output)
{
	if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[i]);

		if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkcstring>", 0) != string::npos))
		{
			IsChanged = true;
			openpoint = curline;
			open = true;
		}
		else
		{
			IsChanged = false;
			output.push_back("<!-- ORIGINAL -->");
			output.push_back(storeline1[curline]);
			output.push_back("<!-- CLOSE -->");
			open = false;
		}

		IsEdited = true;
	}
	else
	{
		output.push_back(storeline2[i]);
	}
}

inline void postProcess(string elementName, vecstr storeline1, vecstr storeline2, int curline, int i, bool& IsChanged, int openpoint, bool& open, bool& IsEdited, vecstr& output)
{
	if (!open)
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		IsEdited = true;
		open = true;
	}

	output.push_back(storeline2[i]);

	if (storeline2[i + 1].find("<hkparam name=\"" + elementName + "\" numelements=", 0) != string::npos)
	{
		if (IsChanged)
		{
			if (openpoint != curline)
			{
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < curline; j++)
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