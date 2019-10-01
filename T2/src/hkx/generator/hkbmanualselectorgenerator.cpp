#include <boost\thread.hpp>
#include "hkbmanualselectorgenerator.h"
#include "highestscore.h"

using namespace std;

namespace manualselectorgenerator
{
	string key = "m";
	string classname = "hkbManualSelectorGenerator";
	string signature = "0xd932fab8";
}

hkbmanualselectorgenerator::hkbmanualselectorgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + manualselectorgenerator::key + to_string(functionlayer) + ">region";

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
		string dummyID = CrossReferencing(id, address, functionlayer, compare);

		if (compare)
		{
			Dummy(dummyID);

			if (IsForeign[id])
			{
				if (!region[id].empty())
				{
					address = region[id];
				}
				else
				{
					address = preaddress;
				}
			}
		}
		else
		{
			IsNegated = true;
			address = region[id];
		}
	}
}

void hkbmanualselectorgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	unordered_map<string, bool> IsReferenceExist;
	string line;
	bool pauseline = false;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (pauseline)
			{
				if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);
						generator.push_back(tempgenerator);

						if (!IsReferenceExist[tempgenerator])
						{
							referencingIDs[tempgenerator].push_back(id);
							IsReferenceExist[tempgenerator] = true;
						}

						parent[tempgenerator] = id;
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;
				}
				else if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

					if (variablebindingset != "null")
					{
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"name\">", 0) != string::npos)
				{
					s_name = line.substr(24, line.find("</hkparam>") - 24);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	address = s_name + "(m" + to_string(regioncount[s_name]) + ")=>";
	regioncount[s_name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	unordered_map<string, bool> IsReferenceExist;
	vecstr newline;
	string line;
	bool pauseline = false;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; i++)
		{
			line = FunctionLineEdited[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					int curgen = 1;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = 0;
						usize tempint = 0;

						for (int j = 0; j < curgen; j++)
						{
							position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
						}

						curgen++;
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							int tempint = line.find(tempgenerator);
							int templength = tempgenerator.length();
							tempgenerator = exchangeID[tempgenerator];
							line.replace(tempint, templength, tempgenerator);
						}

						parent[tempgenerator] = id;
						generator.push_back(tempgenerator);

						if (!IsReferenceExist[tempgenerator])
						{
							referencingIDs[tempgenerator].push_back(id);
							IsReferenceExist[tempgenerator] = true;
						}
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
				else if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

					if (variablebindingset != "null")
					{
						if (!exchangeID[variablebindingset].empty())
						{
							int tempint = line.find(variablebindingset);
							variablebindingset = exchangeID[variablebindingset];
							line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
						}

						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbManualSelectorGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}
	
	// stage 2
	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		elements[tempid] = children;
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbManualSelectorGenerator(newID: " << id << ") with hkbManualSelectorGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < generator.size(); i++)
		{
			referencingIDs[generator[i]].pop_back();
			referencingIDs[generator[i]].push_back(tempid);
			parent[generator[i]] = tempid;
		}

		vecstr storeline = FunctionLineTemp[tempid];

		// stage 3
		int curline = 1;
		bool IsNewChild = false;
		vecstr newstoreline;
		vecstr newchild;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
			if (!IsNewChild)
			{
				if ((newline[i].find("<hkparam name=\"generators\" numelements=", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					int numelement1 = stoi(boost::regex_replace(string(newline[i]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));
					int numelement2 = stoi(boost::regex_replace(string(storeline[curline]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));

					if (numelement1 != numelement2)
					{
						IsNewChild = true;
					}
				}

				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				if ((storeline[curline].find("</hkparam>", 0) != string::npos) && (storeline[curline].length() < 15))
				{
					if ((newline[i].find("</hkparam>", 0) != string::npos) && (newline[i].length() < 15))
					{
						for (unsigned int j = 0; j < newchild.size(); j++)
						{
							newstoreline.push_back(newchild[j]);
						}

						newstoreline.push_back(newline[i]);
						curline++;
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}
				}
				else if (storeline[curline].find(newline[i], 0) == string::npos)
				{
					usize size = count(storeline[curline].begin(), storeline[curline].end(), '#');
					usize size2 = count(newline[i].begin(), newline[i].end(), '#');

					if (size < size2)
					{
						usize position = 0;
						usize tempint = 0;

						for (unsigned int j = 0; j < size + 1; j++)
						{
							position = newline[i].find("#", tempint);
							tempint = newline[i].find("#", position + 1);
						}

						newstoreline.push_back(newline[i].substr(0, position - 1));
						newchild.push_back("				" + newline[i].substr(position, -1));
					}
					else
					{
						newstoreline.push_back(newline[i]);
					}

					curline++;
				}
				else
				{
					newstoreline.push_back(newline[i]);
					curline++;
				}
			}
		}

		FunctionLineNew[tempid] = newstoreline;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbManualSelectorGenerator(newID: " << id << ") with hkbManualSelectorGenerator(oldID: " << tempid << ") is complete!" << endl;
		}

		address = region[tempid];
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbManualSelectorGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	bool pauseline = false;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					break;
				}
				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempgenerator = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempgenerator].empty())
						{
							tempgenerator = exchangeID[tempgenerator];
						}

						parent[tempgenerator] = id;
						generator.push_back(tempgenerator);
					}
				}
			}

			else
			{
				if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
				{
					variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

					if (variablebindingset != "null")
					{
						if (!exchangeID[variablebindingset].empty())
						{
							variablebindingset = exchangeID[variablebindingset];
						}

						parent[variablebindingset] = id;
					}
				}

				else if (line.find("<hkparam name=\"generators\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(43, line.length() - 45));
					generator.reserve(children);
					pauseline = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbManualSelectorGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbManualSelectorGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string hkbmanualselectorgenerator::NextGenerator(int child)
{
	return "#" + boost::regex_replace(string(generator[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int hkbmanualselectorgenerator::GetChildren()
{
	return children;
}

string hkbmanualselectorgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbmanualselectorgenerator::IsBindingNull()
{
	if (variablebindingset.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbmanualselectorgenerator::GetClass(string id, string inputfile)
{
	string line;
	char charline[2000];
	string classname = "<hkobject name=\"" + id;
	FILE* input;
	fopen_s(&input, inputfile.c_str(), "r");

	if (input)
	{
		while (fgets(charline, 2000, input))
		{
			if (charline[strlen(charline) - 1] == '\n')
			{
				charline[strlen(charline) - 1] = '\0';
			}

			line = charline;

			if (line.find(classname, 0) != string::npos)
			{
				int tempint = line.find("class") + 7;
				return line.substr(tempint, line.find("signature") - tempint - 2);
			}
		}

		fclose(input);
	}
	else
	{
		cout << "ERROR: GetClass Inputfile(" << id << ", " << inputfile << ")" << endl;
	}

	cout << "ERROR: GetClass(" << id << ", " << inputfile << ")" << endl;
	Error = true;

	return "null";
}

string hkbmanualselectorgenerator::GetAddress()
{
	return address;
}

bool hkbmanualselectorgenerator::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbmanualselectorgenerator>> hkbmanualselectorgeneratorList;
safeStringUMap<shared_ptr<hkbmanualselectorgenerator>> hkbmanualselectorgeneratorList_E;

void hkbmanualselectorgenerator::regis(string id, bool isEdited)
{
	isEdited ? hkbmanualselectorgeneratorList_E[id] = shared_from_this() : hkbmanualselectorgeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbmanualselectorgenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos || line.find("	#") != string::npos)
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
					usize numelement;

					if (readEleParam("states", line, numelement))
					{
						generators.reserve(numelement);
						++type;
					}

					break;
				}
				case 4:
				{
					vecstr reflist;

					if (!getNodeRefList(line, reflist))
					{
						if (readParam("selectedGeneratorIndex", line, selectedGeneratorIndex)) ++type;

						break;
					}

					for (auto& ref : reflist)
					{
						generators.emplace_back(isEdited ? hkbgeneratorList_E[ref] : hkbgeneratorList[ref]);
					}

					break;
				}
				case 5:
				{
					if (readParam("currentGeneratorIndex", line, currentGeneratorIndex)) ++type;
				}
			}
		}
		if (type == 0)
		{
			if (line.find("<hkparam name=\"") != string::npos)
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
					itr != boost::sregex_iterator(); ++itr)
				{
					string header = itr->str(1);

					if (header == "variableBindingSet") variableBindingSet = isEdited ? hkbvariablebindingsetList_E[itr->str(2)] : hkbvariablebindingsetList[itr->str(2)];
					else if (header == "userData") userData = stoi(itr->str(2));
					else if (header == "name")
					{
						++type;
						name = itr->str(2);
					}

					break;
				}
			}
		}
		else if (type == 1)
		{
			if (line.find("<hkparam name=\"generators\" numelements=\"") != string::npos)
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"generators\" numelements=\"([0-9]+)\">(<\\/hkparam>|)"));
					itr != boost::sregex_iterator(); ++itr)
				{
					if (itr->str(2).length() == 0) generators.reserve(stoi(itr->str(1)));

					++type;
					break;
				}
			}
		}
		else
		{
			if (line.find("<hkparam name=\"") != string::npos)
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
					itr != boost::sregex_iterator(); ++itr)
				{
					string header = itr->str(1);

					if (header == "selectedGeneratorIndex") selectedGeneratorIndex = stoi(itr->str(2));
					else if (header == "currentGeneratorIndex") currentGeneratorIndex = stoi(itr->str(2));

					break;
				}
			}
			else
			{
				for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("(?<!>)(#[0-9]+)")); itr != boost::sregex_iterator(); ++itr)
				{
					generators.emplace_back(isEdited ? hkbgeneratorList_E[itr->str(1)] : hkbgeneratorList[itr->str(1)]);
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << manualselectorgenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbmanualselectorgenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + manualselectorgenerator::key + to_string(functionlayer) + ">region";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			functionlayer = 0;
			RecordID(ID, address);

			while (regioncountlock.test_and_set(boost::memory_order_acquire));
			address = name + "(" + manualselectorgenerator::key + to_string(regioncount[name]) + ")=>";
			regioncount[name]++;
			regioncountlock.clear(boost::memory_order_release);
			region[ID] = address;
			boost::thread_group multithreads;
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (num_thread < boost::thread::hardware_concurrency())
			{
				for (usize i = 0; i < generators.size(); ++i)
				{
					multithreads.create_thread(boost::bind(&hkbmanualselectorgenerator::threadedNextNode, this, generators[i], filepath, curadd + to_string(i), functionlayer, graphroot));
				}

				num_thread += multithreads.size();
			}
			else
			{
				for (usize i = 0; i < generators.size(); ++i)
				{
					threadedNextNode(generators[i], filepath, curadd + to_string(i), functionlayer, graphroot);
				}
			}

			if (multithreads.size() > 0)
			{
				multithreads.join_all();
				num_thread -= multithreads.size();
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
				hkbmanualselectorgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];

				hkbmanualselectorgeneratorList_E[ID] = protect;
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
				hkbmanualselectorgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbmanualselectorgeneratorList_E[ID] = protect;
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

string hkbmanualselectorgenerator::getClassCode()
{
	return manualselectorgenerator::key;
}

void hkbmanualselectorgenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(100);
	usize base = 2;
	hkbmanualselectorgenerator* ctrpart = static_cast<hkbmanualselectorgenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, manualselectorgenerator::classname, manualselectorgenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);

	usize size = generators.size();
	usize orisize = size;

	if (size == 0)
	{
		output.push_back(openParam(base, "generators", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "generators", size));		// 2

	matchScoring(generators, ctrpart->generators, ID);
	size = generators.size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if (generators[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				string line;

				do
				{
					line.append(ctrpart->generators[i]->ID + " ");
					++i;
				} while (i < size && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
		// deleted existing data
		else if (ctrpart->generators[i]->ID.length() == 0)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (ctrpart->generators[i]->ID.length() != 0)
				{
					--i;
					break;
				}

				storeline.push_back(autoVoid(base, generators[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(generators[i], ctrpart->generators[i], output, storeline, base, false, open, isEdited);
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

	paramMatch("selectedGeneratorIndex", selectedGeneratorIndex, ctrpart->selectedGeneratorIndex, output, storeline, base, false, open, isEdited);
	paramMatch("currentGeneratorIndex", currentGeneratorIndex, ctrpart->currentGeneratorIndex, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", manualselectorgenerator::classname, output, isEdited);
}

void hkbmanualselectorgenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, manualselectorgenerator::classname, manualselectorgenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));

	usize size = generators.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "generators", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "generators", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(generators[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(autoParam(base, "selectedGeneratorIndex", selectedGeneratorIndex));
	output.push_back(autoParam(base, "currentGeneratorIndex", currentGeneratorIndex));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, manualselectorgenerator::classname, output, true);
}

void hkbmanualselectorgenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();

	for (auto& generator : generators)
	{
		hkb_parent[generator] = shared_from_this();
	}
}

void hkbmanualselectorgenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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

		for (auto& generator : generators)
		{
			parentRefresh();
			generator->connect(filepath, address + to_string(tempint), 0, true, graphroot);

			if (IsForeign.find(generator->ID) == IsForeign.end()) ++tempint;
			else
			{
				auto branch_itr = IsBranchOrigin.find(generator->ID);

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
		for (auto& generator : generators)
		{
			parentRefresh();
			generator->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbmanualselectorgenerator::matchScoring(vector<shared_ptr<hkbgenerator>>& ori, vector<shared_ptr<hkbgenerator>>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<shared_ptr<hkbgenerator>> newOri;
		vector<shared_ptr<hkbgenerator>> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(make_shared<hkbgenerator>());
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

			if (i == j)
			{
				scorelist[i][j] += 5;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 5;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<shared_ptr<hkbgenerator>> newOri;
	vector<shared_ptr<hkbgenerator>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<hkbgenerator>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<hkbgenerator>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbmanualselectorgenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbManualSelectorGeneratorExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		bool start = false;

		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if (start)
			{
				if (line.find("</hkparam>") != string::npos)
				{
					start = false;
				}
				else
				{
					stringstream sstream(line);
					istream_iterator<string> ssbegin(sstream);
					istream_iterator<string> ssend;
					vecstr curElements(ssbegin, ssend);
					copy(curElements.begin(), curElements.end(), curElements.begin());

					for (auto& element : curElements)
					{
						storeline1.push_back("				" + element);
					}
				}
			}
			else if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}

			if (line.find("numelements=\"") != string::npos)
			{
				start = true;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbManualSelectorGenerator Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		bool start = false;

		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (start)
			{
				if (line.find("</hkparam>") != string::npos)
				{
					start = false;
				}
				else
				{
					stringstream sstream(line);
					istream_iterator<string> ssbegin(sstream);
					istream_iterator<string> ssend;
					vecstr curElements(ssbegin, ssend);
					copy(curElements.begin(), curElements.end(), curElements.begin());

					for (auto& element : curElements)
					{
						storeline2.push_back("				" + element);
					}
				}
			}
			else if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}

			if (line.find("numelements=\"") != string::npos)
			{
				start = true;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbManualSelectorGenerator Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vecstr output;
	vecstr oriline;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	bool IsOpenOut = false;
	int curline = 0;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (storeline1[curline].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos) // existing variable value
		{
			if (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (storeline1[curline].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos)
				{
					output.push_back("");
					oriline.push_back(storeline1[curline]);
					++curline;
				}
				
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), oriline.begin(), oriline.end());
				output.push_back("<!-- CLOSE -->");
				oriline.clear();
				--i;
				open = false;
			}
			else
			{
				if (storeline1[curline] == storeline2[i])
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), oriline.begin(), oriline.end());
						output.push_back("<!-- CLOSE -->");
						oriline.clear();
						open = false;
					}

					output.push_back(storeline1[curline]);
				}
				else
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
					oriline.push_back(storeline1[curline]);
				}

				curline++;
			}
		}
		else
		{
			if (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) != string::npos)
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), oriline.begin(), oriline.end());
					output.push_back("<!-- CLOSE -->");
					oriline.clear();
					open = false;
				}

				output.push_back("			</hkparam>");

				if (storeline1[curline] == storeline2[i])
				{
					output.push_back(storeline1[curline]);
				}
				else
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back(storeline2[i]);
					oriline.push_back(storeline1[curline]);
					IsEdited = true;
					open = true;
				}

				curline++;
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (storeline2[i].find("<hkparam name=\"selectedGeneratorIndex\">", 0) == string::npos)
				{
					output.push_back(storeline2[i]);
					++i;
				}

				if(oriline.size() > 0)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), oriline.begin(), oriline.end());
					oriline.clear();
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
				--i;
			}
		}
	}

	if (open) // close unclosed edits
	{
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), oriline.begin(), oriline.end());
		output.push_back("<!-- CLOSE -->");
		oriline.clear();
		open = false;
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

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit hkbManualSelectorGenerator Output Not Found (File: " << filename << ")" << endl;
			Error = true;
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
