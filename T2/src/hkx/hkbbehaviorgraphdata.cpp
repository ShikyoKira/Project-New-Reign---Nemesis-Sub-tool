#include <boost\thread.hpp>
#include <boost\algorithm\string.hpp>

#include "highestscore.h"
#include "hkbbehaviorgraphdata.h"

using namespace std;
vector<usize> datapacktracker;

namespace behaviorgraphdata
{
	const string key = "f";
	const string classname = "hkbBehaviorGraphData";
	const string signature = "0x95aca5d";
}

string hkbbehaviorgraphdata::GetAddress()
{
	return address;
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
