#include <boost\algorithm\string.hpp>
#include "hkbboneindexarray.h"

using namespace std;

namespace boneindexarray
{
	string key = "bq";
	string classname = "hkbBoneIndexArray";
	string signature = "0xaa8619";
}

hkbboneindexarray::hkbboneindexarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + boneindexarray::key + to_string(functionlayer) + ">";

	if (Error) return;		

	if (!IsExist[id])
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
	else
	{
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare, true);

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
}

void hkbboneindexarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBoneIndexArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; i++)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					referencingIDs[variablebindingset].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbBoneIndexArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBoneIndexArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbboneindexarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBoneIndexArray(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; i++)
		{
			line = FunctionLineEdited[id][i];

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

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBoneIndexArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
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
			cout << "Comparing hkbBoneIndexArray(newID: " << id << ") with hkbBoneIndexArray(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		vecstr storeline = FunctionLineTemp[tempid];

		// stage 3
		int curline = 1;
		vecstr newstoreline;
		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			if (storeline[curline].find(newline[i], 0) != string::npos) // store function body
			{
				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				newstoreline.push_back(newline[i]);
			}
		}

		FunctionLineNew[tempid] = newstoreline;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBoneIndexArray(newID: " << id << ") with hkbBoneIndexArray(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbBoneIndexArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbboneindexarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBoneIndexArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

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

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbClipGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBoneIndexArray(ID: " << id << ") is complete!" << endl;
	}
}

string hkbboneindexarray::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbboneindexarray::IsBindingNull()
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

string hkbboneindexarray::GetAddress()
{
	return address;
}

bool hkbboneindexarray::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbboneindexarray>> hkbboneindexarrayList;
safeStringUMap<shared_ptr<hkbboneindexarray>> hkbboneindexarrayList_E;

void hkbboneindexarray::regis(string id, bool isEdited)
{
	isEdited ? hkbboneindexarrayList_E[id] = shared_from_this() : hkbboneindexarrayList[id] = shared_from_this();
	ID = id;
}

void hkbboneindexarray::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
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
				usize numelement;

				if (readEleParam("boneIndices", line, numelement))
				{
					boneIndices.reserve(numelement);
					++type;
				}

				break;
			}
			case 2:
			{
				if (line.find("</hkparam>") != string::npos)
				{
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

					boneIndices.push_back(stoi(each));
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << boneindexarray::classname + " (ID: " << ID << ") is complete!" << endl;
	}	
}

void hkbboneindexarray::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + boneindexarray::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) variableBindingSet->connect(filepath, curadd, functionlayer + 1, false, graphroot);
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
				hkbboneindexarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbboneindexarrayList_E[ID] = protect;
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
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare, true);

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
				hkbboneindexarrayList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbboneindexarrayList_E[ID] = protect;
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

string hkbboneindexarray::getClassCode()
{
	return boneindexarray::key;
}

void hkbboneindexarray::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(50);
	output.reserve(50);
	usize base = 2;
	hkbboneindexarray* ctrpart = static_cast<hkbboneindexarray*>(counterpart.get());

	output.push_back(openObject(base, ID, boneindexarray::classname, boneindexarray::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);

	usize size = boneIndices.size();

	if (size == 0)
	{
		output.push_back(openParam(base, "boneIndices", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "boneIndices", size));		// 2

	for (usize i = 0; i < size; ++i)
	{
		// deleted existing data
		if (i >= ctrpart->boneIndices.size())
		{
			nemesis::try_open(open, isEdited, output);

			while (i < size)
			{
				storeline.push_back(autoVoid(base, boneIndices[i]));
				++i;
				output.push_back("");	// 1 spaces
			}
		}
		// both exist
		else
		{
			voidMatch(boneIndices[i], ctrpart->boneIndices[i], output, storeline, base, false, open, isEdited);
		}
	}

	// newly created data
	if (size < ctrpart->boneIndices.size())
	{
		nemesis::try_open(open, isEdited, output);
		usize i = size;
		size = ctrpart->boneIndices.size();

		while (i < size)
		{
			string line;

			do
			{
				line.append(to_string(ctrpart->boneIndices[i]) + " ");
				++i;
			} while (i < size && i % 16 != 0);

			line.pop_back();
			output.push_back(autoVoid(base, line));
		}
	}

	if (size > 0)
	{
		if (boneIndices.size() == 0)
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
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", boneindexarray::classname, output, isEdited);
}

void hkbboneindexarray::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(50);

	output.push_back(openObject(base, ID, boneindexarray::classname, boneindexarray::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));

	usize size = boneIndices.size();
	usize counter = 0;

	if (size == 0)
	{
		output.push_back(openParam(base, "boneIndices", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "boneIndices", size));		// 2

	while (counter < size)
	{
		string line;

		do
		{
			line.append(to_string(boneIndices[counter]) + " ");
			++counter;
		} while (counter < size && counter % 16 != 0);

		line.pop_back();
		output.push_back(autoVoid(base, line));
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, boneindexarray::classname, output, true);
}

void hkbboneindexarray::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbboneindexarray::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbBoneIndexArrayExport(string id)
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
		cout << "ERROR: Edit hkbBoneIndexArray Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	//stage 2 reading and identifying edits
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
		cout << "ERROR: Edit hkbBoneIndexArray Output Not Found (ID: " << id << ")" << endl;
		Error = true;
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

			fwrite << "			</hkparam>\n";
			fwrite << "		</hkobject>\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbBoneIndexArray Output Not Found (File: " << filename << ")" << endl;
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