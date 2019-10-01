#include <boost\thread.hpp>
#include "bsboneswitchgenerator.h"
#include "generatorlines.h"

using namespace std;

namespace boneswitchgenerator
{
	string key = "e";
	string classname = "BSBoneSwitchGenerator";
	string signature = "0xf33d3eea";
}

bsboneswitchgenerator::bsboneswitchgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + boneswitchgenerator::key + to_string(functionlayer) + ">region";

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
	else
	{
		return;
	}
}

void bsboneswitchgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSBoneSwitchGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	bool pauseline = false;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (pauseline)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					pauseline = false;
				}

				else if (line.find("#", 0) != string::npos)
				{
					usize tempint = 0;
					usize size = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < size; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempbonedata = line.substr(position, tempint - position - 1);
						bonedata.push_back(tempbonedata);
						parent[tempbonedata] = id;
						referencingIDs[tempbonedata].push_back(id);
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
						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
				{
					generator = line.substr(37, line.find("</hkparam>") - 37);

					if (generator != "null")
					{
						parent[generator] = id;
						referencingIDs[generator].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"ChildrenA\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(42, line.length() - 44));
					bonedata.reserve(children);
					elements[id + "T"] = children;
					pauseline = true;

				}
				else if (line.find("<hkparam name=\"name\">", 0) != string::npos)
				{
					name = line.substr(24, line.find("</hkparam>") - 24);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: BSBoneSwitchGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID
	
	address = name + "(i" + to_string(regioncount[name]) + ")=>";
	regioncount[name]++;
	region[id] = address;

	if ((Debug) && (!Error))
	{
		cout << "BSBoneSwitchGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsboneswitchgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSBoneSwitchGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	bool pauseline = false;
	
	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
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
						string tempbonedata = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempbonedata].empty())
						{
							int tempint = line.find(tempbonedata);
							int templength = tempbonedata.length();
							tempbonedata = exchangeID[tempbonedata];
							line.replace(tempint, templength, tempbonedata);
						}

						bonedata.push_back(tempbonedata);
						parent[tempbonedata] = id;
						referencingIDs[tempbonedata].push_back(id);
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
							int tempint = line.find(variablebindingset);
							variablebindingset = exchangeID[variablebindingset];
							line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
						}

						parent[variablebindingset] = id;
						referencingIDs[variablebindingset].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
				{
					generator = line.substr(37, line.find("</hkparam>") - 37);

					if (!exchangeID[generator].empty())
					{
						int tempint = line.find(generator);
						generator = exchangeID[generator];
						line.replace(tempint, line.find("</hkparam>") - tempint, generator);
					}

					if (generator != "null")
					{
						parent[generator] = id;
						referencingIDs[generator].push_back(id);
					}
				}
				else if (line.find("<hkparam name=\"ChildrenA\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(42, line.length() - 44));
					bonedata.reserve(children);
					pauseline = true;
				}
			}
			
			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSBoneSwitchGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSBoneSwitchGenerator(newID: " << id << ") with BSBoneSwitchGenerator(oldID: " << tempid << ")" << endl;
		}

		// replace referencing IDs
		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < bonedata.size(); i++)
		{
			referencingIDs[bonedata[i]].pop_back();
			referencingIDs[bonedata[i]].push_back(tempid);
			parent[bonedata[i]] = tempid;
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
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
				if ((newline[i].find("numelements", 0) != string::npos) && (newline[i].find(storeline[curline], 0) == string::npos))
				{
					IsNewChild = true;
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
			cout << "Comparing BSBoneSwitchGenerator(newID: " << id << ") with BSBoneSwitchGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSBoneSwitchGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsboneswitchgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSBoneSwitchGenerator(ID: " << id << ") has been initialized!" << endl;
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
						string tempbonedata = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempbonedata].empty())
						{
							tempbonedata = exchangeID[tempbonedata];
						}

						bonedata.push_back(tempbonedata);
						parent[tempbonedata] = id;
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
				else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
				{
					generator = line.substr(37, line.find("</hkparam>") - 37);

					if (!exchangeID[generator].empty())
					{
						generator = exchangeID[generator];
					}

					if (generator != "null")
					{
						parent[generator] = id;
					}
				}
				else if (line.find("<hkparam name=\"ChildrenA\" numelements=", 0) != string::npos)
				{
					children = stoi(line.substr(42, line.length() - 44));
					bonedata.reserve(children);
					pauseline = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSBoneSwitchGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (!region[id].empty())
	{
		address = region[id];
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSBoneSwitchGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bsboneswitchgenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string bsboneswitchgenerator::GetBoneData(int child)
{
	return "#" + boost::regex_replace(string(bonedata[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int bsboneswitchgenerator::GetChildren()
{
	return children;
}

string bsboneswitchgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsboneswitchgenerator::IsBindingNull()
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

string bsboneswitchgenerator::GetAddress()
{
	return address;
}

bool bsboneswitchgenerator::IsNegate()
{
	return IsNegated;
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

	paramMatch("pDefaultGenerator", pDefaultGenerator, ctrpart->pDefaultGenerator, output, storeline, base, true, open, isEdited);
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

	output.push_back(autoParam(base, "pDefaultGenerator", pDefaultGenerator));
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

void BSBoneSwitchGeneratorExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	vecstr storeline2;

	if (!generatorLines(storeline1, storeline2, id, "BSBoneSwitchGenerator"))
	{
		return;
	}

	vecstr output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (i < storeline1.size()) // existing variable value
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
			{
				if (open)
				{
					if (IsChanged)
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

						IsChanged = false;
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
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}

			output.push_back(storeline2[i]);
			curline++;
		}
		else // new data
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = curline;
				IsEdited = true;
				open = true;
			}

			output.push_back(storeline2[i]);
		}
	}

	if (open) // close unclosed edits
	{
		if (IsChanged)
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

			IsChanged = false;
		}

		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 2 output if it is edited
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

			fwrite << "			</hkparam>" << "\n";
			fwrite << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit BSBoneSwitchGenerator Output Not Found (File: " << filename << ")" << endl;
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