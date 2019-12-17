#include <boost\thread.hpp>
#include <boost\algorithm\string.hpp>
#include "hkbvariablevalueset.h"
#include "src\utilities\stringdatalock.h"

using namespace std;

namespace variablevalueset
{
	const string key = "u";
	const string classname = "hkbVariableValueSet";
	const string signature = "0x27812d8d";
}

string hkbvariablevalueset::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbvariablevalueset>> hkbvariablevaluesetList;
safeStringUMap<shared_ptr<hkbvariablevalueset>> hkbvariablevaluesetList_E;

void hkbvariablevalueset::regis(string id, bool isEdited)
{
	isEdited ? hkbvariablevaluesetList_E[id] = shared_from_this() : hkbvariablevaluesetList[id] = shared_from_this();
	ID = id;
}

void hkbvariablevalueset::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;
	usize counter = 0;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				usize numelement;

				if (readEleParam("wordVariableValues", line, numelement))
				{
					if (numelement != 0)
					{
						usize numID = stoi(ID.substr(1)) - 1;
						safeStringUMap<shared_ptr<variableinfopack>>& infoPackPtr = isEdited ? variablePackNode_E : variablePackNode;
						safeStringUMap<shared_ptr<hkbvariablevalueset>>& variablevaluesetPtr = isEdited ? hkbvariablevaluesetList_E : hkbvariablevaluesetList;

						do
						{
							string strID = to_string(numID);

							while (strID.length() < 4)
							{
								strID = "0" + strID;
							}

							auto itr = infoPackPtr.find("#" + strID);

							if (itr != infoPackPtr.end())
							{
								wordVariableValues = itr->second;
								break;
							}
							else
							{
								auto it = variablevaluesetPtr.find("#" + strID);

								if (it != variablevaluesetPtr.end()) break;
							}
						} while (--numID > 0);
					}

					++type;
				}

				break;
			}
			case 1:
			{
				if (wordVariableValues && counter < wordVariableValues->size() && readParam("value", line, (*wordVariableValues)[counter].value))
				{
					++counter;
					break;
				}

				usize numelement;

				if (readEleParam("quadVariableValues", line, numelement))
				{
					if (numelement != 0) quadVariableValues.reserve(numelement);

					++type;
				}

				break;
			}
			case 2:
			{
				if (line.find("(") != string::npos)
				{
					vector<string> tokens;
					boost::trim_if(line, boost::is_any_of("\t ()"));
					boost::split(tokens, line, boost::is_any_of("\t ()"), boost::token_compress_on);

					if (tokens.size() > 0 && isOnlyNumber(tokens[0]))
					{
						quadVariableValues.push_back(coordinate(tokens[0], tokens[1], tokens[2], tokens[3]));
						break;
					}
				}

				usize numelement;

				if (readEleParam("variantVariableValues", line, numelement))
				{
					variantVariableValues.reserve(numelement);
					++type;
				}

				break;
			}
			case 3:
			{
				if (line.find("\t#") != string::npos)
				{
					vecstr tokens;
					boost::trim_if(line, boost::is_any_of("\t "));
					boost::split(tokens, line, boost::is_any_of("\t "), boost::token_compress_on);

					for (auto& ref : tokens)
					{
						variantVariableValues.emplace_back((isEdited ? editedBehavior : originalBehavior)[ref]);
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << variablevalueset::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbvariablevalueset::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + variablevalueset::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			usize size = variantVariableValues.size();
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			for (usize i = 0; i < size; ++i)
			{
				threadedNextNode(variantVariableValues[i], filepath, curadd + to_string(i), functionlayer, graphroot);
			}
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
				hkbvariablevaluesetList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbvariablevaluesetList_E[ID] = protect;
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
				hkbvariablevaluesetList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbvariablevaluesetList_E[ID] = protect;
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

string hkbvariablevalueset::getClassCode()
{
	return variablevalueset::key;
}

void hkbvariablevalueset::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	if (num_stringData > 0)
	{
		unique_lock<mutex> lock(mutex_stringData);
		cont_stringData.wait(lock, [] {return num_stringData == 0; });
	}

	bool open = false;
	bool isEdited = false;
	hkbvariablevalueset* ctrpart = static_cast<hkbvariablevalueset*>(counterpart.get());
	vecstr storeline;
	storeline.reserve(10);
	usize base = 2;
	usize size = wordVariableValues->d_size();
	vecstr output;
	output.reserve(5000);

	output.push_back(openObject(base, ID, variablevalueset::classname, variablevalueset::signature));	// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "wordVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "wordVariableValues", size));		// 2

	size = wordVariableValues->size();

	for (usize i = 0; i < size; ++i)
	{
		// newly created data
		if ((*wordVariableValues)[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				output.push_back(openObject(base));		// 3
				output.push_back(autoParam(base, "value", (*ctrpart->wordVariableValues)[i].value));
				output.push_back(closeObject(base));	// 3
				++i;
			}
		}
		// deleted existing data
		else if ((*ctrpart->wordVariableValues)[i].proxy)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				if (!(*ctrpart->wordVariableValues)[i].proxy)
				{
					--i;
					break;
				}

				storeline.push_back(openObject(base));		// 3
				storeline.push_back(autoParam(base, "value", (*wordVariableValues)[i].value));
				storeline.push_back(closeObject(base));	// 3
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
			output.push_back(openObject(base));		// 3
			paramMatch("value", (*wordVariableValues)[i].value, (*ctrpart->wordVariableValues)[i].value, output, storeline, base, true, open, isEdited);
			output.push_back(closeObject(base));	// 3
		}
	}

	if (size > 0)
	{
		if (wordVariableValues->d_size() == 0)
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

	size = quadVariableValues.size();
	usize size2 = ctrpart->quadVariableValues.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "quadVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "quadVariableValues", size));		// 2

	if (size > size2)
	{
		cout << "ERROR: Do not delete base data to ensure compatibility with other behavior mods" << endl;
		Error = true;
		throw 5;
	}

	for (usize i = 0; i < size; ++i)
	{
		voidMatch(quadVariableValues[i], ctrpart->quadVariableValues[i], output, storeline, base, false, open, isEdited);
	}

	// newly created
	{
		usize i = quadVariableValues.size();

		if (i < size2)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size2)
			{
				output.push_back(autoVoid(base, ctrpart->quadVariableValues[i]));
				++i;
			}
		}
	}

	if (size2 > 0)
	{
		if (size == 0)
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

	size = variantVariableValues.size();
	size2 = ctrpart->variantVariableValues.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "variantVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "variantVariableValues", size));		// 2

	if (size > size2)
	{
		cout << "ERROR: Do not delete base data to ensure compatibility with other behavior mods" << endl;
		Error = true;
		throw 5;
	}

	for (usize i = 0; i < size; ++i)
	{
		voidMatch(variantVariableValues[i], ctrpart->variantVariableValues[i], output, storeline, base, false, open, isEdited);
	}

	// newly created
	{
		usize i = variantVariableValues.size();

		if (i < size2)
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size2)
			{
				string line;

				do
				{
					line.append(ctrpart->variantVariableValues[i]->ID + " ");
					++i;
				} while (i < size2 && i % 16 != 0);

				line.pop_back();
				output.push_back(autoVoid(base, line));
			}
		}
	}

	if (size2 > 0)
	{
		if (size == 0)
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", variablevalueset::classname, output, isEdited);
}

void hkbvariablevalueset::newNode()
{
	if (num_stringData > 0)
	{
		unique_lock<mutex> lock(mutex_stringData);
		cont_stringData.wait(lock, [] {return num_stringData == 0; });
	}

	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = wordVariableValues->size();
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(5000);

	output.push_back(openObject(base, ID, variablevalueset::classname, variablevalueset::signature));	// 1

	if (size == 0)
	{
		output.push_back(openParam(base, "wordVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "wordVariableValues", size));		// 2
	
	for (auto& data : (*wordVariableValues))
	{
		output.push_back(openObject(base));		// 3
		output.push_back(autoParam(base, "value", data.value));
		output.push_back(closeObject(base));	// 3
	}

	if (size > 0) output.push_back(closeParam(base));			// 2

	size = quadVariableValues.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "quadVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "quadVariableValues", size));		// 2
	
	for (auto& cor : quadVariableValues)
	{
		output.push_back(autoVoid(base, cor.getString()));
	}

	if (size > 0) output.push_back(closeParam(base));			// 2

	size = variantVariableValues.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "variantVariableValues", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "variantVariableValues", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(variantVariableValues[counter]->ID + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));			// 2

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, variablevalueset::classname, output, true);
}

void hkbvariablevalueset::parentRefresh()
{
	for (auto& value : variantVariableValues)
	{
		hkb_parent[value] = shared_from_this();
	}
}

void hkbvariablevalueset::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld)
	{
		++functionlayer;
		usize tempint = 0;

		for (auto& value : variantVariableValues)
		{
			parentRefresh();
			value->connect(filepath, address + to_string(tempint), functionlayer, true, graphroot);

			if (IsForeign.find(value->ID) == IsForeign.end()) ++tempint;
		}
	}
	else
	{
		for (auto& value : variantVariableValues)
		{
			parentRefresh();
			value->connect(filepath, address, functionlayer, true, graphroot);
		}
	}
}

void hkbvariablevalueset::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

safeStringUMap<shared_ptr<hkreferencedobject>> hkreferencedobjectList;
safeStringUMap<shared_ptr<hkreferencedobject>> hkreferencedobjectList_E;

void hkreferencedobject::regis(string id, bool isEdited)
{
	isEdited ? hkreferencedobjectList_E[id] = shared_from_this() : hkreferencedobjectList[id] = shared_from_this();
	ID = id;
}

namespace referenceobject
{
	const string key = "a";
	const string classname = "hkReferencedObject";
	const string signature = "";
}

string hkreferencedobject::getClassCode()
{
	return referenceobject::key;
}
