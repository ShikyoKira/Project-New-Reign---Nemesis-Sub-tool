#include <boost\regex.hpp>
#include "hkrootlevelcontainer.h"
#include "highestscore.h"

using namespace std;

namespace rootlevelcontainer
{
	const string key = "n";
	const string classname = "hkRootLevelContainer";
	const string signature = "0x2772c11e";
}

string hkrootlevelcontainer::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkrootlevelcontainer>> hkrootlevelcontainerList;
safeStringUMap<shared_ptr<hkrootlevelcontainer>> hkrootlevelcontainerList_E;

void hkrootlevelcontainer::regis(string id, bool isEdited)
{
	isEdited ? hkrootlevelcontainerList_E[id] = shared_from_this() : hkrootlevelcontainerList[id] = shared_from_this();
	ID = id;
}

void hkrootlevelcontainer::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 3;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				string getdata;

				if (readParam("name", line, getdata))
				{
					namedVariants.push_back(getdata);
					++type;
				}

				break;
			}
			case 1:
			{
				if (readParam("className", line, namedVariants.back().className)) ++type;

				break;
			}
			case 2:
			{
				string getdata;

				if (readParam("variant", line, getdata))
				{
					namedVariants.back().variant = (isEdited ? editedBehavior : originalBehavior)[getdata];
					type = 0;
				}

				break;
			}
			case 3:
			{
				usize numelement;

				if (readEleParam("namedVariants", line, numelement))
				{
					namedVariants.reserve(numelement);
					type = 0;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << rootlevelcontainer::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkrootlevelcontainer::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + rootlevelcontainer::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			usize tempint = 0;
			string curadd = address;
			curLock.unlock();

			for (auto& variant : namedVariants)
			{
				if (variant.variant) variant.variant->connect(filepath, curadd + to_string(tempint++), functionlayer, false, graphroot);
			}
		}
		else
		{
			auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
			hkrootlevelcontainerList_E.erase(ID);
			editedBehavior.erase(ID);
			ID = addressID[address];
			hkrootlevelcontainerList_E[ID] = protect;
			editedBehavior[ID] = protect;
			RecordID(ID, address, true);
			nextNode(filepath, functionlayer, true, graphroot);
		}
	}
	else
	{
		cout << "ERROR: Invalid reference. Root can only be refered once" << endl;
		Error = true;
	}
}

string hkrootlevelcontainer::getClassCode()
{
	return rootlevelcontainer::key;
}

void hkrootlevelcontainer::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr output;
	vecstr storeline;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = namedVariants.size();
	usize orisize = size;
	hkrootlevelcontainer* ctrpart = static_cast<hkrootlevelcontainer*>(counterpart.get());

	output.push_back(openObject(base, ID, rootlevelcontainer::classname, rootlevelcontainer::signature));	// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "namedVariants", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "namedVariants", size));		// 2

	matchScoring(namedVariants, ctrpart->namedVariants, ID);
	size = namedVariants.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (namedVariants[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "name", ctrpart->namedVariants[i].name));
				output.push_back(autoParam(base, "className", ctrpart->namedVariants[i].className));
				output.push_back(autoParam(base, "variant", ctrpart->namedVariants[i].variant));
				output.push_back(closeObject(base));	// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->namedVariants[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->namedVariants[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "name", namedVariants[i].name));
				storeline.push_back(autoParam(base, "className", namedVariants[i].className));
				storeline.push_back(autoParam(base, "variant", namedVariants[i].variant));
				storeline.push_back(closeObject(base));		// 3
				++i;
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");	// 5 spaces
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));			// 3
			paramMatch("name", namedVariants[i].name, ctrpart->namedVariants[i].name, output, storeline, base, false, open, isEdited);
			paramMatch("className", namedVariants[i].className, ctrpart->namedVariants[i].className, output, storeline, base, false, open, isEdited);
			paramMatch("variant", namedVariants[i].variant, ctrpart->namedVariants[i].variant, output, storeline, base, true, open, isEdited);
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

	output.push_back(closeObject(base));			// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", rootlevelcontainer::classname, output, isEdited);
}

void hkrootlevelcontainer::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = namedVariants.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, rootlevelcontainer::classname, rootlevelcontainer::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "namedVariants", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "namedVariants", size));		// 2

	for (auto& variant : namedVariants)
	{
		output.push_back(openObject(base));										// 3
		output.push_back(autoParam(base, "name", variant.name));
		output.push_back(autoParam(base, "className", variant.className));
		output.push_back(autoParam(base, "variant", variant.variant));
		output.push_back(closeObject(base));									// 3
	}

	if (size > 0) output.push_back(closeParam(base));			// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, rootlevelcontainer::classname, output, true);
}

void hkrootlevelcontainer::parentRefresh()
{
	for (auto& variant : namedVariants)
	{
		if (variant.variant) hkb_parent[variant.variant] = shared_from_this();
	}
}

void hkrootlevelcontainer::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	++functionlayer;	
	usize tempint = 0;

	for (auto& variant : namedVariants)
	{
		if (variant.variant)
		{
			parentRefresh();
			variant.variant->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

			if (IsForeign.find(variant.variant->ID) == IsForeign.end()) ++tempint;
		}
	}
}

void hkrootlevelcontainer::matchScoring(vector<namedvariant>& ori, vector<namedvariant>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<namedvariant> newOri;
		vector<namedvariant> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(namedvariant());
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

			if (ori[i].name == edit[j].name)
			{
				scorelist[i][j] += 100;
			}

			if (ori[i].className == edit[j].className)
			{
				scorelist[i][j] += 100;
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
	vector<namedvariant> newOri;
	vector<namedvariant> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(namedvariant());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(namedvariant());
		else newEdit.push_back(edit[order.edited]);
	}

	ori = newOri;
	edit = newEdit;
}
