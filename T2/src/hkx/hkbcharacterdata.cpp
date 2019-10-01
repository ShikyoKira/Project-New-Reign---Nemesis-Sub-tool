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
	string key = "ce";
	string classname = "hkbCharacterData";
	string signature = "0x300d6808";
}

safeStringUMap<shared_ptr<hkpcharactercontrollercinfo>> hkpcharactercontrollercinfoList;
safeStringUMap<shared_ptr<hkpcharactercontrollercinfo>> hkpcharactercontrollercinfoList_E;

void hkpcharactercontrollercinfo::regis(string id, bool isEdited)
{
	isEdited ? hkpcharactercontrollercinfoList_E[id] = shared_from_this() : hkpcharactercontrollercinfoList[id] = shared_from_this();
	ID = id;
}

hkbcharacterdata::hkbcharacterdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + characterdata::key + to_string(functionlayer) + ">";

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

void hkbcharacterdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		for (unsigned int i = 0; i < FunctionLineOriginal[id].size(); i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				size_t pos = line.find("characterPropertyValues\">") + 25;
				charPropertyID = line.substr(pos, line.find("</hkparam>") - pos);

				if (charPropertyID != "null")
				{
					referencingIDs[charPropertyID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"footIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("footIkDriverInfo\">") + 18;
				footIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (footIKID != "null")
				{
					referencingIDs[footIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"handIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("handIkDriverInfo\">") + 18;
				handIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (handIKID != "null")
				{
					referencingIDs[handIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				size_t pos = line.find("stringData\">") + 12;
				n_stringData = line.substr(pos, line.find("</hkparam>") - pos);

				if (n_stringData != "null")
				{
					referencingIDs[n_stringData].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"mirroredSkeletonInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("mirroredSkeletonInfo\">") + 22;
				skeleton = line.substr(pos, line.find("</hkparam>") - pos);

				if (skeleton != "null")
				{
					referencingIDs[skeleton].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbCharacterData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbCharacterData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCharacterData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr storeline2;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		for (unsigned int i = 0; i < FunctionLineEdited[id].size(); i++)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				usize pos = line.find("characterPropertyValues\">") + 25;
				charPropertyID = line.substr(pos, line.find("</hkparam>") - pos);

				if (charPropertyID != "null")
				{
					if (!exchangeID[charPropertyID].empty())
					{
						usize tempint = line.find(charPropertyID);
						charPropertyID = exchangeID[charPropertyID];
						line.replace(tempint, line.find("</hkparam>") - tempint, charPropertyID);
					}

					parent[charPropertyID] = id;
					referencingIDs[charPropertyID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"footIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("footIkDriverInfo\">") + 18;
				footIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (footIKID != "null")
				{
					if (!exchangeID[footIKID].empty())
					{
						usize tempint = line.find(footIKID);
						footIKID = exchangeID[footIKID];
						line.replace(tempint, line.find("</hkparam>") - tempint, footIKID);
					}

					parent[footIKID] = id;
					referencingIDs[footIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"handIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("handIkDriverInfo\">") + 18;
				handIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (handIKID != "null")
				{
					if (!exchangeID[handIKID].empty())
					{
						usize tempint = line.find(handIKID);
						handIKID = exchangeID[handIKID];
						line.replace(tempint, line.find("</hkparam>") - tempint, handIKID);
					}

					parent[handIKID] = id;
					referencingIDs[handIKID].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				size_t pos = line.find("stringData\">") + 12;
				n_stringData = line.substr(pos, line.find("</hkparam>") - pos);

				if (n_stringData != "null")
				{
					if (!exchangeID[n_stringData].empty())
					{
						usize tempint = line.find(n_stringData);
						n_stringData = exchangeID[n_stringData];
						line.replace(tempint, line.find("</hkparam>") - tempint, n_stringData);
					}

					parent[n_stringData] = id;
					referencingIDs[n_stringData].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"mirroredSkeletonInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("mirroredSkeletonInfo\">") + 22;
				skeleton = line.substr(pos, line.find("</hkparam>") - pos);

				if (skeleton != "null")
				{
					if (!exchangeID[skeleton].empty())
					{
						usize tempint = line.find(skeleton);
						skeleton = exchangeID[skeleton];
						line.replace(tempint, line.find("</hkparam>") - tempint, skeleton);
					}

					parent[skeleton] = id;
					referencingIDs[skeleton].push_back(id);
				}
			}

			storeline2.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbCharacterData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			address = addressChange[address];
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbCharacterData(newID: " << id << ") with hkbCharacterData(oldID: " << tempid << ")" << endl;
		}

		if (charPropertyID != "null")
		{
			referencingIDs[charPropertyID].pop_back();
			referencingIDs[charPropertyID].push_back(tempid);
			parent[charPropertyID] = tempid;
		}

		if (footIKID != "null")
		{
			referencingIDs[footIKID].pop_back();
			referencingIDs[footIKID].push_back(tempid);
			parent[footIKID] = tempid;
		}

		if (handIKID != "null")
		{
			referencingIDs[handIKID].pop_back();
			referencingIDs[handIKID].push_back(tempid);
			parent[handIKID] = tempid;
		}

		if (n_stringData != "null")
		{
			referencingIDs[n_stringData].pop_back();
			referencingIDs[n_stringData].push_back(tempid);
			parent[n_stringData] = tempid;
		}

		if (skeleton != "null")
		{
			referencingIDs[skeleton].pop_back();
			referencingIDs[skeleton].push_back(tempid);
			parent[skeleton] = tempid;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < storeline2.size(); i++)
		{
			FunctionLineNew[tempid].push_back(storeline2[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbCharacterData(newID: " << id << ") with hkbCharacterData(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = storeline2;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbCharacterData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcharacterdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbCharacterData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				size_t pos = line.find("characterPropertyValues\">") + 25;
				charPropertyID = line.substr(pos, line.find("</hkparam>") - pos);

				if (charPropertyID != "null")
				{
					if (!exchangeID[charPropertyID].empty())
					{
						charPropertyID = exchangeID[charPropertyID];
					}

					parent[charPropertyID] = id;
				}
			}
			else if (line.find("<hkparam name=\"footIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("footIkDriverInfo\">") + 18;
				footIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (footIKID != "null")
				{
					if (!exchangeID[footIKID].empty())
					{
						footIKID = exchangeID[footIKID];
					}

					parent[footIKID] = id;
				}
			}
			else if (line.find("<hkparam name=\"handIkDriverInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("handIkDriverInfo\">") + 18;
				handIKID = line.substr(pos, line.find("</hkparam>") - pos);

				if (handIKID != "null")
				{
					if (!exchangeID[handIKID].empty())
					{
						handIKID = exchangeID[handIKID];
					}

					parent[handIKID] = id;
				}
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				size_t pos = line.find("stringData\">") + 12;
				n_stringData = line.substr(pos, line.find("</hkparam>") - pos);

				if (n_stringData != "null")
				{
					if (!exchangeID[n_stringData].empty())
					{
						n_stringData = exchangeID[n_stringData];
					}

					parent[n_stringData] = id;
				}
			}
			else if (line.find("<hkparam name=\"mirroredSkeletonInfo\">", 0) != string::npos)
			{
				size_t pos = line.find("mirroredSkeletonInfo\">") + 22;
				skeleton = line.substr(pos, line.find("</hkparam>") - pos);

				if (skeleton != "null")
				{
					if (!exchangeID[skeleton].empty())
					{
						skeleton = exchangeID[skeleton];
					}

					parent[skeleton] = id;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbCharacterData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbCharacterData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbcharacterdata::GetAddress()
{
	return address;
}

bool hkbcharacterdata::IsNegate()
{
	return IsNegated;
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

string hkbcharacterdata::GetCharPropertyValues()
{
	return charPropertyID;
}

string hkbcharacterdata::GetFootIK()
{
	return footIKID;
}

string hkbcharacterdata::GetHandIK()
{
	return handIKID;
}

string hkbcharacterdata::GetSkeletonInfo()
{
	return skeleton;
}

string hkbcharacterdata::GetStringData()
{
	return n_stringData;
}

bool hkbcharacterdata::HasCharProperty()
{
	if (charPropertyID.find("null", 0) != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool hkbcharacterdata::HasFootIK()
{
	if (footIKID.find("null", 0) != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool hkbcharacterdata::HasHandIK()
{
	if (handIKID.find("null", 0) != string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void hkbCharacterDataExport(string id)
{
	//stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	vecstr storeline2 = FunctionLineNew[id];

	// stage 2 identify edits
	vecstr output;
	bool open = false;
	bool IsEdited = false;
	int curline = 2;
	int openpoint;
	int closepoint;
	int nextpoint;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		openpoint = curline - 1;
		IsEdited = true;
		open = true;
	}

	output.push_back(storeline2[1]);

	for (unsigned int i = 2; i < storeline2.size(); i++)		// wordVariableValues data
	{
		if (storeline1[curline].find("\t\t\t<hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkobject>", 0) != string::npos ||
			storeline1[curline].find("\t\t\t</hkparam>", 0) != string::npos)
		{
			if (open)
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

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			output.push_back(storeline1[curline]);
			curline++;
		}
		else if (storeline1[curline].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) == string::npos)		// existing variable value
		{
			if (storeline1[curline].find(storeline2[i], 0) != string::npos)
			{
				if (open)
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

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsEdited = true;
					open = true;
				}
			}

			curline++;
			output.push_back(storeline2[i]);
		}
		else // added variable value
		{
			if (open)
			{
				closepoint = curline - 1;
				output.pop_back();

				if (closepoint != openpoint)
				{
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			nextpoint = i;
			break;
		}
	}

	bool skip = false;

	for (unsigned int i = nextpoint; i < storeline2.size(); i++)		// characterPropertyInfos data
	{
		if (storeline1[curline].find("<hkparam name=\"numBonesPerLod\" numelements=", 0) == string::npos)		// existing variable value
		{
			if (storeline1[curline + 1].find("<hkparam name=\"numBonesPerLod\" numelements=", 0) == string::npos && (storeline1[curline].find("\t\t\t<hkobject>", 0) !=
				string::npos || storeline1[curline].find("\t\t\t</hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkparam>", 0) != string::npos ||
				(storeline1[curline].find("\t\t\t<hkparam name=\"role\">", 0) != string::npos &&
					storeline1[curline].find("\t\t\t<hkparam name=\"role\">ROLE_DEFAULT</hkparam>", 0) == string::npos)))
			{
				if (open)
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

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back(storeline1[curline]);
				curline++;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					if (unsigned int(nextpoint) + 5 > i && storeline1[curline].find("</hkparam>", 0) != string::npos)
					{
						skip = true;
					}
				}
				else
				{
					if (open)
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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}

				curline++;
				output.push_back(storeline2[i]);
			}
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"numBonesPerLod\" numelements=", 0) != string::npos)
			{
				if (open)
				{
					if (skip)
					{
						closepoint = curline;
					}
					else
					{
						closepoint = curline - 1;
						output.pop_back();
					}

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					output.push_back("<!-- CLOSE -->");

					if (!skip)
					{
						output.push_back("			</hkparam>");
					}

					open = false;
				}

				nextpoint = i;
				break;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						openpoint = curline;
						open = true;
					}
				}
				else
				{
					if (open)
					{
						closepoint = curline - 1;
						output.pop_back();

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}

			output.push_back(storeline2[i]);
		}
	}

	for (unsigned int i = nextpoint; i < storeline2.size(); i++)		// numBonesPerLod data
	{
		if (storeline1[curline].find("<hkparam name=\"characterPropertyValues\">", 0) == string::npos)		// existing variable value
		{
			if (storeline1[curline + 1].find("<hkparam name=\"characterPropertyValues\">", 0) == string::npos &&
				(storeline1[curline].find("\t\t\t<hkobject>", 0) != string::npos || storeline1[curline].find("\t\t\t</hkobject>", 0) != string::npos ||
					storeline1[curline].find("\t\t\t</hkparam>", 0) != string::npos ||
					(storeline1[curline].find("\t\t\t<hkparam name=\"role\">", 0) != string::npos &&
						storeline1[curline].find("\t\t\t<hkparam name=\"role\">ROLE_DEFAULT</hkparam>", 0) == string::npos)))
			{
				if (open)
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

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back(storeline1[curline]);
				curline++;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					if (unsigned int(nextpoint) + 5 > i && storeline1[curline].find("</hkparam>", 0) != string::npos)
					{
						skip = true;
					}
				}
				else
				{
					if (open)
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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}

				curline++;
				output.push_back(storeline2[i]);
			}
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"characterPropertyValues\">", 0) != string::npos)
			{
				if (open)
				{
					if (skip)
					{
						closepoint = curline;
					}
					else
					{
						closepoint = curline - 1;
						output.pop_back();
					}

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					output.push_back("<!-- CLOSE -->");

					if (!skip)
					{
						output.push_back("			</hkparam>");
					}

					open = false;
				}

				nextpoint = i;
				break;
			}
			else
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						openpoint = curline;
						open = true;
					}
				}
				else
				{
					if (open)
					{
						closepoint = curline - 1;
						output.pop_back();

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}

			output.push_back(storeline2[i]);
		}
	}

	for (unsigned int i = nextpoint; i < storeline2.size(); i++)		 // leftover data
	{
		if (storeline1[curline].find(storeline2[i], 0) != string::npos)
		{
			if (open)
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

				output.push_back("<!-- CLOSE -->");
				open = false;
			}
		}
		else
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = curline;
				IsEdited = true;
				open = true;
			}
		}

		curline++;
		output.push_back(storeline2[i]);
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
				fwrite << output[i] << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbCharacterData Output Not Found (File: " << filename << ")" << endl;
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
