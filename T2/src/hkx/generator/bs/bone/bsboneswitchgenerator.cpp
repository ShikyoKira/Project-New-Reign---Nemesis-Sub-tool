#include <boost\thread.hpp>
#include "bsboneswitchgenerator.h"
#include "generatorlines.h"

using namespace std;

namespace boneswitchgenerator
{
	const string key = "e";
	const string classname = "BSBoneSwitchGenerator";
	const string signature = "0xf33d3eea";
}

string bsboneswitchgenerator::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<bsboneswitchgenerator>> bsboneswitchgeneratorList;
safeStringUMap<shared_ptr<bsboneswitchgenerator>> bsboneswitchgeneratorList_E;

void bsboneswitchgenerator::regis(string id, bool isEdited)
{
	isEdited ? bsboneswitchgeneratorList_E[id] = shared_from_this() : bsboneswitchgeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void bsboneswitchgenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos || line.find("\t#") != string::npos)
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
					string output;

					if (readParam("pDefaultGenerator", line, output))
					{
						pDefaultGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 4:
				{
					usize numelement;

					if (readEleParam("ChildrenA", line, numelement))
					{
						ChildrenA.reserve(numelement);
						++type;
					}

					break;
				}
				case 5:
				{
					vecstr reflist;

					if (getNodeRefList(line, reflist))
					{
						for (auto& ref : reflist)
						{
							ChildrenA.emplace_back((isEdited ? bsboneswitchgeneratorbonedataList_E : bsboneswitchgeneratorbonedataList)[ref]);
						}
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << boneswitchgenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsboneswitchgenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + boneswitchgenerator::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + boneswitchgenerator::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (pDefaultGenerator) threadedNextNode(pDefaultGenerator, filepath, curadd, functionlayer, graphroot);

			for (usize i = 0; i < ChildrenA.size(); ++i)
			{
				threadedNextNode(ChildrenA[i], filepath, curadd + to_string(i), functionlayer, graphroot);
			}
		}
		else
		{
			// existed
			if (IsOldFunction(filepath, shared_from_this(), address))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				bsboneswitchgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsboneswitchgeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
				editedBehavior[ID] = protect;
				address = region[ID];
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
	else if (!compare || IsForeign.find(ID) != IsForeign.end())
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
				bsboneswitchgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsboneswitchgeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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
}

string bsboneswitchgenerator::getClassCode()
{
	return boneswitchgenerator::key;
}

void bsboneswitchgenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(20);
	output.reserve(20);
	usize base = 2;
	bsboneswitchgenerator* ctrpart = static_cast<bsboneswitchgenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, boneswitchgenerator::classname, boneswitchgenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, true, open, isEdited);
	paramMatch("pDefaultGenerator", pDefaultGenerator, ctrpart->pDefaultGenerator, output, storeline, base, true, open, isEdited);
	
	usize size = ChildrenA.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "ChildrenA", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "ChildrenA", size));		// 2

	matchScoring(ChildrenA, ctrpart->ChildrenA, ID);
	size = ChildrenA.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (ChildrenA[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(autoVoid(base, ctrpart->ChildrenA[i]));
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->ChildrenA[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->ChildrenA[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, ChildrenA[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(ChildrenA[i], ctrpart->ChildrenA[i], output, storeline, base, false, open, isEdited);
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

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", boneswitchgenerator::classname, output, isEdited);
}

void bsboneswitchgenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(20);

	output.push_back(openObject(base, ID, boneswitchgenerator::classname, boneswitchgenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "pDefaultGenerator", pDefaultGenerator));

	usize size = ChildrenA.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "ChildrenA", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "ChildrenA", size));		// 2

	for (auto& child : ChildrenA)
	{
		output.push_back(autoVoid(base, child->ID));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, boneswitchgenerator::classname, output, true);
}

void bsboneswitchgenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pDefaultGenerator) hkb_parent[pDefaultGenerator] = shared_from_this();

	for (auto child : ChildrenA)
	{
		hkb_parent[child] = shared_from_this();
	}
}

void bsboneswitchgenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) functionlayer = 0;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pDefaultGenerator)
	{
		parentRefresh();
		pDefaultGenerator->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (isOld)
	{
		usize tempint = 0;

		for (auto& child : ChildrenA)
		{
			parentRefresh();
			child->connect(filepath, address + to_string(tempint), 0, true, graphroot);

			if (IsForeign.find(child->ID) == IsForeign.end()) ++tempint;
		}
	}
	else
	{
		for (auto& child : ChildrenA)
		{
			parentRefresh();
			child->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void bsboneswitchgenerator::matchScoring(vector<shared_ptr<bsboneswitchgeneratorbonedata>>& ori, vector<shared_ptr<bsboneswitchgeneratorbonedata>>& edit, std::string id)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<bsboneswitchgeneratorbonedata>> newOri;
		vector<shared_ptr<bsboneswitchgeneratorbonedata>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<bsboneswitchgeneratorbonedata>());
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

			if (ori[i]->ID == edit[j]->ID)
			{
				scorelist[i][j] += 5;
			}

			if (i == j)
			{
				scorelist[i][j] += 2;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 2;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<bsboneswitchgeneratorbonedata>> newOri;
	vector<shared_ptr<bsboneswitchgeneratorbonedata>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<bsboneswitchgeneratorbonedata>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<bsboneswitchgeneratorbonedata>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void bsboneswitchgenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}
