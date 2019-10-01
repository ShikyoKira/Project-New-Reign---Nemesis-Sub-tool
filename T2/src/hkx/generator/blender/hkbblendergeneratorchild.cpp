#include <boost\thread.hpp>
#include "hkbblendergeneratorchild.h"

using namespace std;

namespace blendergeneratorchild
{
	string key = "h";
	string classname = "hkbBlenderGeneratorChild";
	string signature = "0xe2b384b0";
}

hkbblendergeneratorchild::hkbblendergeneratorchild(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + blendergeneratorchild::key + to_string(functionlayer) + ">";

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
}

void hkbblendergeneratorchild::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBlenderGeneratorChild(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"generator\">", 0) != string::npos)
			{
				s_generator = line.substr(29, line.find("</hkparam>") - 29);
				referencingIDs[s_generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweights = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweights != "null")
				{
					referencingIDs[boneweights].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbBlenderGeneratorChild Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBlenderGeneratorChild(ID: " << id << ") is complete!" << endl;
	}
}

void hkbblendergeneratorchild::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBlenderGeneratorChild(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"generator\">", 0) != string::npos)
			{
				s_generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[s_generator].empty())
				{
					int tempint = line.find(s_generator);
					s_generator = exchangeID[s_generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, s_generator);
				}

				if (s_generator != "null")
				{
					parent[s_generator] = id;
					referencingIDs[s_generator].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweights = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweights != "null")
				{
					if (!exchangeID[boneweights].empty())
					{
						int tempint = line.find(boneweights);
						boneweights = exchangeID[boneweights];
						line.replace(tempint, line.find("</hkparam>") - tempint, boneweights);
					}

					parent[boneweights] = id;
					referencingIDs[boneweights].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBlenderGeneratorChild Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// if (IsOldFunction(filepath, id, address)) // is this new function or old
	if (ID != ID) // pre-foreign, will go through IsOldFunction using generator
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
			cout << "Comparing hkbBlenderGeneratorChild(newID: " << id << ") with hkbBlenderGeneratorChild(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (s_generator != "null")
		{
			referencingIDs[s_generator].pop_back();
			referencingIDs[s_generator].push_back(tempid);
			parent[s_generator] = tempid;
		}

		if (boneweights != "null")
		{
			referencingIDs[boneweights].pop_back();
			referencingIDs[boneweights].push_back(tempid);
			parent[boneweights] = tempid;
		}

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
			cout << "Comparing hkbBlenderGeneratorChild(newID: " << id << ") with hkbBlenderGeneratorChild(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbBlenderGeneratorChild(ID: " << id << ") is complete!" << endl;
	}
}

void hkbblendergeneratorchild::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBlenderGeneratorChild(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"generator\">", 0) != string::npos)
			{
				s_generator = line.substr(29, line.find("</hkparam>") - 29);

				if (!exchangeID[s_generator].empty())
				{
					s_generator = exchangeID[s_generator];
				}

				parent[s_generator] = id;
			}
			else if (line.find("<hkparam name=\"boneWeights\">", 0) != string::npos)
			{
				boneweights = line.substr(31, line.find("</hkparam>") - 31);

				if (boneweights != "null")
				{
					if (!exchangeID[boneweights].empty())
					{
						boneweights = exchangeID[boneweights];
					}

					parent[boneweights] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbBlenderGeneratorChild Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if (s_generator.length() == 0)
	{
		system("pause");
	}
	else if (variablebindingset.length() == 0)
	{
		system("pause");
	}
	else if (boneweights.length() == 0)
	{
		system("pause");
	}

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBlenderGeneratorChild(ID: " << id << ") is complete!" << endl;
	}
}

string hkbblendergeneratorchild::NextGenerator()
{
	return "#" + boost::regex_replace(string(s_generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbblendergeneratorchild::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbblendergeneratorchild::IsBindingNull()
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

string hkbblendergeneratorchild::GetBoneWeights()
{
	return "#" + boost::regex_replace(string(boneweights), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbblendergeneratorchild::IsBoneWeightsNull()
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

string hkbblendergeneratorchild::GetAddress()
{
	return address;
}

bool hkbblendergeneratorchild::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbblendergeneratorchild>> hkbblendergeneratorchildList;
safeStringUMap<shared_ptr<hkbblendergeneratorchild>> hkbblendergeneratorchildList_E;

void hkbblendergeneratorchild::regis(string id, bool isEdited)
{
	isEdited ? hkbblendergeneratorchildList_E[id] = shared_from_this() : hkbblendergeneratorchildList[id] = shared_from_this();
	ID = id;
}

void hkbblendergeneratorchild::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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

					if (readParam("generator", line, output))
					{
						generator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 2:
				{
					string output;

					if (readParam("boneWeights", line, output))
					{
						boneWeights = (isEdited ? hkbboneweightarrayList_E : hkbboneweightarrayList)[output];
						++type;
					}

					break;
				}
				case 3:
				{
					if (readParam("weight", line, weight)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("worldFromModelWeight", line, worldFromModelWeight)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << blendergeneratorchild::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbblendergeneratorchild::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + blendergeneratorchild::key + to_string(functionlayer) + ">";
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

			if (generator) threadedNextNode(generator, filepath, curadd, functionlayer, graphroot);

			if (boneWeights) threadedNextNode(boneWeights, filepath, curadd, functionlayer, graphroot);
		}
		else
		{
			// existed
			// if (IsOldFunction(filepath, shared_from_this(), address)) // is this new function or old
			if (ID != ID) // pre-foreign, will go through IsOldFunction using generator
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbblendergeneratorchildList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbblendergeneratorchildList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				bool isOld = true;

				if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
				{
					isOld = false;
					address = preaddress;
				}

				IsForeign[ID] = true;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, isOld, graphroot);
			}
		}
	}
	else if (!compare)
	{
		CrossReferencing(shared_from_this(), address, functionlayer, compare);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
	{
		bool isOld = true;

		if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
		{
			isOld = false;
			address = preaddress;
		}

		nextNode(filepath, functionlayer, isOld, graphroot);
	}
}

string hkbblendergeneratorchild::getClassCode()
{
	return blendergeneratorchild::key;
}

void hkbblendergeneratorchild::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(6);
	output.reserve(6);
	usize base = 2;
	hkbblendergeneratorchild* ctrpart = static_cast<hkbblendergeneratorchild*>(counterpart.get());

	output.push_back(openObject(base, ID, blendergeneratorchild::classname, blendergeneratorchild::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("generator", generator, ctrpart->generator, output, storeline, base, false, open, isEdited);
	paramMatch("boneWeights", boneWeights, ctrpart->boneWeights, output, storeline, base, false, open, isEdited);
	paramMatch("weight", weight, ctrpart->weight, output, storeline, base, false, open, isEdited);
	paramMatch("worldFromModelWeight", worldFromModelWeight, ctrpart->worldFromModelWeight, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", blendergeneratorchild::classname, output, isEdited);
}

void hkbblendergeneratorchild::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(6);

	output.push_back(openObject(base, ID, blendergeneratorchild::classname, blendergeneratorchild::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "generator", generator));
	output.push_back(autoParam(base, "boneWeights", boneWeights));
	output.push_back(autoParam(base, "weight", weight));
	output.push_back(autoParam(base, "worldFromModelWeight", worldFromModelWeight));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, blendergeneratorchild::classname, output, true);
}

void hkbblendergeneratorchild::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (generator) hkb_parent[generator] = shared_from_this();
	if (boneWeights) hkb_parent[boneWeights] = shared_from_this();
}

void hkbblendergeneratorchild::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (generator)
	{
		if (isOld && IsExist.find(generator->ID) != IsExist.end() && IsForeign.find(generator->ID) == IsForeign.end())
		{
			auto add_ptr = addressID.find(address);

			if (add_ptr != addressID.end())
			{
				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbblendergeneratorchildList_E.erase(ID);
				editedBehavior.erase(ID);
				IsForeign.erase(ID);
				if (IsExist.find(ID) != IsExist.end()) IsExist.erase(ID);
				ID = add_ptr->second;
				hkbblendergeneratorchildList_E[ID] = protect;
				editedBehavior[ID] = protect;
			}
		}

		parentRefresh();
		generator->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (boneWeights)
	{
		parentRefresh();
		boneWeights->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbblendergeneratorchild::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbBlenderGeneratorChildExport(string id)
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
		cout << "ERROR: Edit hkbBlenderGeneratorChild Output Not Found (ID: " << id << ")" << endl;
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
		}
		else
		{
			cout << "ERROR: Edit hkbBlenderGeneratorChild Output Not Found (File: " << filename << ")" << endl;
			Error = true;
		}

		output.close();
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

void keepsake(string filepath, string id, string address, string variablebindingset, string generator, string boneweights, vecstr newline)
{
	string line;

	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			string tempaddress = addressChange[address];
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbBlenderGeneratorChild(newID: " << id << ") with hkbBlenderGeneratorChild(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
		}

		if (boneweights != "null")
		{
			referencingIDs[boneweights].pop_back();
			referencingIDs[boneweights].push_back(tempid);
		}

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
			cout << "Comparing hkbBlenderGeneratorChild(newID: " << id << ") with hkbBlenderGeneratorChild(oldID: " << tempid << ") is complete!" << endl;
		}
	}
}