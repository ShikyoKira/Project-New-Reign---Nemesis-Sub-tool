#include <boost\thread.hpp>
#include "bsistatetagginggenerator.h"

using namespace std;

namespace istatetagginggenerator
{
	string key = "v";
	string classname = "BSiStateTaggingGenerator";
	string signature = "0xf0826fc1";
}

bsistatetagginggenerator::bsistatetagginggenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + istatetagginggenerator::key + to_string(functionlayer) + ">";

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

void bsistatetagginggenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSiStateTaggingGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pDefaultGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);
				referencingIDs[generator].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: BSiStateTaggingGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSiStateTaggingGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsistatetagginggenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSiStateTaggingGenerator(ID: " << id << ") has been initialized!" << endl;
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

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSiStateTaggingGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSiStateTaggingGenerator(newID: " << id << ") with BSiStateTaggingGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
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
			cout << "Comparing BSiStateTaggingGenerator(newID: " << id << ") with BSiStateTaggingGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSiStateTaggingGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsistatetagginggenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSiStateTaggingGenerator(ID: " << id << ") has been initialized!" << endl;
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

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSiStateTaggingGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSiStateTaggingGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bsistatetagginggenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string bsistatetagginggenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsistatetagginggenerator::IsBindingNull()
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

string bsistatetagginggenerator::GetAddress()
{
	return address;
}

bool bsistatetagginggenerator::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bsistatetagginggenerator>> bsistatetagginggeneratorList;
safeStringUMap<shared_ptr<bsistatetagginggenerator>> bsistatetagginggeneratorList_E;

void bsistatetagginggenerator::regis(string id, bool isEdited)
{
	isEdited ? bsistatetagginggeneratorList_E[id] = shared_from_this() : bsistatetagginggeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void bsistatetagginggenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("iStateToSetAs", line, iStateToSetAs)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("iPriority", line, iPriority)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << istatetagginggenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsistatetagginggenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + istatetagginggenerator::key + to_string(functionlayer) + ">";
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

			if (pDefaultGenerator) threadedNextNode(pDefaultGenerator, filepath, curadd, functionlayer, graphroot);
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
				bsistatetagginggeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsistatetagginggeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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
				bsistatetagginggeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsistatetagginggeneratorList_E[ID] = protect;
				hkbgeneratorList_E[ID] = protect;
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

string bsistatetagginggenerator::getClassCode()
{
	return istatetagginggenerator::key;
}

void bsistatetagginggenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(8);
	output.reserve(8);
	usize base = 2;
	bsistatetagginggenerator* ctrpart = static_cast<bsistatetagginggenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, istatetagginggenerator::classname, istatetagginggenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("pDefaultGenerator", pDefaultGenerator, ctrpart->pDefaultGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("iStateToSetAs", iStateToSetAs, ctrpart->iStateToSetAs, output, storeline, base, false, open, isEdited);
	paramMatch("iPriority", iPriority, ctrpart->iPriority, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", istatetagginggenerator::classname, output, isEdited);
}

void bsistatetagginggenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(8);

	output.push_back(openObject(base, ID, istatetagginggenerator::classname, istatetagginggenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "pDefaultGenerator", pDefaultGenerator));
	output.push_back(autoParam(base, "iStateToSetAs", iStateToSetAs));
	output.push_back(autoParam(base, "iPriority", iPriority));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, istatetagginggenerator::classname, output, true);
}

void bsistatetagginggenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pDefaultGenerator) hkb_parent[pDefaultGenerator] = shared_from_this();
}

void bsistatetagginggenerator::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

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
}

void bsistatetagginggenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSiStateTaggingGeneratorExport(string id)
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
		cout << "ERROR: Edit BSiStateTaggingGenerator Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit BSiStateTaggingGenerator Output Not Found (File: " << filename << ")" << endl;
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