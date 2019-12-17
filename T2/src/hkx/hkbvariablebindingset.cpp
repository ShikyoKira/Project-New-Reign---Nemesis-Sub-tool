#include "hkbvariablebindingset.h"
#include "Global.h"
#include "highestscore.h"

using namespace std;

namespace variablebindingset_h
{
	const string key = "t";
	const string classname = "hkbVariableBindingSet";
	const string signature = "0x338ad4ff";
}

string hkbvariablebindingset::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbvariablebindingset>> hkbvariablebindingsetList;
safeStringUMap<shared_ptr<hkbvariablebindingset>> hkbvariablebindingsetList_E;

void hkbvariablebindingset::regis(string id, bool isEdited)
{
	isEdited ? hkbvariablebindingsetList_E[id] = shared_from_this() : hkbvariablebindingsetList[id] = shared_from_this();
	ID = id;
}

void hkbvariablebindingset::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	bool done = false;
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (!done)
		{
			usize numelement;

			if (readEleParam("bindings", line, numelement))
			{
				if (numelement > 0) bindings.reserve(numelement);

				done = true;
			}
		}
		else if (line.find("<hkparam name=\"") != string::npos)
		{
			switch (type)
			{
				case 0:
				{
					string output;

					if (readParam("memberPath", line, output))
					{
						++type;
						bindings.push_back(binding(output));
						break;
					}

					int i_output;

					if (readParam("indexOfBindingToEnable", line, i_output))
					{
						type = 0;
						indexOfBindingToEnable = i_output;
					}

					break;
				}
				case 1:
				{
					int output;

					if (readParam("variableIndex", line, output))
					{
						++type;
						bindings.back().variableIndex = output;
					}

					break;
				}
				case 2:
				{
					int output;

					if (readParam("bitIndex", line, output))
					{
						++type;
						bindings.back().bitIndex = output;
					}

					break;
				}
				case 3:
				{
					string output;

					if (readParam("bindingType", line, output))
					{
						type = 0;
						bindings.back().bindingType = output == "BINDING_TYPE_VARIABLE" ? binding::BINDING_TYPE_VARIABLE : binding::BINDING_TYPE_CHARACTER_PROPERTY;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << variablebindingset_h::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbvariablebindingset::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + variablebindingset_h::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	
	for (auto& binding : bindings)
	{
		binding.variableIndex.connectVariableInfo(ID, graphroot);
	}

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
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
				hkbvariablebindingsetList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbvariablebindingsetList_E[ID] = protect;
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
		CrossReferencing(shared_from_this(), address, functionlayer, compare, true);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
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

			auto foreign_itr = IsForeign.find(ID);

			if (foreign_itr != IsForeign.end()) IsForeign.erase(foreign_itr);

			auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
			hkbvariablebindingsetList_E.erase(ID);
			editedBehavior.erase(ID);
			IsExist.erase(ID);
			ID = addressID[address];
			hkbvariablebindingsetList_E[ID] = protect;
			editedBehavior[ID] = protect;
			RecordID(ID, address, true);
			nextNode(filepath, functionlayer, true, graphroot);
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

string hkbvariablebindingset::getClassCode()
{
	return variablebindingset_h::key;
}

void hkbvariablebindingset::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = bindings.size();
	usize orisize = size;
	hkbvariablebindingset* ctrpart = static_cast<hkbvariablebindingset*>(counterpart.get());

	output.push_back(openObject(base, ID, variablebindingset_h::classname, variablebindingset_h::signature));	// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "bindings", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bindings", size));		// 2

	matchScoring(bindings, ctrpart->bindings, ID);
	size = bindings.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (bindings[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "memberPath", ctrpart->bindings[i].memberPath));
				output.push_back(autoParam(base, "variableIndex", ctrpart->bindings[i].variableIndex));
				output.push_back(autoParam(base, "bitIndex", ctrpart->bindings[i].bitIndex));
				output.push_back(autoParam(base, "bindingType", ctrpart->bindings[i].getBindingType()));
				output.push_back(closeObject(base));	// 3
				++i;
			}
		}
		// deleted existing data
		else if (ctrpart->bindings[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!ctrpart->bindings[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "memberPath", bindings[i].memberPath));
				storeline.push_back(autoParam(base, "variableIndex", bindings[i].variableIndex));
				storeline.push_back(autoParam(base, "bitIndex", bindings[i].bitIndex));
				storeline.push_back(autoParam(base, "bindingType", bindings[i].getBindingType()));
				storeline.push_back(closeObject(base));		// 3
				++i;
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");
				output.push_back("");	// 6 spaces
			}
		}
		// both exist
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(openObject(base));		// 3
			paramMatch("memberPath", bindings[i].memberPath, ctrpart->bindings[i].memberPath, output, storeline, base, false, open, isEdited);
			paramMatch("variableIndex", bindings[i].variableIndex, ctrpart->bindings[i].variableIndex, output, storeline, base, false, open, isEdited);
			paramMatch("bitIndex", bindings[i].bitIndex, ctrpart->bindings[i].bitIndex, output, storeline, base, false, open, isEdited);
			paramMatch("bindingType", bindings[i].getBindingType(), ctrpart->bindings[i].getBindingType(), output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));	// 3
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

	paramMatch("indexOfBindingToEnable", indexOfBindingToEnable, ctrpart->indexOfBindingToEnable, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", variablebindingset_h::classname, output, isEdited);
}

void hkbvariablebindingset::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = bindings.size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, variablebindingset_h::classname, variablebindingset_h::signature));		// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "bindings", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bindings", size));		// 2

	for (auto& binding : bindings)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "memberPath", binding.memberPath));
		output.push_back(autoParam(base, "variableIndex", binding.variableIndex));
		output.push_back(autoParam(base, "bitIndex", binding.bitIndex));
		output.push_back(autoParam(base, "bindingType", binding.getBindingType()));
		output.push_back(closeObject(base));	// 3
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "indexOfBindingToEnable", indexOfBindingToEnable));
	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, variablebindingset_h::classname, output, true);
}

void hkbvariablebindingset::matchScoring(vector<binding>& ori, vector<binding>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<binding> newOri;
		vector<binding> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(binding());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	map<int, map<int, int>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].memberPath == edit[j].memberPath)
			{
				scorelist[i][j] += 100;
			}

			if (ori[i].variableIndex == edit[j].variableIndex)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].bitIndex == edit[j].bitIndex)
			{
				++scorelist[i][j];
			}

			if (ori[i].bindingType == edit[j].bindingType)
			{
				++scorelist[i][j];
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<binding> newOri;
	vector<binding> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(binding());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(binding());
		else newEdit.push_back(edit[order.edited]);
	}

	ori = newOri;
	edit = newEdit;
}

string hkbvariablebindingset::binding::getBindingType()
{
	return bindingType == BINDING_TYPE_CHARACTER_PROPERTY ? "BINDING_TYPE_CHARACTER_PROPERTY" : "BINDING_TYPE_VARIABLE";
}
