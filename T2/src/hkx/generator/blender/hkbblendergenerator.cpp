#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

#include "hkbblendergenerator.h"
#include "generatorlines.h"
#include "src/utilities/hkMap.h"

using namespace std;

namespace blendergenerator
{
	const string key = "i";
	const string classname = "hkbBlenderGenerator";
	const string signature = "0x22df7147";

	hkMap<string, hkbblendergenerator::blenderflags> flagMap
	{
		{ "FLAG_SYNC", hkbblendergenerator::FLAG_SYNC },
		{ "FLAG_SMOOTH_GENERATOR_WEIGHTS", hkbblendergenerator::FLAG_SMOOTH_GENERATOR_WEIGHTS },
		{ "FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS", hkbblendergenerator::FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS },
		{ "FLAG_PARAMETRIC_BLEND", hkbblendergenerator::FLAG_PARAMETRIC_BLEND },
		{ "FLAG_IS_PARAMETRIC_BLEND_CYCLIC", hkbblendergenerator::FLAG_IS_PARAMETRIC_BLEND_CYCLIC },
		{ "FLAG_FORCE_DENSE_POSE", hkbblendergenerator::FLAG_FORCE_DENSE_POSE },
	};
}

string hkbblendergenerator::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbblendergenerator>> hkbblendergeneratorList;
safeStringUMap<shared_ptr<hkbblendergenerator>> hkbblendergeneratorList_E;

void hkbblendergenerator::regis(string id, bool isEdited)
{
	isEdited ? hkbblendergeneratorList_E[id] = shared_from_this() : hkbblendergeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbblendergenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos || line.find("	#") != string::npos)
		{
			switch(type)
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
					if (readParam("referencePoseWeightThreshold", line, referencePoseWeightThreshold)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("blendParameter", line, blendParameter)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("minCyclicBlendParameter", line, minCyclicBlendParameter)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("maxCyclicBlendParameter", line, maxCyclicBlendParameter)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("indexOfSyncMasterChild", line, indexOfSyncMasterChild)) ++type;

					break;
				}
				case 8:
				{
					string output;

					if (readParam("flags", line, output))
					{
						if (isOnlyNumber(output))
						{
							flags = static_cast<blenderflags>(stoi(output));
						}
						else
						{
							vecstr list;
							boost::trim_if(output, boost::is_any_of("\t "));
							boost::split(list, output, boost::is_any_of("|"), boost::token_compress_on);
							usize data = static_cast<usize>(flags);

							for (auto& flg : list)
							{
								if (flg == "0") continue;

								data |= blendergenerator::flagMap[flg];
							}

							flags = static_cast<hkbblendergenerator::blenderflags>(data);
						}

						++type;
					}

					break;
				}
				case 9:
				{
					if (readParam("subtractLastChild", line, subtractLastChild)) ++type;

					break;
				}
				case 10:
				{
					usize numelement;

					if (readEleParam("children", line, numelement))
					{
						children.reserve(numelement);
						++type;
					}

					break;
				}
				case 11:
				{
					vecstr reflist;

					if (getNodeRefList(line, reflist))
					{
						for (auto& ref : reflist)
						{
							children.emplace_back((isEdited ? hkbblendergeneratorchildList_E : hkbblendergeneratorchildList)[ref]);
						}
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << blendergenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbblendergenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + blendergenerator::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);


	if (name == "1HM_Staff_SprintForward_3rdP_Sword")
	{
		Error = false;
	}

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + blendergenerator::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				boost::thread_group multithreads;

				for (usize i = 0; i < children.size(); ++i)
				{
					multithreads.create_thread(boost::bind(&hkbblendergenerator::threadedNextNode, this, children[i], filepath, curadd + to_string(i), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
				multithreads.join_all();
				num_thread -= multithreads.size();
			}
			else
			{
				for (usize i = 0; i < children.size(); ++i)
				{
					threadedNextNode(children[i], filepath, curadd + to_string(i), functionlayer, graphroot);
				}
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
				hkbblendergeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbblendergeneratorList_E[ID] = protect;
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
				hkbblendergeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbblendergeneratorList_E[ID] = protect;
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

string hkbblendergenerator::getClassCode()
{
	return blendergenerator::key;
}

void hkbblendergenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(50);
	usize base = 2;
	hkbblendergenerator* ctrpart = static_cast<hkbblendergenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, blendergenerator::classname, blendergenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("referencePoseWeightThreshold", referencePoseWeightThreshold, ctrpart->referencePoseWeightThreshold, output, storeline, base, false, open, isEdited);
	paramMatch("blendParameter", blendParameter, ctrpart->blendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("minCyclicBlendParameter", minCyclicBlendParameter, ctrpart->minCyclicBlendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("maxCyclicBlendParameter", maxCyclicBlendParameter, ctrpart->maxCyclicBlendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("indexOfSyncMasterChild", indexOfSyncMasterChild, ctrpart->indexOfSyncMasterChild, output, storeline, base, false, open, isEdited);
	paramMatch("flags", static_cast<int>(flags), static_cast<int>(ctrpart->flags), output, storeline, base, false, open, isEdited);
	paramMatch("subtractLastChild", subtractLastChild, ctrpart->subtractLastChild, output, storeline, base, true, open, isEdited);

	usize size = children.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "children", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "children", size));		// 2

	matchScoring(children, ctrpart->children, ID, blendergenerator::classname);
	size = children.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (children[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->children[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->children[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->children[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, children[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(children[i], ctrpart->children[i], output, storeline, base, false, open, isEdited);
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", blendergenerator::classname, output, isEdited);
}

void hkbblendergenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(50);

	output.push_back(openObject(base, ID, blendergenerator::classname, blendergenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "referencePoseWeightThreshold", referencePoseWeightThreshold));
	output.push_back(autoParam(base, "blendParameter", blendParameter));
	output.push_back(autoParam(base, "minCyclicBlendParameter", minCyclicBlendParameter));
	output.push_back(autoParam(base, "maxCyclicBlendParameter", maxCyclicBlendParameter));
	output.push_back(autoParam(base, "indexOfSyncMasterChild", indexOfSyncMasterChild));
	output.push_back(autoParam(base, "flags", static_cast<int>(flags)));
	output.push_back(autoParam(base, "subtractLastChild", subtractLastChild));

	usize size = children.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "children", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "children", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(children[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, blendergenerator::classname, output, true);
}

void hkbblendergenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto child : children)
	{
		hkb_parent[child] = shared_from_this();
	}
}

void hkbblendergenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) functionlayer = 0;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (isOld)
	{
		usize tempint = 0;

		for (auto& child : children)
		{
			parentRefresh();
			child->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

			if (IsForeign.find(child->ID) == IsForeign.end()) ++tempint;
			else
			{
				auto branch_itr = IsBranchOrigin.find(child->ID);

				if (branch_itr != IsBranchOrigin.end())
				{
					++tempint;
					IsBranchOrigin.erase(branch_itr);
				}
			}
		}
	}
	else
	{
		for (auto& child : children)
		{
			parentRefresh();
			child->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbblendergenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void blenderBase::matchScoring(vector<shared_ptr<hkbblendergeneratorchild>>& ori, vector<shared_ptr<hkbblendergeneratorchild>>& edit, string id, string classname)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<hkbblendergeneratorchild>> newOri;
		vector<shared_ptr<hkbblendergeneratorchild>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<hkbblendergeneratorchild>());
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
	vector<shared_ptr<hkbblendergeneratorchild>> newOri;
	vector<shared_ptr<hkbblendergeneratorchild>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<hkbblendergeneratorchild>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<hkbblendergeneratorchild>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}
