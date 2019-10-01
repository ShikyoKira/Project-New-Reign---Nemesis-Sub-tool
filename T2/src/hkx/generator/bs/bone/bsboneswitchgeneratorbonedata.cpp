#include <boost\thread.hpp>
#include "bsboneswitchgeneratorbonedata.h"

using namespace std;

namespace boneswitchgeneratorbonedata
{
	string key = "ct";
	string classname = "BSBoneSwitchGeneratorBoneData";
	string signature = "0xc1215be6";
}

bsboneswitchgeneratorbonedata::bsboneswitchgeneratorbonedata(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + boneswitchgeneratorbonedata::key + to_string(functionlayer) + ">";

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
	else
	{
		return;
	}
}

void bsboneswitchgeneratorbonedata::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSBoneSwitchGeneratorBoneData(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
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
			else if (line.find("<hkparam name=\"pGenerator\">", 0) != string::npos)
			{
				generator = line.substr(30, line.find("</hkparam>") - 30);
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"spBoneWeight\">", 0) != string::npos)
			{
				boneweights = line.substr(32, line.find("</hkparam>") - 32);
				referencingIDs[boneweights].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: BSBoneSwitchGeneratorBoneData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSBoneSwitchGeneratorBoneData(ID: " << id << ") is complete!" << endl;
	}
}

void bsboneswitchgeneratorbonedata::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSBoneSwitchGeneratorBoneData(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pGenerator\">", 0) != string::npos)
			{
				generator = line.substr(30, line.find("</hkparam>") - 30);

				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					generator = exchangeID[generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, generator);
				}

				parent[generator] = id;
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"spBoneWeight\">", 0) != string::npos)
			{
				boneweights = line.substr(32, line.find("</hkparam>") - 32);

				if (!exchangeID[boneweights].empty())
				{
					int tempint = line.find(boneweights);
					boneweights = exchangeID[boneweights];
					line.replace(tempint, line.find("</hkparam>") - tempint, boneweights);
				}

				parent[boneweights] = id;
				referencingIDs[boneweights].push_back(id);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSBoneSwitchGeneratorBoneData Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if ((addressID[address] != "") && (!IsForeign[parent[id]])) // is this new function or old for non generator
	{
		IsForeign[id] = false;		
		string tempid;
		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSBoneSwitchGeneratorBoneData(newID: " << id << ") with BSBoneSwitchGeneratorBoneData(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		referencingIDs[boneweights].pop_back();
		referencingIDs[boneweights].push_back(tempid);
		parent[boneweights] = tempid;

		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);
		parent[generator] = tempid;

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
			cout << "Comparing BSBoneSwitchGeneratorBoneData(newID: " << id << ") with BSBoneSwitchGeneratorBoneData(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSBoneSwitchGeneratorBoneData(ID: " << id << ") is complete!" << endl;
	}
}

void bsboneswitchgeneratorbonedata::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSBoneSwitchGeneratorBoneData(ID: " << id << ") has been initialized!" << endl;
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
			}
			else if (line.find("<hkparam name=\"pGenerator\">", 0) != string::npos)
			{
				generator = line.substr(30, line.find("</hkparam>") - 30);

				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}

				parent[generator] = id;
			}
			else if (line.find("<hkparam name=\"spBoneWeight\">", 0) != string::npos)
			{
				boneweights = line.substr(32, line.find("</hkparam>") - 32);

				if (!exchangeID[boneweights].empty())
				{
					boneweights = exchangeID[boneweights];
				}

				parent[boneweights] = id;
				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSBoneSwitchGeneratorBoneData Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSBoneSwitchGeneratorBoneData(ID: " << id << ") is complete!" << endl;
	}
}

string bsboneswitchgeneratorbonedata::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string bsboneswitchgeneratorbonedata::GetBoneWeights()
{
	return "#" + boost::regex_replace(string(boneweights), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsboneswitchgeneratorbonedata::IsBoneWeightsNull()
{
	if (boneweights.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bsboneswitchgeneratorbonedata::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsboneswitchgeneratorbonedata::IsBindingNull()
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

string bsboneswitchgeneratorbonedata::GetAddress()
{
	return address;
}

bool bsboneswitchgeneratorbonedata::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bsboneswitchgeneratorbonedata>> bsboneswitchgeneratorbonedataList;
safeStringUMap<shared_ptr<bsboneswitchgeneratorbonedata>> bsboneswitchgeneratorbonedataList_E;

void bsboneswitchgeneratorbonedata::regis(string id, bool isEdited)
{
	isEdited ? bsboneswitchgeneratorbonedataList_E[id] = shared_from_this() : bsboneswitchgeneratorbonedataList[id] = shared_from_this();
	ID = id;
}

void bsboneswitchgeneratorbonedata::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos)
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
					string output;

					if (readParam("pGenerator", line, output))
					{
						pGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 2:
				{
					string output;

					if (readParam("spBoneWeight", line, output))
					{
						spBoneWeight = (isEdited ? hkbboneweightarrayList_E : hkbboneweightarrayList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << boneswitchgeneratorbonedata::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsboneswitchgeneratorbonedata::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + boneswitchgeneratorbonedata::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (pGenerator) threadedNextNode(pGenerator, filepath, curadd, functionlayer, graphroot);

			if (spBoneWeight) threadedNextNode(spBoneWeight, filepath, curadd, functionlayer, graphroot);
		}
		else
		{
			// existed
			if (addressID.find(address) != addressID.end() && IsForeign.find(parent[ID]) == IsForeign.end())
			{
				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				bsboneswitchgeneratorbonedataList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsboneswitchgeneratorbonedataList_E[ID] = protect;
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
				bsboneswitchgeneratorbonedataList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsboneswitchgeneratorbonedataList_E[ID] = protect;
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

string bsboneswitchgeneratorbonedata::getClassCode()
{
	return boneswitchgeneratorbonedata::key;
}

void bsboneswitchgeneratorbonedata::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(5);
	output.reserve(5);
	usize base = 2;
	bsboneswitchgeneratorbonedata* ctrpart = static_cast<bsboneswitchgeneratorbonedata*>(counterpart.get());

	output.push_back(openObject(base, ID, boneswitchgeneratorbonedata::classname, boneswitchgeneratorbonedata::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("pGenerator", pGenerator, ctrpart->pGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("spBoneWeight", spBoneWeight, ctrpart->spBoneWeight, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", boneswitchgeneratorbonedata::classname, output, isEdited);
}

void bsboneswitchgeneratorbonedata::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(5);

	output.push_back(openObject(base, ID, boneswitchgeneratorbonedata::classname, boneswitchgeneratorbonedata::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "pGenerator", pGenerator));
	output.push_back(autoParam(base, "spBoneWeight", spBoneWeight));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, boneswitchgeneratorbonedata::classname, output, true);
}

void bsboneswitchgeneratorbonedata::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pGenerator) hkb_parent[pGenerator] = shared_from_this();
	if (spBoneWeight) hkb_parent[spBoneWeight] = shared_from_this();
}

void bsboneswitchgeneratorbonedata::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pGenerator)
	{
		parentRefresh();
		pGenerator->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (spBoneWeight)
	{
		parentRefresh();
		spBoneWeight->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bsboneswitchgeneratorbonedata::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSBoneSwitchGeneratorBoneDataExport(string id)
{
	//stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find(storeline1[curline], 0) != string::npos) && (line.length() == storeline1[curline].length()))
			{
				if (open)
				{
					closepoint = curline;

					if (closepoint != openpoint)
					{
						storeline2.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							storeline2.push_back(storeline1[j]);
						}
					}

					storeline2.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					storeline2.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					open = true;
				}

				IsEdited = true;
			}

			storeline2.push_back(line);
			curline++;
		}
	}
	else
	{
		cout << "ERROR: Edit BSBoneSwitchGeneratorBoneData Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(stoi(id.substr(1)), storeline2);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream output(filename);

		if (output.is_open())
		{
			FunctionWriter fwrite(&output);

			for (unsigned int i = 0; i < storeline2.size(); i++)
			{
				fwrite << storeline2[i] << "\n";
			}

			output.close();
		}
		else
		{
			cout << "ERROR: Edit BSBoneSwitchGeneratorBoneData Output Not Found (File: " << filename << ")" << endl;
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