#include <boost\atomic.hpp>
#include <boost\algorithm\string\split.hpp>

#include "Global.h"

#include "src\hkx\condition\condition.h"
#include "src\hkx\hkbcharacterstringdata.h"
#include "src\hkx\modifier\hkbmodifierlist.h"
#include "src\hkx\generator\hkbmanualselectorgenerator.h"
#include "src\hkx\generator\bs\bone\bsboneswitchgenerator.h"
#include "src\hkx\generator\blender\hkbblendergeneratorchild.h"
#include "src\hkx\generator\blender\hkbposematchinggenerator.h"
#include "src\hkx\generator\statemachine\hkbstatemachine.h"
#include "src\hkx\generator\statemachine\hkbstatemachinetransitioninfoarray.h"

using namespace std;

bool Error = false;
bool Debug = false;
nodelist FunctionLineNew;		// function new data
nodelist FunctionLineTemp;		// function temp data
nodelist FunctionLineOriginal;	// function original data
nodelist FunctionLineEdited;	// function edited data
hkRefPtr originalBehavior;
hkRefPtr editedBehavior;
set<int> idcount;
safeStringUMap<bool> IsExist;
map<string, string> addressID;
unordered_map<string, int> elements;
unordered_map<string, bool> IsForeign;
unordered_map<string, bool> IsFileInUse;
unordered_map<string, bool> IsBranchOrigin;
unordered_map<string, bool> PerfectMatchCondition;
mapstring parent;
mapstring exchangeID;
mapstring addressChange;				// temp address change check
vector<mapstring> eventID;
vector<mapstring> variableID;
vector<mapstring> attributeID;
vector<mapstring> characterID;
unordered_map<string, vecstr> referencingIDs;		// which function use this ID
unordered_map<shared_ptr<hkbobject>, shared_ptr<hkbobject>> hkb_parent;
vector<shared_ptr<AnimDataProject>> AnimDataOriginal;
vector<shared_ptr<AnimDataProject>> AnimDataEdited;
vector<shared_ptr<AnimSetDataProject>> AnimSetDataOriginal;
vector<shared_ptr<AnimSetDataProject>> AnimSetDataEdited;
string modcode = "null";
string targetfilename;
string targetfilenameedited;
string shortFileName;
string shortFileNameEdited;
int functioncount = 0;
vector<unsigned int> eventCount;
vector<unsigned int> varCount;

mutex addresslock;
boost::atomic_flag existlock = BOOST_ATOMIC_FLAG_INIT;
boost::atomic_flag idlock = BOOST_ATOMIC_FLAG_INIT;

bool IsOldRegion(shared_ptr<hkbobject> id, string address, short classcode, bool special);
bool IsOldFunctionExt(string filename, shared_ptr<hkbobject> hkb_obj, string address, bool condition);
bool blendingOldFunction(shared_ptr<hkbobject> hkb_obj, string address, int functionlayer);
void conditionCheckAndReplace(string oriID, shared_ptr<hkbobject> hkb_obj);

int sameWordCount(string line, string word)
{
	size_t nextWord = 0;
	int wordCount = 0;

	while (true)
	{
		nextWord = line.find(word, nextWord + 1);

		if (nextWord != -1)
		{
			wordCount++;
		}
		else
		{
			break;
		}
	}

	return wordCount;
}

bool IsFileExist(const string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool IsOldFunction(string filename, shared_ptr<hkbobject> id, string address)
{
	if (IsOldFunctionExt(filename, id, address, false))
	{
		if (IsForeign.find(hkb_parent[id]->ID) != IsForeign.end())
		{
			shared_ptr<hkbobject> tempparent = hkb_parent[id];

			while (IsForeign.find(hkb_parent[tempparent]->ID) != IsForeign.end())
			{
				tempparent = hkb_parent[tempparent];
			}

			IsBranchOrigin[tempparent->ID] = true;
		}

		return true;
	}

	return false;
}

bool IsOldFunctionExt(string filename, shared_ptr<hkbobject> hkb_obj, string address, bool condition)
{
	string tempadd = address.substr(address.find_last_of("="));

	if (filename != "ZeroRegion")
	{
		if (count(tempadd.begin(), tempadd.end(), '>') == 2)
		{
			if (address.find("(m", 0) != string::npos) return IsOldRegion(hkb_obj, address, 2, false);
			else if (address.find("(r", 0) != string::npos) return IsOldRegion(hkb_obj, address, 3, false);
			else if (address.find("(y", 0) != string::npos) return IsOldRegion(hkb_obj, address, 4, false);
			else if (address.find("(e", 0) != string::npos && tempadd.find("ct") != string::npos) return IsOldRegion(hkb_obj, address, 1, false);
		}
		else if (count(tempadd.begin(), tempadd.end(), '>') == 3)
		{
			if (address.find("(cj", 0) != string::npos) return IsOldRegion(hkb_obj, address, 1, true);
			else if (address.find("(i", 0) != string::npos) return IsOldRegion(hkb_obj, address, 2, true);
		}
	}

	auto& add_itr = addressID.find(address);

	if (add_itr != addressID.end() && add_itr->second.length() > 0)
	{
		string tempOldID = add_itr->second;
		
		if (IsExist.find(tempOldID) != IsExist.end())
		{
			if (tempOldID == hkb_obj->ID && IsForeign.find(tempOldID) == IsForeign.end()) return true;

			if (!condition)
			{
				if (editedBehavior.find(tempOldID) != editedBehavior.end()) return false;
			}
		}

		string classname = hkb_obj->getClassCode();

		if (classname != originalBehavior[tempOldID]->getClassCode()) return false;

		// if it is variableBindingSet
		if (classname == "l")
		{
			hkbexpressioncondition* e_obj = static_cast<hkbexpressioncondition*>(hkb_obj.get());
			shared_ptr<hkbexpressioncondition> o_obj = hkbexpressionconditionList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			return o_obj->expression == e_obj->expression ? true : false;
		}
		else if (classname == "ck")
		{
			hkbstringcondition* e_obj = static_cast<hkbstringcondition*>(hkb_obj.get());
			shared_ptr<hkbstringcondition> o_obj = hkbstringconditionList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			return o_obj->conditionString == e_obj->conditionString ? true : false;
		}
		else if (classname == "p")
		{
			hkbstatemachinestateinfo* e_obj = static_cast<hkbstatemachinestateinfo*>(hkb_obj.get());
			shared_ptr<hkbstatemachinestateinfo> o_obj = hkbstatemachinestateinfoList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			if (IsForeign.find(hkb_parent[hkb_obj]->ID) != IsForeign.end()) return false;

			return o_obj->name == e_obj->name ? true : false;
		}
		else if (classname == "cc")
		{
			hkbcharacterstringdata* e_obj = static_cast<hkbcharacterstringdata*>(hkb_obj.get());
			shared_ptr<hkbcharacterstringdata> o_obj = hkbcharacterstringdataList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			return o_obj->name == e_obj->name ? true : false;
		}
		else if (classname == "a")
		{
			bsboneswitchgeneratorbonedata* e_obj = static_cast<bsboneswitchgeneratorbonedata*>(hkb_obj.get());
			shared_ptr<bsboneswitchgeneratorbonedata> o_obj = bsboneswitchgeneratorbonedataList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			if (o_obj->pGenerator && !e_obj->pGenerator || !o_obj->pGenerator && e_obj->pGenerator) return false;

			return o_obj->pGenerator->name == e_obj->pGenerator->name ? true : false;
		}
		else if (classname == "h")
		{
			hkbblendergeneratorchild* e_obj = static_cast<hkbblendergeneratorchild*>(hkb_obj.get());
			shared_ptr<hkbblendergeneratorchild> o_obj = hkbblendergeneratorchildList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			return o_obj->weight == e_obj->weight && o_obj->worldFromModelWeight == e_obj->worldFromModelWeight ? true : false;
		}
		else if (hkb_obj->isGenerator)
		{
			hkbgenerator* e_obj = static_cast<hkbgenerator*>(hkb_obj.get());
			shared_ptr<hkbgenerator> o_obj = hkbgeneratorList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			return o_obj->name == e_obj->name ? true : false;
		}
		else if (hkb_obj->isModifier)
		{
			hkbmodifier* e_obj = static_cast<hkbmodifier*>(hkb_obj.get());
			shared_ptr<hkbmodifier> o_obj = hkbmodifierList[tempOldID];

			if (o_obj == nullptr)
			{
				cout << "ERROR: Invalid reference of original (Global)" << endl;
				Error = true;
				throw 5;
			}

			if (e_obj == nullptr) return false;

			return o_obj->name == e_obj->name ? true : false;
		}
	}

	return false;
}

bool createDirectories(string directoryPath)
{
	return boost::filesystem::exists(directoryPath) || boost::filesystem::create_directories(directoryPath);
}

string ExePath()
{
	string path = boost::filesystem::current_path().string();
	string::size_type pos = path.find_last_of("\\/");
	return path.substr(0, pos);
}

string CrossReferencing(shared_ptr<hkbobject> hkb_obj, string address, int functionlayer, bool compare, bool special)
{
	if (!compare)
	{
		lock_guard<mutex> lock(addresslock);

		if (address.substr(address.find("=")).find("region", 0) == string::npos)
		{
			unordered_map<string, string> tempmap;

			for (auto& o_add : hkb_obj->poolAddress)
			{
				auto& it = addressID.find(o_add);

				while (it != addressID.end() && it->first.find(o_add) != string::npos)
				{
					string tempadd = it->first;

					if (tempadd != o_add)
					{
						string subadd = tempadd.substr(o_add.length());
						int newlayer = functionlayer + 1;
						vecstr tokens;
						boost::split(tokens, subadd, [](char c) { return c == '>'; });
						tempadd = address;

						for (auto& token : tokens)
						{
							bool unlock = false;

							for (auto& chr : token)
							{
								if (isdigit(chr))
								{
									if (unlock)
									{
										tempadd.append(to_string(newlayer++) + ">");
										break;
									}
								}
								else
								{
									unlock = true;
								}

								tempadd.push_back(chr);
							}
						}

						tempmap[tempadd] = it->second;
					}

					++it;
				}
			}

			for (auto it = tempmap.begin(); it != tempmap.end(); it++)
			{
				addressID[it->first] = it->second;
			}
		}

		auto addptr = addressID.find(address);

		if (addptr == addressID.end())
		{
			addressID[address] = hkb_obj->ID;
		}

		return hkb_obj->ID;
	}
	else
	{
		string tempadd = address.substr(address.rfind("="));
		bool pass = false;
		string newID;

		if (tempadd.find("l") != string::npos || tempadd.find("ck") != string::npos)
		{
			string addline = address;
			addline.pop_back();
			addline = addline.substr(addline.find_last_of(">") + 1) + ">";
			bool ck = false;

			if (tempadd.find("l") != string::npos)
			{
				addline = boost::regex_replace(string(addline), boost::regex("([(0-9]+l[0-9]+)>$"), string("\\1"));
			}
			else
			{
				addline = boost::regex_replace(string(addline), boost::regex("([(0-9]+ck[0-9]+)>$"), string("\\1"));
				ck = true;
			}

			if (addline.back() == '>')
			{
				if (special && hkb_parent.find(hkb_obj) != hkb_parent.end() && IsForeign.find(hkb_parent[hkb_obj]->ID) == IsForeign.end())
				{
					pass = true;
				}
			}
			else
			{
				newID = conditionOldFunction(hkb_obj, address, functionlayer, addline, ck);

				if (PerfectMatchCondition.find(hkb_obj->ID) != PerfectMatchCondition.end())
				{
					return hkb_obj->ID;
				}

				if (newID.length() > 0)
				{
					if (special && IsForeign.find(hkb_parent[hkb_obj]->ID) == IsForeign.end())
					{
						PerfectMatchCondition[newID] = true;
						pass = true;
					}
					else if (newID != hkb_obj->ID)
					{
						if (addressChange.find(address) == addressChange.end()) addressChange[address] = address;

						return newID;
					}
				}
			}
		}
		else if (special && hkb_parent.find(hkb_obj) != hkb_parent.end() && IsForeign.find(hkb_parent[hkb_obj]->ID) == IsForeign.end())
		{
			pass = true;
		}

		if (!pass && hkb_obj->getClassCode() == "j")
		{
			if (blendingOldFunction(hkb_obj, address, functionlayer))
			{
				auto& iter = addressChange.find(address);

				if (iter != addressChange.end())
				{
					if (IsForeign.find(hkb_obj->ID) != IsForeign.end()) IsForeign.erase(hkb_obj->ID);

					string oriID = addressID[iter->second];
					addressChange.erase(iter);
					return oriID;
				}
			}
		}

		if (pass || IsOldFunction(("new/" + hkb_obj->ID + ".txt"), hkb_obj, address)) // old function confirmation
		{
			if (IsForeign.find(hkb_obj->ID) != IsForeign.end()) IsForeign.erase(hkb_obj->ID);

			string oriID = addressID[address];

			if (oriID.length() < 3)
			{
				IsForeign[hkb_obj->ID] = true;
				return hkb_obj->ID;
			}

			return oriID;
		}
		else
		{
			IsForeign[hkb_obj->ID] = true;
		}
	}

	return hkb_obj->ID;
}

string conditionOldFunction(shared_ptr<hkbobject> hkb_obj, string address, int functionlayer, string addline, bool isString)
{
	if (PerfectMatchCondition.find(hkb_obj->ID) != PerfectMatchCondition.end())
	{
		return hkb_obj->ID;
	}

	shared_ptr<hkbobject> tempparent = hkb_parent[hkb_obj];
	string oldID;

	if (IsOldFunctionExt("ZeroRegion", hkb_obj, address, true))
	{
		string oriID = addressID[address];
		PerfectMatchCondition[oriID] = true;

		if (IsExist[oriID])
		{
			conditionCheckAndReplace(oriID, hkb_obj);

			if (Error) return "";
		}

		return oriID;
	}

	usize element = 0;
	string flayer = isString ? "ck" + to_string(functionlayer) : "l" + to_string(functionlayer);

	vector<hkbstatemachinetransitioninfoarray::transitioninfo>* transitions = &static_cast<hkbstatemachinetransitioninfoarray*>(tempparent.get())->transitions;

	for (auto& transition : *transitions)
	{
		if (transition.condition) ++element;
	}

	// run through all possible elements
	// as there might be new elements added changing the address
	for (usize i = 0; i < element; i++)
	{
		string tempadd = address;
		tempadd.replace(address.rfind(addline), addline.length(), to_string(i) + flayer);

		if (IsOldFunctionExt("ZeroRegion", hkb_obj, tempadd, true))
		{
			string oriID = addressID[tempadd];
			PerfectMatchCondition[oriID] = true;

			if (IsExist[oriID])
			{
				conditionCheckAndReplace(oriID, hkb_obj);

				if (Error) return "";
			}

			addressChange[address] = tempadd;
			return oriID;
		}
	}

	// capture edited condition without making it seemingly like a newly created 1
	if (IsForeign.find(tempparent->ID) == IsForeign.end())
	{
		string oriID = addressID[address];

		if (PerfectMatchCondition.find(oriID) == PerfectMatchCondition.end() && IsExist.find(oriID) == IsExist.end())
		{
			return oriID;
		}
	}

	return "";
}

void conditionCheckAndReplace(string oriID, shared_ptr<hkbobject> hkb_obj)
{
	auto genref = hkbconditionList_E[oriID];

	if (!genref)
	{
		cout << "BUG DETECTED in CrossReference function (ID: " << hkb_obj->ID << ", supposed ID: " << oriID << ")" << endl;
		Error = true;
		return;
	}

	if (genref->getClassCode() == "l")
	{
		auto ref = hkbexpressionconditionList_E[oriID];
		hkbexpressionconditionList_E.erase(ref->ID);
		hkbconditionList_E.erase(ref->ID);
		editedBehavior.erase(ref->ID);
		ref->ID = ref->previousID;
		hkbexpressionconditionList_E[ref->ID] = ref;
		hkbconditionList_E[ref->ID] = ref;
		editedBehavior[ref->ID] = ref;
	}
	else
	{
		auto ref = hkbstringconditionList_E[oriID];
		hkbstringconditionList_E.erase(ref->ID);
		hkbconditionList_E.erase(ref->ID);
		editedBehavior.erase(ref->ID);
		ref->ID = ref->previousID;
		hkbstringconditionList_E[ref->ID] = ref;
		hkbconditionList_E[ref->ID] = ref;
		editedBehavior[ref->ID] = ref;
	}
}

int FileLineCount(string filename)
{
	int linecount = 0;
	char line[1000];
	FILE* file;
	fopen_s(&file, filename.c_str(), "r");

	if (file)
	{
		while (fgets(line, 1000, file))
		{
			linecount++;
		}

		fclose(file);
	}
	else
	{
		cout << "ERROR: Unable to open file (File: " << filename << ")" << endl;
		Error = true;
	}

	return linecount;
}

void RecordID(string id, string address, bool compare)
{
	while (idlock.test_and_set(boost::memory_order_acquire));
	idcount.insert(stoi(id.substr(1)));
	idlock.clear(boost::memory_order_release);

	auto itr = exchangeID.find(id);

	if (itr != exchangeID.end())
	{
		while (existlock.test_and_set(boost::memory_order_acquire));
		IsExist[itr->second] = true;
		existlock.clear(boost::memory_order_release);
	}
	else
	{
		while (existlock.test_and_set(boost::memory_order_acquire));
		IsExist[id] = true;
		existlock.clear(boost::memory_order_release);
	}

	if (!compare)
	{
		lock_guard<mutex> lock(addresslock);
		addressID.emplace(address, id);
	}
}

bool IsOldRegion(shared_ptr<hkbobject> id, string address, short classcode, bool special)
{
	shared_ptr<hkbobject> tempparent = hkb_parent[id];

	if (!special)
	{
		if (IsOldFunction("ZeroRegion", id, address))
		{
			return true;
		}

		while (IsForeign.find(tempparent->ID) != IsForeign.end())
		{
			tempparent = hkb_parent[tempparent];
		}

		usize numelement;

		switch (classcode)
		{
			case 1:
			{
				bsboneswitchgenerator* ptr = static_cast<bsboneswitchgenerator*>(tempparent.get());
				bsboneswitchgenerator* o_ptr = static_cast<bsboneswitchgenerator*>(originalBehavior.at(tempparent->ID).get());
				numelement = max(ptr->ChildrenA.size(), o_ptr->ChildrenA.size());
				break;
			}
			case 2:
			{
				hkbmanualselectorgenerator* ptr = static_cast<hkbmanualselectorgenerator*>(tempparent.get());
				hkbmanualselectorgenerator* o_ptr = static_cast<hkbmanualselectorgenerator*>(originalBehavior.at(tempparent->ID).get());
				numelement = max(ptr->generators.size(), o_ptr->generators.size());
				break;
			}
			case 3:
			{
				hkbstatemachine* ptr = static_cast<hkbstatemachine*>(tempparent.get());
				hkbstatemachine* o_ptr = static_cast<hkbstatemachine*>(originalBehavior.at(tempparent->ID).get());
				numelement = max(ptr->states.size(), o_ptr->states.size());
				break;
			}
			case 4:
			{
				hkbmodifierlist* ptr = static_cast<hkbmodifierlist*>(tempparent.get());
				hkbmodifierlist* o_ptr = static_cast<hkbmodifierlist*>(originalBehavior.at(tempparent->ID).get());
				numelement = max(ptr->modifiers.size(), o_ptr->modifiers.size());
				break;
			}
		}

		usize tempint = (boost::regex_replace(string(address.substr(address.find_last_of("="))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"))).length();

		for (usize i = 0; i < numelement; i++)
		{
			string tempadd = address;
			tempadd.replace(address.find_last_of("=") + 2, tempint, to_string(i));
			auto add_ptr = addressID.find(tempadd);

			if (add_ptr != addressID.end() && IsExist.find(add_ptr->second) == IsExist.end() && IsOldFunction("ZeroRegion", id, tempadd))
			{
				addressChange[address] = tempadd;
				return true;
			}
		}
	}
	else
	{
		shared_ptr<hkbobject> revertBack;

		while (tempparent->getClassCode() != "h")
		{
			revertBack = tempparent;
			tempparent = hkb_parent[tempparent];

			if (!tempparent)
			{
				cout << "ERROR: Parent backtracking error (Class: " << tempparent->getClassCode() << ")" << endl;
				Error = true;
				throw 5;
			}

			if (IsForeign.find(tempparent->ID) == IsForeign.end())
			{
				if (revertBack->getClassCode() == "e" || revertBack->getClassCode() == "i" || revertBack->getClassCode() == "m" || revertBack->getClassCode() == "y" ||
					revertBack->getClassCode() == "r" || revertBack->getClassCode() == "cj")
				{
					return false;
				}

				string newID = id->ID;

				for (auto& NodeID : exchangeID)
				{
					if (tempparent->ID == NodeID.second)
					{
						newID = NodeID.first;
						break;
					}
				}

				cout << "ERROR: Parent status tracing error (ID: " << newID << ")" << endl;
				Error = true;
				throw 5;
			}
		}

		if (IsForeign.find(tempparent->ID) == IsForeign.end())
		{
			cout << "ERROR: Parent status tracing error (ID: " << tempparent->ID << ")" << endl;
			Error = true;
			throw 5;
		}

		shared_ptr<hkbobject> masterparent = hkb_parent[tempparent];

		if (masterparent->getClassCode() != "cj" && masterparent->getClassCode() != "i")
		{
			cout << "ERROR: Class error in pairing (Class: " << masterparent->getClassCode() << ")" << endl;
			Error = true;
			throw 5;
		}

		if (IsForeign.find(masterparent->ID) != IsForeign.end())
		{
			return false;
		}

		string tempadd = address;
		int nextposition = 0;
		usize position = 0;

		for (int j = 0; j < 2; j++)
		{
			position = tempadd.find(">", nextposition);
			nextposition = tempadd.find(">", position + 1);
		}

		string masteradd = tempadd.substr(0, position + 1);

		if (IsOldFunction("ZeroRegion", id, tempadd))
		{
			auto add_ptr = addressID.find(masteradd);

			if (add_ptr != addressID.end())
			{
				if (IsForeign.find(tempparent->ID) != IsForeign.end() && tempparent->ID != add_ptr->second)
				{
					IsForeign.erase(tempparent->ID);
					editedBehavior.erase(tempparent->ID);
					editedBehavior[add_ptr->second] = tempparent;
					hkbgeneratorList_E[add_ptr->second] = hkbgeneratorList_E[tempparent->ID];
					hkbgeneratorList_E.erase(tempparent->ID);
					tempparent->ID = add_ptr->second;
				}
			}
			else
			{
				cout << "ERROR: Missing function address (ID: " << id << ", address: " << masteradd << ")" << endl;
				Error = true;
				return false;
			}

			return true;
		}

		usize numelement;

		switch (classcode)
		{
			case 1:
			{
				hkbposematchinggenerator* ptr = static_cast<hkbposematchinggenerator*>(masterparent.get());
				hkbposematchinggenerator* o_ptr = static_cast<hkbposematchinggenerator*>(originalBehavior.at(masterparent->ID).get());
				numelement = max(ptr->children.size(), o_ptr->children.size());
				break;
			}
			case 2:
			{
				hkbblendergenerator* ptr = static_cast<hkbblendergenerator*>(masterparent.get());
				hkbblendergenerator* o_ptr = static_cast<hkbblendergenerator*>(originalBehavior.at(masterparent->ID).get());
				numelement = max(ptr->children.size(), o_ptr->children.size());
				break;
			}
		}

		int tempint = (boost::regex_replace(string(address.substr(address.find_last_of("="))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"))).length();
		auto add_ptr = addressID.find(address);

		if (add_ptr != addressID.end() && IsExist.find(add_ptr->second) == IsExist.end())
		{
			for (usize i = 0; i < numelement; i++)
			{
				tempadd.replace(address.find_last_of("=") + 2, tempint, to_string(i));
				add_ptr = addressID.find(tempadd);

				if (add_ptr != addressID.end() && IsExist.find(add_ptr->second) == IsExist.end())
				{
					size_t size = 0;
					position = 0;

					for (int j = 0; j < 2; j++)
					{
						position = tempadd.find(">", size);
						size = tempadd.find(">", position + 1);
					}

					masteradd = tempadd.substr(0, position + 1);

					if (IsOldFunction("ZeroRegion", id, tempadd))
					{
						add_ptr = addressID.find(masteradd);

						if (add_ptr != addressID.end())
						{
							if(IsForeign.find(tempparent->ID) != IsForeign.end()) IsForeign.erase(tempparent->ID);

							editedBehavior.erase(tempparent->ID);
							editedBehavior[add_ptr->second] = tempparent;
							IsExist.erase(tempparent->ID);
							hkbblendergeneratorchildList_E[add_ptr->second] = hkbblendergeneratorchildList_E[tempparent->ID];
							hkbblendergeneratorchildList_E.erase(tempparent->ID);
							tempparent->ID = add_ptr->second;
							IsExist[tempparent->ID] = true;
						}
						else
						{
							cout << "ERROR: Missing function address (ID: " << id << ", address: " << masteradd << ")" << endl;
							Error = true;
						}

						addressChange[address] = tempadd;
						return true;
					}
				}
			}
		}
	}

	return false;
}
