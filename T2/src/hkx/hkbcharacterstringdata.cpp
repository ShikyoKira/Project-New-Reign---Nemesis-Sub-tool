#include <boost\thread.hpp>
#include "hkbcharacterstringdata.h"
#include "highestscore.h"
#include "src\utilities\stringdatalock.h"
#include "src\utilities\levenshteindistance.h"

using namespace std;

boost::atomic_flag datalock_c = BOOST_ATOMIC_FLAG_INIT;

namespace characterstringdata
{
	const string key = "cc";
	const string classname = "hkbCharacterStringData";
	const string signature = "0x655b42bc";
}

string hkbcharacterstringdata::GetAddress()
{
	return address;
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
