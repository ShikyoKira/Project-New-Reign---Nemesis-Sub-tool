#include <boost\algorithm\\string.hpp>
#include "hkbmirroredskeletoninfo.h"
#include "Global.h"

using namespace std;

namespace mirroredskeletoninfo
{
	const string key = "cb";
	const string classname = "hkbMirroredSkeletonInfo";
	const string signature = "0xc6c2da4f";
}

string hkbmirroredskeletoninfo::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbmirroredskeletoninfo>> hkbmirroredskeletoninfoList;
safeStringUMap<shared_ptr<hkbmirroredskeletoninfo>> hkbmirroredskeletoninfoList_E;

void hkbmirroredskeletoninfo::regis(string id, bool isEdited)
{
	isEdited ? hkbmirroredskeletoninfoList_E[id] = shared_from_this() : hkbmirroredskeletoninfoList[id] = shared_from_this();
	ID = id;
}

void hkbmirroredskeletoninfo::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				if (readParam("mirrorAxis", line, mirrorAxis)) ++type;

				break;
			}
			case 1:
			{
				usize numelement;

				if (readEleParam("bonePairMap", line, numelement))
				{
					bonePairMap.reserve(numelement);
					++type;
				}
			}
			case 2:
			{
				string temp = line;
				vecstr list;
				boost::trim_if(temp, boost::is_any_of("\t "));
				boost::split(list, temp, boost::is_any_of("\t "), boost::token_compress_on);

				for (auto& each : list)
				{
					if (!isOnlyNumber(each))
					{
						++type;
						break;
					}

					bonePairMap.push_back(stoi(each));
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << mirroredskeletoninfo::classname + " (ID: " << ID << ") is complete!" << endl;
	}	
}

void hkbmirroredskeletoninfo::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + mirroredskeletoninfo::key + to_string(functionlayer) + ">";
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
				hkbmirroredskeletoninfoList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbmirroredskeletoninfoList_E[ID] = protect;
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
				hkbmirroredskeletoninfoList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbmirroredskeletoninfoList_E[ID] = protect;
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

string hkbmirroredskeletoninfo::getClassCode()
{
	return mirroredskeletoninfo::key;
}

void hkbmirroredskeletoninfo::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = bonePairMap.size();
	usize orisize = size;
	hkbmirroredskeletoninfo* ctrpart = static_cast<hkbmirroredskeletoninfo*>(counterpart.get());

	output.push_back(openObject(base, ID, mirroredskeletoninfo::classname, mirroredskeletoninfo::signature));		// 1
	paramMatch("mirrorAxis", mirrorAxis, ctrpart->mirrorAxis, output, storeline, base, true, open, isEdited);

	if (size == 0)
	{
		output.push_back(openParam(base, "bonePairMap", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bonePairMap", size));		// 2

	for (usize i = 0; i < size; ++i)
	{
		if (i >= ctrpart->bonePairMap.size())
		{
			while (i < size)
			{
				storeline.push_back(to_string(bonePairMap[i]));
				output.push_back("");
				++i;
			}
		}
		else
		{
			voidMatch(bonePairMap[i], ctrpart->bonePairMap[i], output, storeline, base, false, open, isEdited);
		}
	}

	if (size < ctrpart->bonePairMap.size())
	{
		usize i = size;
		size = ctrpart->bonePairMap.size();
		nemesis::try_open(open, isEdited, output);

		while (i < size)
		{
			output.push_back(to_string(ctrpart->bonePairMap[i]));
			++i;
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

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", mirroredskeletoninfo::classname, output, isEdited);
}

void hkbmirroredskeletoninfo::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = bonePairMap.size();
	usize counter = 0;
	string line = "				";
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, mirroredskeletoninfo::classname, mirroredskeletoninfo::signature));		// 1
	output.push_back(autoParam(base, "mirrorAxis", mirrorAxis.getString()));

	if (size == 0)
	{
		output.push_back(openParam(base, "bonePairMap", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bonePairMap", size));		// 2

	for (auto& bone : bonePairMap)
	{
		line.append(to_string(bone) + " ");
		
		if (++counter == 16)
		{
			line.pop_back();
			output.push_back(line);
			line = "				";
			counter = 0;
		}
	}

	if (counter != 0)
	{
		line.pop_back();
		output.push_back(line);
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, mirroredskeletoninfo::classname, output, true);
}
