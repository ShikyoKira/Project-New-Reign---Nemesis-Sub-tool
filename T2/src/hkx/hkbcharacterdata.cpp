#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>
#include "hkbcharacterdata.h"

#include "src\utilities\stringdatalock.h"

#include "src\hkx\hkbvariablevalueset.h"
#include "src\hkx\hkbfootikdriverinfo.h"
#include "src\hkx\hkbhandikdriverinfo.h"
#include "src\hkx\hkbcharacterstringdata.h"
#include "src\hkx\hkbmirroredskeletoninfo.h"

using namespace std;

namespace characterdata
{
	const string key = "ce";
	const string classname = "hkbCharacterData";
	const string signature = "0x300d6808";
}

safeStringUMap<shared_ptr<hkpcharactercontrollercinfo>> hkpcharactercontrollercinfoList;
safeStringUMap<shared_ptr<hkpcharactercontrollercinfo>> hkpcharactercontrollercinfoList_E;

void hkpcharactercontrollercinfo::regis(string id, bool isEdited)
{
	isEdited ? hkpcharactercontrollercinfoList_E[id] = shared_from_this() : hkpcharactercontrollercinfoList[id] = shared_from_this();
	ID = id;
}

string hkbcharacterdata::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbcharacterdata>> hkbcharacterdataList;
safeStringUMap<shared_ptr<hkbcharacterdata>> hkbcharacterdataList_E;

void hkbcharacterdata::regis(string id, bool isEdited)
{
	isEdited ? hkbcharacterdataList_E[id] = shared_from_this() : hkbcharacterdataList[id] = shared_from_this();
	ID = id;
}

void hkbcharacterdata::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	usize counter = 0;

	for (auto& line : nodelines)
	{
		usize pos = line.find("<hkparam name=\"stringData\">");

		if (pos != string::npos)
		{
			pos += 27;
			string stringID = line.substr(pos, line.find("</hkparam>", pos) - pos);

			if (stringID != "null")
			{
				stringData = (isEdited ? hkbcharacterstringdataList_E : hkbcharacterstringdataList)[stringID];
				characterPropertyInfos = stringData->characterPropertyNames;
			}
		}
	}

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				if (readParam("capsuleHeight", line, characterControllerInfo.capsuleHeight)) ++type;

				break;
			}
			case 1:
			{
				if (readParam("capsuleRadius", line, characterControllerInfo.capsuleRadius)) ++type;

				break;
			}
			case 2:
			{
				if (readParam("collisionFilterInfo", line, characterControllerInfo.collisionFilterInfo)) ++type;

				break;
			}
			case 3:
			{
				string output;

				if (readParam("characterControllerCinfo", line, output))
				{
					characterControllerInfo.characterControllerCinfo = (isEdited ? hkpcharactercontrollercinfoList_E : hkpcharactercontrollercinfoList)[output];
					++type;
				}

				break;
			}
			case 4:
			{
				if (readParam("modelUpMS", line, modelUpMS)) ++type;

				break;
			}
			case 5:
			{
				if (readParam("modelForwardMS", line, modelForwardMS)) ++type;

				break;
			}
			case 6:
			{
				if (readParam("modelRightMS", line, modelRightMS)) ++type;

				break;
			}
			case 7:
			{
				string output;

				if (readParam("role", line, output))
				{
					(*characterPropertyInfos)[counter].role.installRole(output);
					++type;
					break;
				}

				usize numelement;

				if (readEleParam("numBonesPerLod", line, numelement))
				{
					counter = 0;
					numBonesPerLod.reserve(numelement);
					type = 10;
				}

				break;
			}
			case 8:
			{
				string output;

				if (readParam("flags", line, output))
				{
					vecstr list;
					boost::trim_if(output, boost::is_any_of("\t "));
					boost::split(list, output, boost::is_any_of("|"), boost::token_compress_on);

					for (auto& each : list)
					{
						(*characterPropertyInfos)[counter].role.flags.update(each);
					}

					++type;
				}

				break;
			}
			case 9:
			{
				string output;

				if (readParam("type", line, output))
				{
					(*characterPropertyInfos)[counter].typeUpdate(output);
					++counter;
					type = 7;
				}

				break;
			}
			case 10:
			{
				vecstr list;
				boost::trim_if(line, boost::is_any_of("\t "));
				boost::split(list, line, boost::is_any_of("\t "), boost::token_compress_on);

				for (auto& each : list)
				{
					if (!isOnlyNumber(each))
					{
						++type;
						break;
					}

					numBonesPerLod.push_back(stoi(each));
				}
			}
			case 11:
			{
				string output;

				if (readParam("characterPropertyValues", line, output))
				{
					characterPropertyValues = (isEdited ? hkbvariablevaluesetList_E : hkbvariablevaluesetList)[output];
					++type;
				}

				break;
			}
			case 12:
			{
				string output;

				if (readParam("footIkDriverInfo", line, output))
				{
					footIkDriverInfo = (isEdited ? hkbfootikdriverinfoList_E : hkbfootikdriverinfoList)[output];
					++type;
				}

				break;
			}
			case 13:
			{
				string output;

				if (readParam("handIkDriverInfo", line, output))
				{
					handIkDriverInfo = (isEdited ? hkbhandikdriverinfoList_E : hkbhandikdriverinfoList)[output];
					++type;
				}

				break;
			}
			case 14:
			{
				string output;

				if (readParam("stringData", line, output))
				{
					stringData = (isEdited ? hkbcharacterstringdataList_E : hkbcharacterstringdataList)[output];
					++type;
				}

				break;
			}
			case 15:
			{
				string output;

				if (readParam("mirroredSkeletonInfo", line, output))
				{
					mirroredSkeletonInfo = (isEdited ? hkbmirroredskeletoninfoList_E : hkbmirroredskeletoninfoList)[output];
					++type;
				}

				break;
			}
			case 16:
			{
				if (readParam("scale", line, scale)) ++type;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << characterdata::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcharacterdata::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + characterdata::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			boost::thread_group multithreads;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (characterControllerInfo.characterControllerCinfo) threadedNextNode(characterControllerInfo.characterControllerCinfo, filepath, curadd, functionlayer, graphroot);
			
			if (num_thread < boost::thread::hardware_concurrency())
			{
				if (characterPropertyValues) multithreads.create_thread(boost::bind(&hkbcharacterdata::threadedNextNode, this, characterPropertyValues, filepath, curadd,
					functionlayer, graphroot));
			}
			else
			{
				if (characterPropertyValues) threadedNextNode(characterPropertyValues, filepath, curadd, functionlayer, graphroot);
			}

			if (footIkDriverInfo) threadedNextNode(footIkDriverInfo, filepath, curadd, functionlayer, graphroot);

			if (handIkDriverInfo) threadedNextNode(handIkDriverInfo, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				if (stringData) multithreads.create_thread(boost::bind(&hkbcharacterdata::threadedNextNode, this, stringData, filepath, curadd, functionlayer, graphroot));
			}
			else
			{
				if (stringData) threadedNextNode(stringData, filepath, curadd, functionlayer, graphroot);
			}

			if (mirroredSkeletonInfo) threadedNextNode(mirroredSkeletonInfo, filepath, curadd, functionlayer, graphroot);

			num_thread += multithreads.size();
			multithreads.join_all();
			num_thread -= multithreads.size();
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
				hkbcharacterdataList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbcharacterdataList_E[ID] = protect;
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
				hkbcharacterdataList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbcharacterdataList_E[ID] = protect;
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

string hkbcharacterdata::getClassCode()
{
	return characterdata::key;
}

void hkbcharacterdata::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	if (num_stringData > 0)
	{
		unique_lock<mutex> lock(mutex_stringData);
		cont_stringData.wait(lock, [] {return num_stringData == 0; });
	}

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(5000);
	usize base = 2;
	hkbcharacterdata* ctrpart = static_cast<hkbcharacterdata*>(counterpart.get());

	output.push_back(openObject(base, ID, characterdata::classname, characterdata::signature));	// 1
	output.push_back(openParam(base, "characterControllerInfo"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("capsuleHeight", characterControllerInfo.capsuleHeight, ctrpart->characterControllerInfo.capsuleHeight, output, storeline, base, false, open, isEdited);
	paramMatch("capsuleRadius", characterControllerInfo.capsuleRadius, ctrpart->characterControllerInfo.capsuleRadius, output, storeline, base, false, open, isEdited);
	paramMatch("collisionFilterInfo", characterControllerInfo.collisionFilterInfo, ctrpart->characterControllerInfo.collisionFilterInfo, output, storeline, base, false, open,
		isEdited);
	paramMatch("characterControllerCinfo", characterControllerInfo.characterControllerCinfo, ctrpart->characterControllerInfo.characterControllerCinfo, output, storeline, base,
		true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("modelUpMS", modelUpMS, ctrpart->modelUpMS, output, storeline, base, false, open, isEdited);
	paramMatch("modelForwardMS", modelForwardMS, ctrpart->modelForwardMS, output, storeline, base, false, open, isEdited);
	paramMatch("modelRightMS", modelRightMS, ctrpart->modelRightMS, output, storeline, base, true, open, isEdited);

	usize size = characterPropertyInfos->d_size();

	if (size == 0)
	{
		output.push_back(openParam(base, "characterPropertyInfos", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "characterPropertyInfos", size));		// 2

	size = characterPropertyInfos->size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if ((*characterPropertyInfos)[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));				// 3
				output.push_back(openParam(base, "role"));		// 4
				output.push_back(openObject(base));				// 5
				output.push_back(autoParam(base, "role", (*ctrpart->characterPropertyInfos)[i].role.getRole()));
				output.push_back(autoParam(base, "flags", (*ctrpart->characterPropertyInfos)[i].role.flags.getflags()));
				output.push_back(closeObject(base));			// 5
				output.push_back(closeParam(base));				// 4
				output.push_back(autoParam(base, "type", (*ctrpart->characterPropertyInfos)[i].getType()));
				output.push_back(closeObject(base));			// 3
				++i;
			}
		}
		// deleted existing data
		else if ((*ctrpart->characterPropertyInfos)[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!(*ctrpart->characterPropertyInfos)[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));				// 3
				storeline.push_back(openParam(base, "role"));		// 4
				storeline.push_back(openObject(base));				// 5
				storeline.push_back(autoParam(base, "role", (*characterPropertyInfos)[i].role.getRole()));
				storeline.push_back(autoParam(base, "flags", (*characterPropertyInfos)[i].role.flags.getflags()));
				storeline.push_back(closeObject(base));				// 5
				storeline.push_back(closeParam(base));				// 4
				storeline.push_back(autoParam(base, "type", (*characterPropertyInfos)[i].getType()));
				storeline.push_back(closeObject(base));				// 3
				++i;
				usize spaces = 0;

				while (spaces++ < 9)
				{
					output.push_back("");	// 9 spaces
				}
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));				// 3
			output.push_back(openParam(base, "role"));		// 4
			output.push_back(openObject(base));				// 5
			paramMatch("role", (*characterPropertyInfos)[i].role.getRole(), (*ctrpart->characterPropertyInfos)[i].role.getRole(), output, storeline, base, false, open,
				isEdited);
			paramMatch("flags", (*characterPropertyInfos)[i].role.flags.getflags(), (*ctrpart->characterPropertyInfos)[i].role.flags.getflags(), output, storeline,
				base, true, open, isEdited);
			output.push_back(closeObject(base));			// 5
			output.push_back(closeParam(base));				// 4
			paramMatch("type", (*characterPropertyInfos)[i].getType(), (*ctrpart->characterPropertyInfos)[i].getType(), output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));			// 3
		}
	}

	if (size > 0)
	{
		if (characterPropertyInfos->d_size() == 0)
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

	size = numBonesPerLod.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "numBonesPerLod", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "numBonesPerLod", size));		// 2

	for (usize i = 0; i < size; ++i)
	{
		// deleted existing data
		if (ctrpart->numBonesPerLod.size() <= i)
		{
			if (!open) nemesis::try_open(open, isEdited, output);

			storeline.push_back(autoVoid(base, numBonesPerLod[i]));
			++i;
			output.push_back("");		// 1 space
		}
		else
		{
			voidMatch(numBonesPerLod[i], ctrpart->numBonesPerLod[i], output, storeline, base, false, open, isEdited);
		}
	}

	size = ctrpart->numBonesPerLod.size();

	// newly created data
	if (size > numBonesPerLod.size())
	{
		nemesis::try_open(open, isEdited, output);

		for (usize i = numBonesPerLod.size(); i < size; ++i)
		{
			output.push_back(autoVoid(base, ctrpart->numBonesPerLod[i]));
		}
	}

	if (size > 0)
	{
		if (numBonesPerLod.size() == 0)
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

	paramMatch("characterPropertyValues", characterPropertyValues, ctrpart->characterPropertyValues, output, storeline, base, false, open, isEdited);
	paramMatch("footIkDriverInfo", footIkDriverInfo, ctrpart->footIkDriverInfo, output, storeline, base, false, open, isEdited);
	paramMatch("handIkDriverInfo", handIkDriverInfo, ctrpart->handIkDriverInfo, output, storeline, base, false, open, isEdited);
	paramMatch("stringData", stringData, ctrpart->stringData, output, storeline, base, false, open, isEdited);
	paramMatch("mirroredSkeletonInfo", mirroredSkeletonInfo, ctrpart->mirroredSkeletonInfo, output, storeline, base, true, open, isEdited);

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", characterdata::classname, output, isEdited);
}

void hkbcharacterdata::newNode()
{
	if (num_stringData > 0)
	{
		unique_lock<mutex> lock(mutex_stringData);
		cont_stringData.wait(lock, [] {return num_stringData == 0; });
	}

	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(5000);

	output.push_back(openObject(base, ID, characterdata::classname, characterdata::signature));		// 1
	output.push_back(openParam(base, "characterControllerInfo"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "capsuleHeight", characterControllerInfo.capsuleHeight));
	output.push_back(autoParam(base, "capsuleRadius", characterControllerInfo.capsuleRadius));
	output.push_back(autoParam(base, "collisionFilterInfo", characterControllerInfo.collisionFilterInfo));
	output.push_back(autoParam(base, "characterControllerCinfo", characterControllerInfo.characterControllerCinfo));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "modelUpMS", modelUpMS.getString()));
	output.push_back(autoParam(base, "modelForwardMS", modelForwardMS.getString()));
	output.push_back(autoParam(base, "modelRightMS", modelRightMS.getString()));

	usize size = characterPropertyInfos->size();

	if (size == 0)
	{
		output.push_back(openParam(base, "characterPropertyInfos", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "characterPropertyInfos", size));		// 2

	for (auto& data : (*characterPropertyInfos))
	{
		output.push_back(openObject(base));			// 3
		output.push_back(openParam(base, "role"));	// 4
		output.push_back(openObject(base));			// 5
		output.push_back(autoParam(base, "role", data.role.getRole()));
		output.push_back(autoParam(base, "flags", data.role.flags.getflags()));
		output.push_back(closeObject(base));		// 5
		output.push_back(closeParam(base));			// 4
		output.push_back(autoParam(base, "type", data.getType()));
		output.push_back(closeObject(base));		// 3
	}

	if (characterPropertyInfos->size() > 0) output.push_back(closeParam(base));			// 2

	size = numBonesPerLod.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "numBonesPerLod", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "numBonesPerLod", size));		// 2

	for (auto& data : numBonesPerLod)
	{
		output.push_back(autoVoid(base, data));
	}

	if (numBonesPerLod.size() > 0) output.push_back(closeParam(base));			// 2

	output.push_back(autoParam(base, "characterPropertyValues", characterPropertyValues));
	output.push_back(autoParam(base, "footIkDriverInfo", footIkDriverInfo));
	output.push_back(autoParam(base, "handIkDriverInfo", handIkDriverInfo));
	output.push_back(autoParam(base, "stringData", stringData));
	output.push_back(autoParam(base, "mirroredSkeletonInfo", mirroredSkeletonInfo));

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, characterdata::classname, output, true);
}

void hkbcharacterdata::parentRefresh()
{
	if (characterControllerInfo.characterControllerCinfo) hkb_parent[characterControllerInfo.characterControllerCinfo] = shared_from_this();
	if (characterPropertyValues) hkb_parent[characterPropertyValues] = shared_from_this();
	if (footIkDriverInfo) hkb_parent[footIkDriverInfo] = shared_from_this();
	if (handIkDriverInfo) hkb_parent[handIkDriverInfo] = shared_from_this();
	if (stringData) hkb_parent[stringData] = shared_from_this();
	if (mirroredSkeletonInfo) hkb_parent[mirroredSkeletonInfo] = shared_from_this();
}

void hkbcharacterdata::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (characterControllerInfo.characterControllerCinfo)
	{
		parentRefresh();
		characterControllerInfo.characterControllerCinfo->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (characterPropertyValues)
	{
		parentRefresh();
		characterPropertyValues->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (footIkDriverInfo)
	{
		parentRefresh();
		footIkDriverInfo->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (handIkDriverInfo)
	{
		parentRefresh();
		handIkDriverInfo->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (stringData)
	{
		parentRefresh();
		stringData->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (mirroredSkeletonInfo)
	{
		parentRefresh();
		mirroredSkeletonInfo->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbcharacterdata::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
