#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>
#include "hkbbehaviorgraphdata.h"
#include "highestscore.h"
#include "src\utilities\stringdatalock.h"

using namespace std;
vector<usize> datapacktracker;

namespace behaviorgraphdata
{
	string key = "f";
	string classname = "hkbBehaviorGraphData";
	string signature = "0x95aca5d";
}

namespace graphdata
{
	struct variableinfo
	{
		string role;
		string flags;
		string type;
	};

	struct characterpropertyinfo
	{
		string role;
		string flags;
		string type;
	};

	struct eventinfo
	{
		string flags;
	};

	struct graphdata
	{
		string attributeDefaults;
		vector<variableinfo> variableInfos;
		vector<characterpropertyinfo> characterPropertyInfos;
		vector<eventinfo> eventInfos;
		string wordMinVariableValues;
		string wordMaxVariableValues;
		string variableInitialValues;
		string stringData;
	};
}

void inputEventInfos(vecstr& input, graphdata::eventinfo info);
void inputCharacterPropertyInfos(vecstr& input, graphdata::characterpropertyinfo info);
void inputVariableInfo(vecstr& input, graphdata::variableinfo info);
void dataInfoProcess(string line, graphdata::graphdata& graphData, string& current, string id);

hkbbehaviorgraphdata::hkbbehaviorgraphdata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + behaviorgraphdata::key + to_string(functionlayer) + ">";

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

void hkbbehaviorgraphdata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableInitialValues\">", 0) != string::npos)
			{
				variableinitialvalues = line.substr(41, line.find("</hkparam>") - 41);
				referencingIDs[variableinitialvalues].push_back(id);
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				stringdata = line.substr(30, line.find("</hkparam>") - 30);
				referencingIDs[stringdata].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraphData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphdata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraphData(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"variableInitialValues\">", 0) != string::npos)
			{
				variableinitialvalues = line.substr(41, line.find("</hkparam>") - 41);

				if (!exchangeID[variableinitialvalues].empty())
				{
					int tempint = line.find(variableinitialvalues);
					variableinitialvalues = exchangeID[variableinitialvalues];
					line.replace(tempint, line.find("</hkparam>") - tempint, variableinitialvalues);
				}

				parent[variableinitialvalues] = id;
				referencingIDs[variableinitialvalues].push_back(id);
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				stringdata = line.substr(30, line.find("</hkparam>") - 30);

				if (!exchangeID[stringdata].empty())
				{
					int tempint = line.find(stringdata);
					stringdata = exchangeID[stringdata];
					line.replace(tempint, line.find("</hkparam>") - tempint, stringdata);
				}

				parent[stringdata] = id;
				referencingIDs[stringdata].push_back(id);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraphData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (addressID[address] != "") // is this new function or old for non generator
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
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraphData(newID: " << id << ") with hkbBehaviorGraphData(oldID: " << tempid << ")" << endl;
		}

		referencingIDs[variableinitialvalues].pop_back();
		referencingIDs[variableinitialvalues].push_back(tempid);
		parent[variableinitialvalues] = tempid;

		referencingIDs[stringdata].pop_back();
		referencingIDs[stringdata].push_back(tempid);
		parent[stringdata] = tempid;

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBehaviorGraphData(newID: " << id << ") with hkbBehaviorGraphData(oldID: " << tempid << ") is complete!" << endl;
		}
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
		cout << "hkbBehaviorGraphData(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphdata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBehaviorGraphData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"variableInitialValues\">", 0) != string::npos)
			{
				variableinitialvalues = line.substr(41, line.find("</hkparam>") - 41);

				if (!exchangeID[variableinitialvalues].empty())
				{
					variableinitialvalues = exchangeID[variableinitialvalues];
				}

				parent[variableinitialvalues] = id;
			}
			else if (line.find("<hkparam name=\"stringData\">", 0) != string::npos)
			{
				stringdata = line.substr(30, line.find("</hkparam>") - 30);

				if (!exchangeID[stringdata].empty())
				{
					stringdata = exchangeID[stringdata];
				}

				parent[stringdata] = id;
				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbBehaviorGraphData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBehaviorGraphData(ID: " << id << ") is complete!" << endl;
	}
}

string hkbbehaviorgraphdata::GetAddress()
{
	return address;
}

string hkbbehaviorgraphdata::GetVariableInitialValues()
{
	return "#" + boost::regex_replace(string(variableinitialvalues), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbbehaviorgraphdata::GetStringData()
{
	return "#" + boost::regex_replace(string(stringdata), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbbehaviorgraphdata::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbbehaviorgraphdata>> hkbbehaviorgraphdataList;
safeStringUMap<shared_ptr<hkbbehaviorgraphdata>> hkbbehaviorgraphdataList_E;

void hkbbehaviorgraphdata::regis(string id, bool isEdited)
{
	isEdited ? hkbbehaviorgraphdataList_E[id] = shared_from_this() : hkbbehaviorgraphdataList[id] = shared_from_this();
	ID = id;
}

void hkbbehaviorgraphdata::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	usize counter = 0;
	shared_ptr<variableinfopack> curVarInfo = variableInfos;;
	vector<hkbvariablevalue>* curVarValues = &wordMinVariableValues;

	for (auto& line : nodelines)
	{
		usize pos = line.find("<hkparam name=\"stringData\">");

		if (pos != string::npos)
		{
			pos += 27;
			string stringID = line.substr(pos, line.find("</hkparam>", pos) - pos);

			if (stringID != "null")
			{
				stringData = (isEdited ? hkbbehaviorgraphstringdataList_E : hkbbehaviorgraphstringdataList)[stringID];
				attributeDefaults = stringData->attributeNames;
				variableInfos = stringData->variableNames;
				characterPropertyInfos = stringData->characterPropertyNames;
				eventInfos = stringData->eventNames;
			}
		}
	}

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("attributeDefaults", line, numelement))
				{
					++type;
				}

				break;
			}
			case 1:
			{
				usize numelement;

				if (readEleParam("variableInfos", line, numelement))
				{
					counter = 0;
					curVarInfo = variableInfos;
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

					(*attributeDefaults)[counter].value = stoi(each);
					++counter;
				}
			}
			case 2:
			{
				string output;

				if (readParam("role", line, output))
				{
					(*curVarInfo)[counter].role.installRole(output);
					++type;
					break;
				}

				usize numelement;

				if (readEleParam("characterPropertyInfos", line, numelement))
				{
					counter = 0;
					curVarInfo = characterPropertyInfos;
					break;
				}

				if (readEleParam("eventInfos", line, numelement))
				{
					counter = 0;
					type = 5;
				}

				break;
			}
			case 3:
			{
				string output;

				if (readParam("flags", line, output))
				{
					vecstr list;
					boost::trim_if(output, boost::is_any_of("\t "));
					boost::split(list, output, boost::is_any_of("|"), boost::token_compress_on);

					for (auto& each : list)
					{
						(*curVarInfo)[counter].role.flags.update(each);
					}

					++type;
				}

				break;
			}
			case 4:
			{
				string output;

				if (readParam("type", line, output))
				{
					(*curVarInfo)[counter].typeUpdate(output);
					++counter;
					type = 2;
				}

				break;
			}
			case 5:
			{
				string output;

				if (readParam("flags", line, output))
				{
					vecstr list;
					boost::trim_if(output, boost::is_any_of("\t "));
					boost::split(list, output, boost::is_any_of("|"), boost::token_compress_on);

					for (auto& each : list)
					{
						(*eventInfos)[counter].flags.update(each);
					}

					++counter;
					break;
				}

				usize numelement;

				if (readEleParam("wordMinVariableValues", line, numelement))
				{
					wordMinVariableValues.reserve(numelement);
					++type;
				}

				break;
			}
			case 6:
			{
				int output;

				if (readParam("value", line, output))
				{
					curVarValues->push_back(output);
					break;
				}

				usize numelement;

				if (readEleParam("wordMaxVariableValues", line, numelement))
				{
					wordMaxVariableValues.reserve(numelement);
					curVarValues = &wordMaxVariableValues;
					break;
				}

				string s_output;

				if (readParam("variableInitialValues", line, s_output))
				{
					variableInitialValues = (isEdited ? hkbvariablevaluesetList_E : hkbvariablevaluesetList)[s_output];
					++type;
				}

				break;
			}
			case 7:
			{
				string output;

				if (readParam("stringData", line, output))
				{
					stringData = (isEdited ? hkbbehaviorgraphstringdataList_E : hkbbehaviorgraphstringdataList)[output];
					++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << behaviorgraphdata::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbbehaviorgraphdata::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + behaviorgraphdata::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableInitialValues) threadedNextNode(variableInitialValues, filepath, curadd, functionlayer, graphroot);

			if (stringData) threadedNextNode(stringData, filepath, curadd, functionlayer, graphroot);
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
				hkbbehaviorgraphdataList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbbehaviorgraphdataList_E[ID] = protect;
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
				hkbbehaviorgraphdataList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbbehaviorgraphdataList_E[ID] = protect;
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

string hkbbehaviorgraphdata::getClassCode()
{
	return behaviorgraphdata::key;
}

void hkbbehaviorgraphdata::match(shared_ptr<hkbobject> counterpart)
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
	hkbbehaviorgraphdata* ctrpart = static_cast<hkbbehaviorgraphdata*>(counterpart.get());

	output.push_back(openObject(base, ID, behaviorgraphdata::classname, behaviorgraphdata::signature));	// 1

	// attribute default
	if (attributeDefaults)
	{
		usize size = attributeDefaults->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "attributeDefaults", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "attributeDefaults", size));		// 2

		size = attributeDefaults->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*attributeDefaults)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(autoVoid(base, (*ctrpart->attributeDefaults)[i].value));
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->attributeDefaults)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->attributeDefaults)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(autoVoid(base, (*attributeDefaults)[i].value));
					++i;
					output.push_back("");	// 1 spaces
				}
			}
			// both exist
			else
			{
				voidMatch((*attributeDefaults)[i].value, (*ctrpart->attributeDefaults)[i].value, output, storeline, base, false, open, isEdited);
			}
		}

		if (size > 0)
		{
			if (attributeDefaults->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "attributeDefaults", 0) + closeParam());		// 2
		--base;
	}

	// variable info
	if (variableInfos)
	{
		usize size = variableInfos->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "variableInfos", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "variableInfos", size));		// 2

		size = variableInfos->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*variableInfos)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(openObject(base));				// 3
					output.push_back(openParam(base, "role"));		// 4
					output.push_back(openObject(base));				// 5
					output.push_back(autoParam(base, "role", (*ctrpart->variableInfos)[i].role.getRole()));
					output.push_back(autoParam(base, "flags", (*ctrpart->variableInfos)[i].role.flags.getflags()));
					output.push_back(closeObject(base));			// 5
					output.push_back(closeParam(base));				// 4
					output.push_back(autoParam(base, "type", (*ctrpart->variableInfos)[i].getType()));
					output.push_back(closeObject(base));			// 3
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->variableInfos)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->variableInfos)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(openObject(base));				// 3
					storeline.push_back(openParam(base, "role"));		// 4
					storeline.push_back(openObject(base));				// 5
					storeline.push_back(autoParam(base, "role", (*variableInfos)[i].role.getRole()));
					storeline.push_back(autoParam(base, "flags", (*variableInfos)[i].role.flags.getflags()));
					storeline.push_back(closeObject(base));				// 5
					storeline.push_back(closeParam(base));				// 4
					storeline.push_back(autoParam(base, "type", (*variableInfos)[i].getType()));
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
				paramMatch("role", (*variableInfos)[i].role.getRole(), (*ctrpart->variableInfos)[i].role.getRole(), output, storeline, base, false, open, isEdited);
				paramMatch("flags", (*variableInfos)[i].role.flags.getflags(), (*ctrpart->variableInfos)[i].role.flags.getflags(), output, storeline, base, true, open, isEdited);
				output.push_back(closeObject(base));			// 5
				output.push_back(closeParam(base));				// 4
				paramMatch("type", (*variableInfos)[i].getType(), (*ctrpart->variableInfos)[i].getType(), output, storeline, base, true, open, isEdited);
				output.push_back(closeObject(base));			// 3
			}
		}

		if (size > 0)
		{
			if (variableInfos->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "variableInfos", 0) + closeParam());		// 2
		--base;
	}

	// character property info
	if (characterPropertyInfos)
	{
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
				paramMatch("role", (*characterPropertyInfos)[i].role.getRole(), (*ctrpart->characterPropertyInfos)[i].role.getRole(), output, storeline, base, false, open, isEdited);
				paramMatch("flags", (*characterPropertyInfos)[i].role.flags.getflags(), (*ctrpart->characterPropertyInfos)[i].role.flags.getflags(), output, storeline, base, true,
					open, isEdited, false, true);
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
	}
	else
	{
		output.push_back(openParam(base, "characterPropertyInfos", 0) + closeParam());		// 2
		--base;
	}

	// event info
	if (eventInfos)
	{
		usize size = eventInfos->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "eventInfos", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "eventInfos", size));		// 2

		size = eventInfos->size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if ((*eventInfos)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(openObject(base));				// 3
					output.push_back(autoParam(base, "flags", (*ctrpart->eventInfos)[i].flags.getflags()));
					output.push_back(closeObject(base));			// 3
					++i;
				}
			}
			// deleted existing data
			else if ((*ctrpart->eventInfos)[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!(*ctrpart->eventInfos)[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(openObject(base));				// 3
					storeline.push_back(autoParam(base, "flags", (*eventInfos)[i].flags.getflags()));
					storeline.push_back(closeObject(base));				// 3
					++i;
					output.push_back("");
					output.push_back("");
					output.push_back("");	// 3 spaces
				}
			}
			// both exist
			else
			{
				nemesis::try_close(open, output, storeline);
				output.push_back(openObject(base));				// 3
				paramMatch("flags", (*eventInfos)[i].flags.getflags(), (*ctrpart->eventInfos)[i].flags.getflags(), output, storeline, base, true, open, isEdited, false, true);
				output.push_back(closeObject(base));			// 3
			}
		}

		if (size > 0)
		{
			if (eventInfos->d_size() == 0)
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
	}
	else
	{
		output.push_back(openParam(base, "eventInfos", 0) + closeParam());		// 2
		--base;
	}

	// word min variable values
	{
		usize size = wordMinVariableValues.size();

		if (size == 0)
		{
			output.push_back(openParam(base, "wordMinVariableValues", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "wordMinVariableValues", size));		// 2

		matchScoring(wordMinVariableValues, ctrpart->wordMinVariableValues, ID);
		size = wordMinVariableValues.size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if (wordMinVariableValues[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(openObject(base));				// 3
					output.push_back(autoParam(base, "value", ctrpart->wordMinVariableValues[i].value));
					output.push_back(closeObject(base));			// 3
					++i;
				}
			}
			// deleted existing data
			else if (ctrpart->wordMinVariableValues[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!ctrpart->wordMinVariableValues[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(openObject(base));				// 3
					storeline.push_back(autoParam(base, "value", wordMinVariableValues[i].value));
					storeline.push_back(closeObject(base));				// 3
					++i;
					output.push_back("");
					output.push_back("");
					output.push_back("");	// 3 spaces
				}
			}
			// both exist
			else
			{
				nemesis::try_close(open, output, storeline);
				output.push_back(openObject(base));				// 3
				paramMatch("value", wordMinVariableValues[i].value, ctrpart->wordMinVariableValues[i].value, output, storeline, base, true, open, isEdited);
				output.push_back(closeObject(base));			// 3
			}
		}

		if (size > 0)
		{
			if (eventInfos->d_size() == 0)
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
	}

	// word max variable values
	{
		usize size = wordMaxVariableValues.size();

		if (size == 0)
		{
			output.push_back(openParam(base, "wordMaxVariableValues", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "wordMaxVariableValues", size));		// 2

		matchScoring(wordMaxVariableValues, ctrpart->wordMaxVariableValues, ID);
		size = wordMaxVariableValues.size();

		for (usize i = 0; i < size; ++i)
		{
			// newly created data
			if (wordMaxVariableValues[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					output.push_back(openObject(base));				// 3
					output.push_back(autoParam(base, "value", ctrpart->wordMaxVariableValues[i].value));
					output.push_back(closeObject(base));			// 3
					++i;
				}
			}
			// deleted existing data
			else if (ctrpart->wordMaxVariableValues[i].proxy)
			{
				nemesis::try_open(open, isEdited, output);

				while (i < size)
				{
					if (!ctrpart->wordMaxVariableValues[i].proxy)
					{
						--i;
						break;
					}

					storeline.push_back(openObject(base));				// 3
					storeline.push_back(autoParam(base, "value", wordMaxVariableValues[i].value));
					storeline.push_back(closeObject(base));				// 3
					++i;
					output.push_back("");
					output.push_back("");
					output.push_back("");	// 3 spaces
				}
			}
			// both exist
			else
			{
				nemesis::try_close(open, output, storeline);
				output.push_back(openObject(base));				// 3
				paramMatch("value", wordMaxVariableValues[i].value, ctrpart->wordMaxVariableValues[i].value, output, storeline, base, true, open, isEdited);
				output.push_back(closeObject(base));			// 3
			}
		}

		if (size > 0)
		{
			if (eventInfos->d_size() == 0)
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
	}

	paramMatch("variableInitialValues", variableInitialValues, ctrpart->variableInitialValues, output, storeline, base, false, open, isEdited);
	paramMatch("stringData", stringData, ctrpart->stringData, output, storeline, base, true, open, isEdited);

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", behaviorgraphdata::classname, output, isEdited);
}

void hkbbehaviorgraphdata::newNode()
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

	if (attributeDefaults)
	{
		usize size = attributeDefaults->d_size();

		output.push_back(openObject(base, ID, behaviorgraphdata::classname, behaviorgraphdata::signature));		// 1

		if (size == 0)
		{
			output.push_back(openParam(base, "attributeDefaults", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "attributeDefaults", size));		// 2

		for (auto& data : (*attributeDefaults))
		{
			output.push_back(autoVoid(base, data.value));
		}

		if (attributeDefaults->d_size() > 0) output.push_back(closeParam(base));			// 2
	}
	else
	{
		output.push_back(openParam(base, "attributeDefaults", 0) + closeParam());		// 2
		--base;
	}

	if (variableInfos)
	{
		usize size = variableInfos->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "variableInfos", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "variableInfos", size));		// 2
		
		for (auto& data : (*variableInfos))
		{
			output.push_back(openObject(base));		// 3
			output.push_back(openParam(base, "role"));		// 4
			output.push_back(openObject(base));		// 5
			output.push_back(autoParam(base, "role", data.role.getRole()));
			output.push_back(autoParam(base, "flags", data.role.flags.getflags()));
			output.push_back(closeObject(base));		// 5
			output.push_back(closeParam(base));		// 4
			output.push_back(autoParam(base, "type", data.getType()));
			output.push_back(closeObject(base));		// 3
		}

		if (variableInfos->d_size() > 0) output.push_back(closeParam(base));			// 2
	}
	else
	{
		output.push_back(openParam(base, "variableInfos", 0) + closeParam());		// 2
		--base;
	}

	if (characterPropertyInfos)
	{
		usize size = characterPropertyInfos->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "characterPropertyInfos", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "characterPropertyInfos", size));		// 2

		for (auto& data : (*characterPropertyInfos))
		{
			output.push_back(openObject(base));		// 3
			output.push_back(openParam(base, "role"));		// 4
			output.push_back(openObject(base));		// 5
			output.push_back(autoParam(base, "role", data.role.getRole()));
			output.push_back(autoParam(base, "flags", data.role.flags.getflags()));
			output.push_back(closeObject(base));		// 5
			output.push_back(closeParam(base));		// 4
			output.push_back(autoParam(base, "type", data.getType()));
			output.push_back(closeObject(base));		// 3
		}

		if (characterPropertyInfos->d_size() > 0) output.push_back(closeParam(base));			// 2
	}
	else
	{
		output.push_back(openParam(base, "characterPropertyInfos", 0) + closeParam());		// 2
		--base;
	}

	if (eventInfos)
	{
		usize size = eventInfos->d_size();

		if (size == 0)
		{
			output.push_back(openParam(base, "eventInfos", size) + closeParam());		// 2
			--base;
		}
		else output.push_back(openParam(base, "eventInfos", size));		// 2

		for (auto& flags : (*eventInfos))
		{
			output.push_back(openObject(base));		// 3
			output.push_back(autoParam(base, "flags", flags.flags.getflags()));
			output.push_back(closeObject(base));		// 3
		}

		if (eventInfos->d_size() > 0) output.push_back(closeParam(base));			// 2
	}
	else
	{
		output.push_back(openParam(base, "eventInfos", 0) + closeParam());		// 2
		--base;
	}

	usize size = wordMinVariableValues.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "wordMinVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "wordMinVariableValues", size));		// 2

	for (auto& value : wordMinVariableValues)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "value", value.value));
		output.push_back(closeObject(base));		// 3
	}

	if (wordMinVariableValues.size() > 0) output.push_back(closeParam(base));			// 2

	size = wordMaxVariableValues.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "wordMaxVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "wordMaxVariableValues", size));		// 2

	for (auto& value : wordMaxVariableValues)
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "value", value.value));
		output.push_back(closeObject(base));		// 3
	}

	if (wordMaxVariableValues.size() > 0) output.push_back(closeParam(base));			// 2

	output.push_back(autoParam(base, "variableInitialValues", variableInitialValues));
	output.push_back(autoParam(base, "stringData", stringData));

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, behaviorgraphdata::classname, output, true);
}

void hkbbehaviorgraphdata::parentRefresh()
{
	if (variableInitialValues) hkb_parent[variableInitialValues] = shared_from_this();
	if (stringData) hkb_parent[stringData] = shared_from_this();
}

void hkbbehaviorgraphdata::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableInitialValues)
	{
		parentRefresh();
		variableInitialValues->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (stringData)
	{
		parentRefresh();
		stringData->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbbehaviorgraphdata::matchScoring(vector<hkbvariablevalue>& ori, vector<hkbvariablevalue>& edit, string id)
{
	if (ori.size() == 0)
	{
		vector<hkbvariablevalue> newOri;
		vector<hkbvariablevalue> newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(hkbvariablevalue());
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
				scorelist[i][j] += 40;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = ((ori.size() - difference) / ori.size()) * 40;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vector<hkbvariablevalue> newOri;
	vector<hkbvariablevalue> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(hkbvariablevalue());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(hkbvariablevalue());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
}

void hkbbehaviorgraphdata::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbBehaviorGraphDataExport(string id)
{
	//stage 1 reading
	graphdata::graphdata oriGraphData;
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		string current = "";

		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];
			dataInfoProcess(line, oriGraphData, current, id);

			if (Error)
			{
				return;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbBehaviorGraphData Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	graphdata::graphdata newGraphData;

	if (FunctionLineNew[id].size() > 0)
	{
		string current = "";

		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];
			dataInfoProcess(line, newGraphData, current, id);

			if (Error)
			{
				return;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbBehaviorGraphData Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	// stage 2 identify edits
	vecstr output;
	bool IsEdited = false;
	size_t size = max(oriGraphData.variableInfos.size(), newGraphData.variableInfos.size());

	output.push_back(FunctionLineTemp[id][0]);
	output.push_back(FunctionLineNew[id][1]);

	if (FunctionLineTemp[id][2] != FunctionLineNew[id][2])
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(FunctionLineNew[id][2]);

		if (oriGraphData.variableInfos.size() != 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back(FunctionLineTemp[id][2]);
			output.push_back("<!-- CLOSE -->");
		}

		IsEdited = true;
	}
	else
	{
		output.push_back(FunctionLineTemp[id][2]);
	}

	for (unsigned int i = 0; i < size; i++)
	{
		if  (i < oriGraphData.variableInfos.size())
		{
			if (i < newGraphData.variableInfos.size())
			{
				bool open = false;
				vecstr storeline;
				output.push_back("				<hkobject>");
				output.push_back("					<hkparam name=\"role\">");
				output.push_back("						<hkobject>");
				
				if (oriGraphData.variableInfos[i].role != newGraphData.variableInfos[i].role)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
					output.push_back("							<hkparam name=\"role\">" + newGraphData.variableInfos[i].role + "</hkparam>");
					storeline.push_back("							<hkparam name=\"role\">" + oriGraphData.variableInfos[i].role + "</hkparam>");
				}
				else
				{
					output.push_back("							<hkparam name=\"role\">" + oriGraphData.variableInfos[i].role + "</hkparam>");
				}

				if (oriGraphData.variableInfos[i].flags != newGraphData.variableInfos[i].flags)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
					}
					
					output.push_back("							<hkparam name=\"flags\">" + newGraphData.variableInfos[i].flags + "</hkparam>");
					storeline.push_back("							<hkparam name=\"flags\">" + oriGraphData.variableInfos[i].flags + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						output.push_back("<!-- CLOSE -->");
						open = false;
					}

					output.push_back("							<hkparam name=\"flags\">" + oriGraphData.variableInfos[i].flags + "</hkparam>");
				}

				output.push_back("						</hkobject>");
				output.push_back("					</hkparam>");

				if (oriGraphData.variableInfos[i].type != newGraphData.variableInfos[i].type)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					output.push_back("					<hkparam name=\"type\">" + newGraphData.variableInfos[i].type + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("					<hkparam name=\"type\">" + oriGraphData.variableInfos[i].type + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
				}
				else
				{
					output.push_back("					<hkparam name=\"type\">" + oriGraphData.variableInfos[i].type + "</hkparam>");
				}

				output.push_back("				</hkobject>");
			}
			else
			{
				vecstr storeline;

				if (newGraphData.variableInfos.size() != 0)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (i < oriGraphData.variableInfos.size())
				{
					int add = 0;

					while (add < 9)
					{
						output.push_back("");
						++add;
					}

					inputVariableInfo(storeline, oriGraphData.variableInfos[i]);
					++i;
				}

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}
		}
		else
		{
			if (oriGraphData.variableInfos.size() != 0)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			while (i < newGraphData.variableInfos.size())
			{
				inputVariableInfo(output, newGraphData.variableInfos[i]);
				++i;
			}
		}
	}

	if (oriGraphData.variableInfos.size() != newGraphData.variableInfos.size())
	{
		if (newGraphData.variableInfos.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else if (oriGraphData.variableInfos.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- ORIGINAL -->");
			output.push_back(FunctionLineTemp[id][2]);
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("<!-- CLOSE -->");
			output.push_back("			</hkparam>");
		}
	}
	else if (oriGraphData.variableInfos.size() != 0)
	{
		output.push_back("			</hkparam>");
	}

	size = max(oriGraphData.characterPropertyInfos.size(), newGraphData.characterPropertyInfos.size());

	if (oriGraphData.characterPropertyInfos.size() != newGraphData.characterPropertyInfos.size())
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

		if (newGraphData.characterPropertyInfos.size() > 0)
		{
			output.push_back("			<hkparam name=\"characterPropertyInfos\" numelements=\"" + to_string(newGraphData.characterPropertyInfos.size()) + "\">");
		}
		else
		{
			output.push_back("			<hkparam name=\"characterPropertyInfos\" numelements=\"0\"></hkparam>");
		}

		if (oriGraphData.characterPropertyInfos.size() != 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"characterPropertyInfos\" numelements=\"" + to_string(oriGraphData.characterPropertyInfos.size()) + "\">");

			if (newGraphData.characterPropertyInfos.size() > 0)
			{
				output.push_back("<!-- CLOSE -->");
			}
		}

		IsEdited = true;
	}
	else
	{
		if (oriGraphData.characterPropertyInfos.size() == 0)
		{
			output.push_back("			<hkparam name=\"characterPropertyInfos\" numelements=\"0\"></hkparam>");
		}
		else
		{
			output.push_back("			<hkparam name=\"characterPropertyInfos\" numelements=\"" + to_string(oriGraphData.characterPropertyInfos.size()) + "\">");
		}
	}

	for (unsigned int i = 0; i < size; i++)
	{
		if (i < oriGraphData.characterPropertyInfos.size())
		{
			if (i < newGraphData.characterPropertyInfos.size())
			{
				bool open = false;
				vecstr storeline;
				output.push_back("				<hkobject>");
				output.push_back("					<hkparam name=\"role\">");
				output.push_back("						<hkobject>");

				if (oriGraphData.characterPropertyInfos[i].role != newGraphData.characterPropertyInfos[i].role)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
					output.push_back("							<hkparam name=\"role\">" + newGraphData.characterPropertyInfos[i].role + "</hkparam>");
					storeline.push_back("							<hkparam name=\"role\">" + oriGraphData.characterPropertyInfos[i].role + "</hkparam>");
				}
				else
				{
					output.push_back("							<hkparam name=\"role\">" + oriGraphData.characterPropertyInfos[i].role + "</hkparam>");
				}

				if (oriGraphData.characterPropertyInfos[i].flags != newGraphData.characterPropertyInfos[i].flags)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
					}

					output.push_back("							<hkparam name=\"flags\">" + newGraphData.characterPropertyInfos[i].flags + "</hkparam>");
					storeline.push_back("							<hkparam name=\"flags\">" + oriGraphData.characterPropertyInfos[i].flags + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						output.push_back("<!-- CLOSE -->");
						open = false;
					}

					output.push_back("							<hkparam name=\"flags\">" + oriGraphData.characterPropertyInfos[i].flags + "</hkparam>");
				}

				output.push_back("						</hkobject>");
				output.push_back("					</hkparam>");

				if (oriGraphData.characterPropertyInfos[i].type != newGraphData.characterPropertyInfos[i].type)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("					<hkparam name=\"type\">" + newGraphData.characterPropertyInfos[i].type + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("					<hkparam name=\"type\">" + oriGraphData.characterPropertyInfos[i].type + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					IsEdited = true;
				}
				else
				{
					output.push_back("					<hkparam name=\"type\">" + oriGraphData.characterPropertyInfos[i].type + "</hkparam>");
				}

				output.push_back("				</hkobject>");
			}
			else
			{
				vecstr storeline;

				if (newGraphData.characterPropertyInfos.size() != 0)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (i < oriGraphData.characterPropertyInfos.size())
				{
					int add = 0;

					while (add < 9)
					{
						output.push_back("");
						++add;
					}

					inputCharacterPropertyInfos(storeline, oriGraphData.characterPropertyInfos[i]);
					++i;
				}

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}
		}
		else
		{
			if (oriGraphData.characterPropertyInfos.size() != 0)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			while (i < newGraphData.characterPropertyInfos.size())
			{
				inputCharacterPropertyInfos(output, newGraphData.characterPropertyInfos[i]);
				++i;
			}
		}
	}

	if (oriGraphData.characterPropertyInfos.size() != newGraphData.characterPropertyInfos.size())
	{
		if (newGraphData.characterPropertyInfos.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else if (oriGraphData.characterPropertyInfos.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"characterPropertyInfos\" numelements=\"0\"></hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("<!-- CLOSE -->");
			output.push_back("			</hkparam>");
		}
	}
	else if (oriGraphData.characterPropertyInfos.size() != 0)
	{
		output.push_back("			</hkparam>");
	}

	size = max(oriGraphData.eventInfos.size(), newGraphData.eventInfos.size());

	if (oriGraphData.eventInfos.size() != newGraphData.eventInfos.size())
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

		if (newGraphData.eventInfos.size() > 0)
		{
			output.push_back("			<hkparam name=\"eventInfos\" numelements=\"" + to_string(newGraphData.eventInfos.size()) + "\">");
		}
		else
		{
			output.push_back("			<hkparam name=\"eventInfos\" numelements=\"0\"></hkparam>");
		}

		if (oriGraphData.eventInfos.size() != 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"eventInfos\" numelements=\"" + to_string(oriGraphData.eventInfos.size()) + "\">");
			output.push_back("<!-- CLOSE -->");
		}

		IsEdited = true;
	}
	else
	{
		if (oriGraphData.eventInfos.size() == 0)
		{
			output.push_back("			<hkparam name=\"eventInfos\" numelements=\"0\"></hkparam>");
		}
		else
		{
			output.push_back("			<hkparam name=\"eventInfos\" numelements=\"" + to_string(oriGraphData.eventInfos.size()) + "\">");
		}
	}
	
	for (unsigned int i = 0; i < size; i++)
	{
		if (i < oriGraphData.eventInfos.size())
		{
			if (i < newGraphData.eventInfos.size())
			{
				bool open = false;
				vecstr storeline;
				output.push_back("				<hkobject>");

				if (oriGraphData.eventInfos[i].flags != newGraphData.eventInfos[i].flags)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("					<hkparam name=\"flags\">" + newGraphData.eventInfos[i].flags + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("					<hkparam name=\"flags\">" + oriGraphData.eventInfos[i].flags + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					IsEdited = true;
				}
				else
				{
					output.push_back("					<hkparam name=\"flags\">" + oriGraphData.eventInfos[i].flags + "</hkparam>");
				}

				output.push_back("				</hkobject>");
			}
			else
			{
				vecstr storeline;

				if (newGraphData.eventInfos.size() != 0)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				while (i < oriGraphData.eventInfos.size())
				{
					int add = 0;

					while (add < 3)
					{
						output.push_back("");
						++add;
					}

					inputEventInfos(storeline, oriGraphData.eventInfos[i]);
					++i;
				}

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}
		}
		else
		{
			if (oriGraphData.eventInfos.size() != 0)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			while (i < newGraphData.eventInfos.size())
			{
				inputEventInfos(output, newGraphData.eventInfos[i]);
				++i;
			}
		}
	}

	if (oriGraphData.eventInfos.size() != newGraphData.eventInfos.size())
	{
		if (newGraphData.eventInfos.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else if (oriGraphData.eventInfos.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"eventInfos\" numelements=\"0\"></hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("<!-- CLOSE -->");
			output.push_back("			</hkparam>");
		}
	}
	else if (oriGraphData.eventInfos.size() != 0)
	{
		output.push_back("			</hkparam>");
	}

	if (oriGraphData.wordMinVariableValues != newGraphData.wordMinVariableValues)
	{
		cout << "ERROR: Unrecognized hkbBehaviorGraphData. Please report it to Nemesis' author" << endl;
		Error = true;
		return;
	}
	else
	{
		output.push_back("			<hkparam name=\"wordMinVariableValues\" numelements=\"0\"></hkparam>");
	}

	if (oriGraphData.wordMaxVariableValues != newGraphData.wordMaxVariableValues)
	{
		cout << "ERROR: Unrecognized hkbBehaviorGraphData. Please report it to Nemesis' author" << endl;
		Error = true;
		return;
	}
	else
	{
		output.push_back("			<hkparam name=\"wordMaxVariableValues\" numelements=\"0\"></hkparam>");
	}

	bool open = false;
	vecstr storeline;

	if (oriGraphData.variableInitialValues != newGraphData.variableInitialValues)
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		IsEdited = true;
		open = true;
		output.push_back(newGraphData.variableInitialValues);
		storeline.push_back(oriGraphData.variableInitialValues);
	}
	else
	{
		output.push_back(oriGraphData.variableInitialValues);
	}

	if (oriGraphData.stringData != newGraphData.stringData)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			open = true;
		}

		output.push_back(newGraphData.stringData);
		storeline.push_back(oriGraphData.stringData);
	}
	else
	{
		if (open)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			output.push_back("<!-- CLOSE -->");
			open = false;
		}

		output.push_back(oriGraphData.stringData);
	}

	if (open)
	{
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	output.push_back("		</hkobject>");

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
			cout << "ERROR: Edit hkbBehaviorGraphData Output Not Found (File: " << filename << ")" << endl;
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

void dataInfoProcess(string line, graphdata::graphdata& graphData, string& current, string id)
{
	if (current == "")
	{
		if (line.find("<hkparam name=\"attributeDefaults\" numelements=\"") != string::npos)
		{
			if (line.find("<hkparam name=\"attributeDefaults\" numelements=\"0\"></hkparam>") == string::npos)
			{
				cout << "ERROR: Unrecognized hkbBehaviorGraphData. Please report it to Nemesis' author" << endl;
				Error = true;
				return;
			}

			graphData.attributeDefaults = line;
			current = "variableInfos";
		}

		return;
	}
	else if (current == "variableInfos")
	{
		size_t pos = line.find("<hkparam name=\"role\">");

		if (pos != string::npos)
		{
			pos += 21;
			size_t nextpos = line.find("</hkparam>", pos);

			if (nextpos != string::npos)
			{
				graphData.variableInfos.back().role = line.substr(pos, nextpos - pos);
			}
			else
			{
				graphdata::variableinfo dummy;
				graphData.variableInfos.push_back(dummy);
			}

			return;
		}

		pos = line.find("<hkparam name=\"flags\">");

		if (pos != string::npos)
		{
			pos += 22;
			graphData.variableInfos.back().flags = line.substr(pos, line.find("</hkparam>", pos) - pos);
			return;
		}

		pos = line.find("<hkparam name=\"type\">");

		if (pos != string::npos)
		{
			pos += 21;
			graphData.variableInfos.back().type = line.substr(pos, line.find("</hkparam>", pos) - pos);
			return;
		}

		if (line.find("<hkparam name=\"characterPropertyInfos\" numelements=\"") != string::npos)
		{
			current = "characterPropertyInfos";
		}
	}
	else if (current == "characterPropertyInfos")
	{
		size_t pos = line.find("<hkparam name=\"role\">");

		if (pos != string::npos)
		{
			pos += 21;
			size_t nextpos = line.find("</hkparam>", pos);

			if (nextpos != string::npos)
			{
				graphData.characterPropertyInfos.back().role = line.substr(pos, nextpos - pos);
			}
			else
			{
				graphdata::characterpropertyinfo dummy;
				graphData.characterPropertyInfos.push_back(dummy);
			}

			return;
		}

		pos = line.find("<hkparam name=\"flags\">");

		if (pos != string::npos)
		{
			pos += 22;
			graphData.characterPropertyInfos.back().flags = line.substr(pos, line.find("</hkparam>", pos) - pos);
			return;
		}

		pos = line.find("<hkparam name=\"type\">");

		if (pos != string::npos)
		{
			pos += 21;
			graphData.characterPropertyInfos.back().type = line.substr(pos, line.find("</hkparam>", pos) - pos);
			return;
		}

		if (line.find("<hkparam name=\"eventInfos\" numelements=\"") != string::npos)
		{
			current = "eventInfos";
		}
	}
	else if (current == "eventInfos")
	{
		size_t pos = line.find("<hkparam name=\"flags\">");

		if (pos != string::npos)
		{
			pos += 22;
			graphdata::eventinfo newEvent;
			newEvent.flags = line.substr(pos, line.find("</hkparam>", pos) - pos);
			graphData.eventInfos.push_back(newEvent);
			return;
		}

		if (line.find("<hkparam name=\"wordMinVariableValues\" numelements=\"") != string::npos)
		{
			if (line.find("<hkparam name=\"wordMinVariableValues\" numelements=\"0\"></hkparam>") == string::npos)
			{
				cout << "ERROR: Unrecognized hkbBehaviorGraphData. Please report it to Nemesis' author" << endl;
				Error = true;
				return;
			}

			current = "wordMinVariableValues";
			graphData.wordMinVariableValues = line;
		}
	}
	else if (current == "wordMinVariableValues")
	{
		if (line.find("<hkparam name=\"wordMaxVariableValues\" numelements=\"0\"></hkparam>") == string::npos)
		{
			cout << "ERROR: Unrecognized hkbBehaviorGraphData. Please report it to Nemesis' author" << endl;
			Error = true;
			return;
		}

		current = "wordMaxVariableValues";
		graphData.wordMaxVariableValues = line;
	}
	else if (current == "wordMaxVariableValues")
	{
		if (line.find("<hkparam name=\"variableInitialValues\">") != string::npos)
		{
			graphData.variableInitialValues = line;
			current = "variableInitialValues";
		}
		else
		{
			cout << "ERROR: Missing data in hkbBehaviorGraphData(ID: " << id << ")" << endl;
			Error = true;
			return;
		}
	}
	else if (current == "variableInitialValues")
	{
		if (line.find("<hkparam name=\"stringData\">") != string::npos)
		{
			graphData.stringData = line;
			current = "stringData";
		}
		else
		{
			cout << "ERROR: Missing data in hkbBehaviorGraphData(ID: " << id << ")" << endl;
			Error = true;
			return;
		}
	}
}

void inputVariableInfo(vecstr& input, graphdata::variableinfo info)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"role\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"role\">" + info.role + "</hkparam>");
	input.push_back("							<hkparam name=\"flags\">" + info.flags + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"type\">" + info.type + "</hkparam>");
	input.push_back("				</hkobject>");
}

void inputCharacterPropertyInfos(vecstr& input, graphdata::characterpropertyinfo info)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"role\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"role\">" + info.role + "</hkparam>");
	input.push_back("							<hkparam name=\"flags\">" + info.flags + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"type\">" + info.type + "</hkparam>");
	input.push_back("				</hkobject>");
}

void inputEventInfos(vecstr& input, graphdata::eventinfo info)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"flags\">" + info.flags + "</hkparam>");
	input.push_back("				</hkobject>");
}

namespace keepsake
{
	void hkbBehaviorGraphDataExport(string id)
	{
		//stage 1 reading
		vecstr storeline1;
		storeline1.reserve(FunctionLineTemp[id].size());
		string line;

		if (FunctionLineTemp[id].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
			{
				line = FunctionLineTemp[id][i];

				if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
				{
					storeline1.push_back(line);
				}
			}
		}
		else
		{
			cout << "ERROR: Edit hkbBehaviorGraphData Input Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		vecstr storeline2;
		storeline2.reserve(FunctionLineNew[id].size());

		if (FunctionLineNew[id].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
			{
				line = FunctionLineNew[id][i];

				if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
				{
					storeline2.push_back(line);
				}
			}
		}
		else
		{
			cout << "ERROR: Edit hkbBehaviorGraphData Output Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		// stage 2 identify edits
		vecstr output;
		bool IsEdited = false;
		bool IsChanged = false;
		bool open = false;
		int curline = 3;
		int part = 0;
		int openpoint;
		int closepoint;

		output.push_back(storeline2[0]);
		output.push_back(storeline2[1]);

		if ((storeline1[2].find(storeline2[2], 0) == string::npos) || (storeline1[2].length() != storeline2[2].length()))
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			output.push_back(storeline2[2]);

			if ((storeline1[2].find("</hkparam>", 0) != string::npos) && (storeline2[3].find("numelements=", 0) == string::npos))
			{
				IsChanged = true;
				openpoint = 2;
				open = true;
			}
			else
			{
				output.push_back("<!-- ORIGINAL -->");
				output.push_back(storeline1[2]);
				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			IsEdited = true;
		}
		else
		{
			output.push_back(storeline2[2]);
		}

		for (unsigned int i = 3; i < storeline2.size(); i++)
		{
			if ((storeline1[curline].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) == string::npos) && (part == 0)) // existing variableInfos value
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}
				}
				else
				{
					if (open)
					{
						closepoint = curline;
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;

				if ((open) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) != string::npos))
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
			else if ((storeline1[curline].find("<hkparam name=\"eventInfos\" numelements=", 0) == string::npos) && (part == 1)) // existing characterPropertyInfos value
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}
				}
				else
				{
					if (open)
					{
						closepoint = curline;
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;

				if ((open) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) != string::npos))
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
			else if ((storeline1[curline].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) == string::npos) && (part == 2)) // existing eventInfos value
			{
				if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}
				}
				else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (open)
					{
						closepoint = curline;
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);
				}

				curline++;

				if ((open) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) != string::npos))
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					IsChanged = false;
					open = false;
				}
			}
			else if (part == 3) // existing leftover settings
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;
			}
			else // added variable value
			{
				if (storeline2[i].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					part = 1;
					curline++;
				}
				else if (storeline2[i].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (i + 1 < storeline2.size() && storeline2[i + 1].find("numelements=", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					part = 2;
					curline++;
				}
				else if (storeline2[i].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
					part = 3;
					curline++;
				}
				else
				{
					if (part == 0)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(storeline2[i]);

						if (i + 1 < storeline2.size() && storeline2[i + 1].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos)
						{
							output.push_back("				</hkobject>");

							if (IsChanged)
							{
								if (openpoint != curline)
								{
									output.push_back("<!-- ORIGINAL -->");

									for (int j = openpoint; j < curline; j++)
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
					else if (part == 1)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(storeline2[i]);

						if (i + 1 < storeline2.size() && storeline2[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
						{
							output.push_back("				</hkobject>");

							if (IsChanged)
							{
								if (openpoint != curline)
								{
									output.push_back("<!-- ORIGINAL -->");

									for (int j = openpoint; j < curline; j++)
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
					else if (part == 2)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(storeline2[i]);

						if (i + 1 < storeline2.size() && storeline2[i + 1].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos)
						{
							if (IsChanged)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
								{
									output.push_back(storeline1[j]);
								}

								IsChanged = false;
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
				}
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

		// stage 3 output if it is edited
		string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
		bool closeOri = false;
		bool closeEdit = false;

		if (IsEdited)
		{
			ofstream outputfile(filename);

			if (outputfile.is_open())
			{
				FunctionWriter fwrite(&outputfile);
				part = 0;

				for (unsigned int i = 0; i < output.size(); i++)
				{
					if (i < output.size() - 1)
					{
						if ((output[i + 1].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"variableInfos\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
						{
							if (!closeEdit)
							{
								fwrite << "			</hkparam>" << "\n";
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
						{
							if (!closeEdit)
							{
								fwrite << "			</hkparam>" << "\n";
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos))
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
						{
							if (!closeEdit)
							{
								fwrite << "			</hkparam>" << "\n";
								closeEdit = true;
							}
						}
					}

					if (part == 0)
					{
						if ((output[i].find("<hkparam name=\"variableInfos\" numelements=", 0) != string::npos) || (output[i].find("<hkparam name=\"characterPropertyInfos\" numelements=", 0) != string::npos))
						{
							if (output[i - 1].find("OPEN", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
								}
								else
								{
									closeEdit = false;
								}
							}
							else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeOri = true;
								}
								else
								{
									closeOri = false;
								}
							}
							else
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
									closeOri = true;
								}
								else
								{
									closeEdit = false;
									closeOri = false;
								}
							}
						}

						if ((output[i].find("<hkparam name=\"role\">", 0) != string::npos) && (output[i].find("</hkparam>", 0) == string::npos))
						{
							fwrite << "				<hkobject>" << "\n";
							fwrite << output[i] << "\n";
							fwrite << "						<hkobject>" << "\n";
						}
						else if (output[i].find("<hkparam name=\"flags\">", 0) != string::npos)
						{
							fwrite << output[i] << "\n";

							if (output[i + 1].find("CLOSE", 0) != string::npos)
							{
								fwrite << "<!-- CLOSE -->" << "\n";
								i++;
							}

							if (output[i + 1].find("ORIGINAL", 0) == string::npos)
							{
								fwrite << "						</hkobject>" << "\n";
								fwrite << "					</hkparam>" << "\n";
							}
						}
						else if (output[i].find("<hkparam name=\"type\">", 0) != string::npos)
						{
							fwrite << output[i] << "\n";

							if (output[i + 1].find("CLOSE", 0) != string::npos)
							{
								fwrite << "<!-- CLOSE -->" << "\n";
								i++;
							}

							if ((output[i + 1].find("</hkobject>", 0) == string::npos) && (output[i + 1].find("ORIGINAL", 0) == string::npos))
							{
								fwrite << "				</hkobject>" << "\n";
							}
						}
						else
						{
							fwrite << output[i] << "\n";
						}

						if (output[i + 1].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
						{
							part = 1;
						}
					}
					else if (part == 1)
					{
						if (output[i].find("<hkparam name=\"eventInfos\" numelements=", 0) != string::npos)
						{
							if (output[i - 1].find("OPEN", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
								}
								else
								{
									closeEdit = false;
								}
							}
							else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeOri = true;
								}
								else
								{
									closeOri = false;
								}
							}
							else
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
									closeOri = true;
								}
								else
								{
									closeEdit = false;
									closeOri = false;
								}
							}
						}

						if (output[i].find("<hkparam name=\"flags\">", 0) != string::npos)
						{
							fwrite << "				<hkobject>" << "\n";
							fwrite << output[i] << "\n";
							fwrite << "				</hkobject>" << "\n";
						}
						else
						{
							fwrite << output[i] << "\n";
						}

						if (output[i + 1].find("<hkparam name=\"wordMinVariableValues\" numelements=", 0) != string::npos)
						{
							part = 2;
						}
					}
					else
					{
						fwrite << output[i] << "\n";
					}

					if (i < output.size() - 1)
					{
						if (output[i + 1].find("numelements=", 0) != string::npos) // close outside if both aren't closed
						{
							if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("<hkparam name=\"type\">", 0) != string::npos) || (output[i].find("<hkparam name=\"flags\">", 0) != string::npos))
							{
								if ((!closeOri) && (!closeEdit))
								{
									fwrite << "			</hkparam>" << "\n";
									closeOri = true;
									closeEdit = true;
								}
							}
						}
					}
				}

				fwrite << "		</hkobject>" << "\n";
				outputfile.close();
			}
			else
			{
				cout << "ERROR: Edit hkbBehaviorGraphData Output Not Found (File: " << filename << ")" << endl;
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
}
