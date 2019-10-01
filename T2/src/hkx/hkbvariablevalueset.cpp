#include <boost\thread.hpp>
#include <boost\algorithm\string.hpp>
#include "hkbvariablevalueset.h"
#include "src\utilities\stringdatalock.h"

using namespace std;

namespace variablevalueset
{
	string key = "u";
	string classname = "hkbVariableValueSet";
	string signature = "0x27812d8d";
}

struct variablevalue
{
	vecstr wordVariableValues;
	vecstr quadVariableValues;
	vecstr variantVariableValues;
};

void inputGraphStringData(vecstr& output, vecstr orinamelist, vecstr newnamelist, bool& IsEdited, string key);
void valuesetprocess(string line, variablevalue& valueSet, int& current);

hkbvariablevalueset::hkbvariablevalueset(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + variablevalueset::key + to_string(functionlayer) + ">";

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

void hkbvariablevalueset::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableValueSet (ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		bool open = false;

		for (unsigned int i = 0; i < FunctionLineOriginal[id].size(); i++)
		{
			line = FunctionLineOriginal[id][i];

			if (open)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					open = false;
				}
				else
				{
					usize tempint = 0;
					usize counter = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < counter; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempbone = line.substr(position, tempint - position - 1);
						boneArray.push_back(tempbone);
						parent[tempbone] = id;
						referencingIDs[tempbone].push_back(id);
					}
				}
			}
			else if (line.find("<hkparam name=\"variantVariableValues\" numelements=\"", 0) != string::npos)
			{
				boneCount = stoi(boost::regex_replace(string(line), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));

				if (boneCount != 0)
				{
					boneArray.reserve(boneCount);
					open = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbVariableValueSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}
	
	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbVariableValueSet (ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablevalueset::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbVariableValueSet (ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr storeline2;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		storeline2 = FunctionLineEdited[id];
		bool open = false;

		for (unsigned int i = 0; i < storeline2.size(); ++i)
		{
			line = storeline2[i];

			if (open)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					open = false;
				}
				else
				{
					usize tempint = 0;
					usize counter = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < counter; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempbone = line.substr(position, tempint - position - 1);
						
						if (!exchangeID[tempbone].empty())
						{
							usize tempint = line.find(tempbone);
							usize templength = tempbone.length();
							tempbone = exchangeID[tempbone];
							storeline2[i].replace(tempint, templength, tempbone);
						}

						boneArray.push_back(tempbone);
						parent[tempbone] = id;
						referencingIDs[tempbone].push_back(id);
					}
				}
			}
			else if (line.find("<hkparam name=\"variantVariableValues\" numelements=\"", 0) != string::npos)
			{
				boneCount = stoi(boost::regex_replace(string(line), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));

				if (boneCount != 0)
				{
					boneArray.reserve(boneCount);
					open = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbVariableValueSet Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2	
	if (addressID[address] != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbVariableValueSet (newID: " << id << ") with hkbVariableValueSet (oldID: " << tempid << ")" << endl;
		}

		for (auto& bone : boneArray)
		{
			parent[bone] = tempid;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < storeline2.size(); i++)
		{
			FunctionLineNew[tempid].push_back(storeline2[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbVariableValueSet (newID: " << id << ") with hkbVariableValueSet (oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = storeline2;
		address = tempaddress;
	}

	RecordID(id, address, true);
	
	if ((Debug) && (!Error))
	{
		cout << "hkbVariableValueSet (ID: " << id << ") is complete!" << endl;
	}
}

void hkbvariablevalueset::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbVariableValueSet (ID: " << id << ") has been initialized!" << endl;
	}

	bool open = false;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (open)
			{
				if (line.find("</hkparam>", 0) != string::npos)
				{
					open = false;
				}
				else
				{
					usize tempint = 0;
					usize counter = count(line.begin(), line.end(), '#');

					for (unsigned int i = 0; i < counter; i++)
					{
						usize position = line.find("#", tempint);
						tempint = line.find("#", position + 1);
						string tempbone = line.substr(position, tempint - position - 1);

						if (!exchangeID[tempbone].empty())
						{
							tempbone = exchangeID[tempbone];
						}

						boneArray.push_back(tempbone);
						parent[tempbone] = id;
					}
				}
			}
			else if (line.find("<hkparam name=\"variantVariableValues\" numelements=\"", 0) != string::npos)
			{
				boneCount = stoi(boost::regex_replace(string(line), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));

				if (boneCount != 0)
				{
					boneArray.reserve(boneCount);
					open = true;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbVariableValueSet Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbVariableValueSet (ID: " << id << ") is complete!" << endl;
	}
}

string hkbvariablevalueset::GetAddress()
{
	return address;
}

string hkbvariablevalueset::GetBoneArray(int order)
{
	return boneArray[order];
}

int hkbvariablevalueset::GetBoneCount()
{
	return boneCount;
}

bool hkbvariablevalueset::IsNegate()
{
	return IsNegated;
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

void hkbVariableValueSetExport(string id)
{
	//stage 1 reading
	variablevalue oriValueSet;

	if (FunctionLineTemp[id].size() > 0)
	{
		int current = 0;

		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			valuesetprocess(FunctionLineTemp[id][i], oriValueSet, current);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbVariableValueSet Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	variablevalue newValueSet;

	if (FunctionLineNew[id].size() > 0)
	{
		int current = 0;

		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			valuesetprocess(FunctionLineNew[id][i], newValueSet, current);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbVariableValueSet Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}
	
	// stage 2 identify edits
	vecstr output;
	bool IsEdited = false;
	output.push_back(FunctionLineTemp[id][0]);
	inputGraphStringData(output, oriValueSet.wordVariableValues, newValueSet.wordVariableValues, IsEdited, "wordVariableValues");
	inputGraphStringData(output, oriValueSet.quadVariableValues, newValueSet.quadVariableValues, IsEdited, "quadVariableValues");
	inputGraphStringData(output, oriValueSet.variantVariableValues, newValueSet.variantVariableValues, IsEdited, "variantVariableValues");
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
			cout << "ERROR: Edit hkbVariableValueSet Output Not Found (File: " << filename << ")" << endl;
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

void valuesetprocess(string line, variablevalue& valueSet, int& current)
{
	if (current == 0)
	{
		if (line.find("<hkparam name=\"wordVariableValues\" numelements=\"") != string::npos)
		{
			++current;
		}
	}
	else if (current == 1)
	{
		if (line.find("<hkparam name=\"value\">") != string::npos)
		{
			string value = boost::regex_replace(string(line), boost::regex("([\t]+)<<hkparam name=\"value\">([^<]+)</hkparam>"), string("\\2"));
			valueSet.wordVariableValues.push_back(value);
		}

		if (line.find("<hkparam name=\"quadVariableValues\" numelements=\"") != string::npos)
		{
			++current;
		}
	}
	else if (current == 2)
	{
		if (line.find("</hkparam>") == string::npos)
		{
			valueSet.quadVariableValues.push_back(line);
		}

		if (line.find("<hkparam name=\"variantVariableValues\" numelements=\"") != string::npos)
		{
			++current;
		}
	}
	else if (current == 3)
	{
		if (line.find("</hkobject>") == string::npos && line.find("</hkparam>") == string::npos)
		{
			valueSet.variantVariableValues.push_back(line);
		}
		else
		{
			++current;
		}
	}
}

void inputGraphStringData(vecstr& output, vecstr orinamelist, vecstr newnamelist, bool& IsEdited, string key)
{
	bool open = false;
	vecstr storeline;
	size_t size = max(orinamelist.size(), newnamelist.size());

	if (orinamelist.size() != newnamelist.size())
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

		if (newnamelist.size() > 0)
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"" + to_string(newnamelist.size()) + "\">");
		}
		else
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"0\"></hkparam>");
		}

		if (orinamelist.size() != 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"" + to_string(orinamelist.size()) + "\">");
			output.push_back("<!-- CLOSE -->");
		}

		IsEdited = true;
	}
	else
	{
		if (orinamelist.size() == 0)
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"0\"></hkparam>");
		}
		else
		{
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"" + to_string(orinamelist.size()) + "\">");
		}
	}

	for (unsigned int i = 0; i < size; i++)
	{
		if (i < orinamelist.size())
		{
			if (i < newnamelist.size())
			{
				if (orinamelist[i] != newnamelist[i])
				{
					if (key == "wordVariableValues")
					{
						output.push_back("				<hkobject>");
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(newnamelist[i]);
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(orinamelist[i]);
						output.push_back("<!-- CLOSE -->");
						output.push_back("				</hkobject>");
						IsEdited = true;
					}
					else
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}
						
						output.push_back(newnamelist[i]);
						storeline.push_back(orinamelist[i]);
					}
				}
				else
				{
					if (key == "wordVariableValues")
					{
						output.push_back("				<hkobject>");
						output.push_back(orinamelist[i]);
						output.push_back("				</hkobject>");
					}
					else
					{
						if (open)
						{
							output.push_back("<!-- ORIGINAL -->");
							output.insert(output.end(), storeline.begin(), storeline.end());
							storeline.clear();
							open = false;
						}

						output.push_back(orinamelist[i]);
					}
				}
			}
			else
			{
				vecstr instore;

				if (newnamelist.size() != 0)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				if (key == "wordVariableValues")
				{
					while (i < orinamelist.size())
					{
						output.push_back("");
						output.push_back("");
						output.push_back("");
						instore.push_back("				<hkobject>");
						instore.push_back(orinamelist[i]);
						instore.push_back("				</hkobject>");
						++i;
					}
				}
				else
				{
					while (i < orinamelist.size())
					{
						output.push_back("");
						instore.push_back(orinamelist[i]);
						++i;
					}
				}

				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());
			}
		}
		else
		{
			if (orinamelist.size() != 0)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			if (key == "wordVariableValues")
			{
				while (i < newnamelist.size())
				{
					output.push_back("				<hkobject>");
					output.push_back(newnamelist[i]);
					output.push_back("				</hkobject>");
					++i;
				}
			}
			else
			{
				while (i < newnamelist.size())
				{
					output.push_back(newnamelist[i]);
					++i;
				}
			}
		}
	}

	if (orinamelist.size() != newnamelist.size())
	{
		if (newnamelist.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else if (orinamelist.size() == 0)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"" + key + "\" numelements=\"0\"></hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("<!-- CLOSE -->");
			output.push_back("			</hkparam>");
		}
	}
	else if (orinamelist.size() != 0)
	{
		if (open)
		{
			output.push_back("<!-- CLOSE -->");
		}

		output.push_back("			</hkparam>");
	}
}

namespace keepsake
{
	void hkbVariableValueSetExport(string id)
	{
		//stage 1 reading
		vecstr storeline1;
		storeline1.reserve(FunctionLineTemp[id].size());
		bool skip = false;
		string line;

		if (FunctionLineTemp[id].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
			{
				line = FunctionLineTemp[id][i];

				if (!skip)
				{
					storeline1.push_back(line);
				}
				else
				{
					skip = false;
				}

				if (line.find("name=\"value\">", 0) != string::npos)
				{
					storeline1.pop_back();
					storeline1.pop_back();
					storeline1.push_back(line);
					skip = true;
				}
			}
		}
		else
		{
			cout << "ERROR: Edit hkbVariableValueSet Input Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		skip = false;
		vecstr storeline2;
		storeline2.reserve(FunctionLineNew[id].size());

		if (FunctionLineNew[id].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
			{
				line = FunctionLineNew[id][i];

				if (line[line.size() - 1] == '\n')
				{
					line.pop_back();
				}

				if (!skip)
				{
					storeline2.push_back(line);
				}
				else
				{
					skip = false;
				}

				if (line.find("name=\"value\">", 0) != string::npos)
				{
					storeline2.pop_back();
					storeline2.pop_back();
					storeline2.push_back(line);
					skip = true;
				}
			}
		}
		else
		{
			cout << "ERROR: Edit hkbVariableValueSet Output Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		// stage 2 identify edits
		vecstr output;
		bool open = false;
		bool IsEdited = false;
		int curline = 2;
		int openpoint;
		int closepoint;
		int nextpoint;
		skip = false;

		output.push_back(storeline2[0]);

		if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			openpoint = curline - 1;
			IsEdited = true;
			open = true;

			if (storeline1[1].find("</hkparam>") != string::npos)
			{
				skip = true;
			}
		}

		output.push_back(storeline2[1]);

		for (unsigned int i = 2; i < storeline2.size(); i++)		// wordVariableValues data
		{
			if (storeline1[curline].find("<hkparam name=\"quadVariableValues\" numelements=", 0) == string::npos)		// existing variable value
			{
				if (storeline1[curline].find(storeline2[i], 0) != string::npos)
				{
					if (open)
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
						IsEdited = true;
						open = true;
					}
				}

				if (!skip)
				{
					curline++;
				}
			}
			else // added variable value
			{
				if (storeline2[i].find("<hkparam name=\"quadVariableValues\" numelements=", 0) != string::npos)
				{
					if (open)
					{
						if (!skip)
						{
							closepoint = curline - 1;
							output.pop_back();
						}
						else
						{
							closepoint = curline;
						}

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;

						if (!skip)
						{
							output.push_back("			</hkparam>");
						}
					}

					nextpoint = i;
					break;
				}
				else
				{
					if (storeline1[curline].find(storeline2[i], 0) == string::npos)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}
					}
					else
					{
						if (open)
						{
							closepoint = curline - 1;
							output.pop_back();

							if (closepoint != openpoint)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);
								}
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
				}
			}

			output.push_back(storeline2[i]);
		}

		skip = false;

		for (unsigned int i = nextpoint; i < storeline2.size(); i++)		// quadVariableValues data
		{
			if (storeline1[curline].find("<hkparam name=\"variantVariableValues\" numelements=", 0) == string::npos && !skip)		// existing variable value
			{
				if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					if (storeline1[curline].find("</hkparam>") != string::npos && i == nextpoint)
					{
						skip = true;
					}
				}
				else
				{
					if (open)
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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}

				if (!skip)
				{
					curline++;
				}
			}
			else
			{
				if (storeline2[i].find("</hkparam>", 0) != string::npos && skip)
				{
					curline++;
				}

				if (storeline2[i].find("<hkparam name=\"variantVariableValues\" numelements=", 0) != string::npos)
				{
					if (open)
					{
						if (skip)
						{
							closepoint = curline;
						}
						else
						{
							closepoint = curline - 1;
							output.pop_back();
						}

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						output.push_back("			</hkparam>");
						open = false;
					}

					nextpoint = i;
					break;
				}
				else
				{
					if (storeline1[curline].find(storeline2[i], 0) == string::npos)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}
					}
					else
					{
						if (open)
						{
							closepoint = curline - 1;
							output.pop_back();

							if (closepoint != openpoint)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);
								}
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
				}
			}

			output.push_back(storeline2[i]);
		}

		bool IsNewChild = false;
		vecstr newchild;

		for (unsigned int i = nextpoint; i < storeline2.size(); i++)		 // variantVariableValues data
		{
			if (curline != storeline1.size() && !IsNewChild)		// existing variable value
			{
				skip = false;

				if ((storeline2[i].find("<hkparam name=\"variantVariableValues\" numelements=", 0) != string::npos) && (storeline2[i].find(storeline1[curline], 0) == string::npos))
				{
					int numelement1 = stoi(boost::regex_replace(string(storeline2[i]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));
					int numelement2 = stoi(boost::regex_replace(string(storeline1[curline]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1")));

					if (numelement1 != numelement2)
					{
						IsNewChild = true;

						if (storeline1[curline].find("</hkparam>") != string::npos)
						{
							skip = true;
						}
					}
				}

				if (storeline2[i].find(storeline1[curline], 0) == string::npos)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}
				}
				else
				{
					if (open)
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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}

				output.push_back(storeline2[i]);

				if (curline != storeline1.size() - 1 && !skip)
				{
					curline++;
				}
			}
			else
			{
				if (storeline2[i].find("</hkparam>", 0) != string::npos && skip)
				{
					skip = false;
				}

				if (storeline1[curline].find("</hkparam>", 0) != string::npos && !skip)
				{
					if ((storeline2[i].find("</hkparam>", 0) != string::npos) && (storeline2[i].length() < 15))
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							openpoint = curline;
							IsEdited = true;
							open = true;
						}

						for (unsigned int j = 0; j < newchild.size(); j++)
						{
							output.push_back(newchild[j]);
						}
					}

					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					if (open)
					{
						closepoint = curline;

						if (storeline1[curline].find("</hkparam>", 0) > 15)
						{
							closepoint++;
						}

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}

					output.push_back(storeline2[i]);

					if (curline != storeline1.size() - 1)
					{
						curline++;
					}
				}
				else if (storeline1[curline].find(storeline2[i], 0) == string::npos)
				{
					usize size = count(storeline1[curline].begin(), storeline1[curline].end(), '#');
					usize size2 = count(storeline2[i].begin(), storeline2[i].end(), '#');
					string newline;

					if (size < size2)
					{
						usize position = 0;
						usize tempint = 0;

						for (unsigned int j = 0; j < size + 1; j++)
						{
							position = storeline2[i].find("#", tempint);
							tempint = storeline2[i].find("#", position + 1);
						}

						newline = storeline2[i].substr(0, position - 1);
						newchild.push_back("				" + storeline2[i].substr(position, -1));
					}
					else
					{
						newline = storeline2[i];
					}

					if (newline.find(storeline1[curline], 0) == string::npos)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							openpoint = curline;
							IsEdited = true;
							open = true;
						}
					}
					else
					{
						if (open)
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

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}

					if (!skip)
					{
						output.push_back(newline);

						if (curline != storeline1.size() - 1)
						{
							curline++;
						}
					}
				}
				else
				{
					if (storeline2[i].find(storeline1[curline], 0) == string::npos)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							openpoint = curline;
							IsEdited = true;
							open = true;
						}
					}
					else
					{
						if (open)
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

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}

					output.push_back(storeline2[i]);

					if (curline != storeline1.size() - 1)
					{
						curline++;
					}
				}
			}
		}

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
					if (output[i].find("name=\"value\">", 0) != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}

				outputfile.close();
			}
			else
			{
				cout << "ERROR: Edit hkbVariableValueSet Output Not Found (File: " << filename << ")" << endl;
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

safeStringUMap<shared_ptr<hkreferencedobject>> hkreferencedobjectList;
safeStringUMap<shared_ptr<hkreferencedobject>> hkreferencedobjectList_E;

void hkreferencedobject::regis(string id, bool isEdited)
{
	isEdited ? hkreferencedobjectList_E[id] = shared_from_this() : hkreferencedobjectList[id] = shared_from_this();
	ID = id;
}

namespace referenceobject
{
	string key = "a";
	string classname = "hkReferencedObject";
	string signature = "";
}

string hkreferencedobject::getClassCode()
{
	return referenceobject::key;
}
