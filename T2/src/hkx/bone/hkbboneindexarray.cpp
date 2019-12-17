#include <boost\algorithm\string.hpp>
#include "hkbboneindexarray.h"

using namespace std;

namespace boneindexarray
{
	const string key = "bq";
	const string classname = "hkbBoneIndexArray";
	const string signature = "0xaa8619";
}

string hkbboneindexarray::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbboneindexarray>> hkbboneindexarrayList;
safeStringUMap<shared_ptr<hkbboneindexarray>> hkbboneindexarrayList_E;

void hkbboneindexarray::regis(string id, bool isEdited)
{
	isEdited ? hkbboneindexarrayList_E[id] = shared_from_this() : hkbboneindexarrayList[id] = shared_from_this();
	ID = id;
}

void hkbboneindexarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
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
				usize numelement;

				if (readEleParam("boneIndices", line, numelement))
				{
					boneIndices.reserve(numelement);
					++type;
				}

				break;
			}
			case 2:
			{
				if (line.find("</hkparam>") != string::npos)
				{
					++type;
					break;
				}

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

					boneIndices.push_back(stoi(each));
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << boneindexarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}	
}

void hkbboneindexarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + boneindexarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

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
			auto parent_itr = hkb_parent.find(shared_from_this());

			// existed
			if (addressID.find(address) != addressID.end() && parent_itr != hkb_parent.end() && IsForeign.find(parent_itr->second->ID) == IsForeign.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbboneindexarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbboneindexarrayList_E[ID] = protect;
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
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare, true);

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
				hkbboneindexarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbboneindexarrayList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
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

string hkbboneindexarray::getClassCode()
{
	return boneindexarray::key;
}

void hkbboneindexarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(50);
	usize base = 2;
	hkbboneindexarray* ctrpart = static_cast<hkbboneindexarray*>(counterpart.get());

	output.push_back(openObject(base, ID, boneindexarray::classname, boneindexarray::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);

	usize size = boneIndices.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "boneIndices", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "boneIndices", size));		// 2

	for (usize i = 0; i < size; ++i)
	{
		// deleted existing data
		if (i >= ctrpart->boneIndices.size())
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				storeline.push_back(autoVoid(base, boneIndices[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(boneIndices[i], ctrpart->boneIndices[i], output, storeline, base, false, open, isEdited);
		}
	}

	// newly created data
	if (size < ctrpart->boneIndices.size())
	{
		nemesis::try_open(open, isEdited, output);
		usize i = size;
		size = ctrpart->boneIndices.size();

		while (i < size)
		{
			string line;

			do
			{
				line.append(to_string(ctrpart->boneIndices[i]) + " ");
				++i;
			} while (i < size && i % 16 != 0);

			line.pop_back();
			output.push_back(autoVoid(base, line));
		}
	}

	if (size > 0)
	{
		if (boneIndices.size() == 0)
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", boneindexarray::classname, output, isEdited);
}

void hkbboneindexarray::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(50);

	output.push_back(openObject(base, ID, boneindexarray::classname, boneindexarray::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));

	usize size = boneIndices.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "boneIndices", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "boneIndices", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(to_string(boneIndices[counter]) + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, boneindexarray::classname, output, true);
}

void hkbboneindexarray::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbboneindexarray::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
