#include <boost\thread.hpp>
#include "bsoffsetanimationgenerator.h"

using namespace std;

namespace offsetanimationgenerator
{
	string key = "ci";
	string classname = "BSOffsetAnimationGenerator";
	string signature = "0xb8571122";
}

bsoffsetanimationgenerator::bsoffsetanimationgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + offsetanimationgenerator::key + to_string(functionlayer) + ">";

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

void bsoffsetanimationgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSOffsetAnimationGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOffsetClipGenerator\">", 0) != string::npos)
			{
				clipgenerator = line.substr(40, line.find("</hkparam>") - 40);
				referencingIDs[clipgenerator].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: BSOffsetAnimationGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSOffsetAnimationGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsoffsetanimationgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSOffsetAnimationGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOffsetClipGenerator\">", 0) != string::npos)
			{
				clipgenerator = line.substr(40, line.find("</hkparam>") - 40);

				if (!exchangeID[clipgenerator].empty())
				{
					int tempint = line.find(clipgenerator);
					clipgenerator = exchangeID[clipgenerator];
					line.replace(tempint, line.find("</hkparam>") - tempint, clipgenerator);
				}

				if (clipgenerator != "null")
				{
					parent[clipgenerator] = id;
					referencingIDs[clipgenerator].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSOffsetAnimationGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSOffsetAnimationGenerator(newID: " << id << ") with BSOffsetAnimationGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (clipgenerator != "null")
		{
			referencingIDs[clipgenerator].pop_back();
			referencingIDs[clipgenerator].push_back(tempid);
			parent[clipgenerator] = tempid;
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
		}

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
			cout << "Comparing BSOffsetAnimationGenerator(newID: " << id << ") with BSOffsetAnimationGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSOffsetAnimationGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bsoffsetanimationgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSOffsetAnimationGenerator(ID: " << id << ") has been initialized!" << endl;
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
			}
			else if (line.find("<hkparam name=\"pOffsetClipGenerator\">", 0) != string::npos)
			{
				clipgenerator = line.substr(40, line.find("</hkparam>") - 40);

				if (!exchangeID[clipgenerator].empty())
				{
					clipgenerator = exchangeID[clipgenerator];
				}

				if (clipgenerator != "null")
				{
					parent[clipgenerator] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSOffsetAnimationGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSOffsetAnimationGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bsoffsetanimationgenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string bsoffsetanimationgenerator::GetClipGenerator()
{
	return "#" + boost::regex_replace(string(clipgenerator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsoffsetanimationgenerator::IsClipNull()
{
	if (clipgenerator.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bsoffsetanimationgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsoffsetanimationgenerator::IsBindingNull()
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

string bsoffsetanimationgenerator::GetAddress()
{
	return address;
}

bool bsoffsetanimationgenerator::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bsoffsetanimationgenerator>> bsoffsetanimationgeneratorList;
safeStringUMap<shared_ptr<bsoffsetanimationgenerator>> bsoffsetanimationgeneratorList_E;

void bsoffsetanimationgenerator::regis(string id, bool isEdited)
{
	isEdited ? bsoffsetanimationgeneratorList_E[id] = shared_from_this() : bsoffsetanimationgeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void bsoffsetanimationgenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("pOffsetClipGenerator", line, output))
					{
						pOffsetClipGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					if (readParam("fOffsetVariable", line, fOffsetVariable)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("fOffsetRangeStart", line, fOffsetRangeStart)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("fOffsetRangeEnd", line, fOffsetRangeEnd)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << offsetanimationgenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsoffsetanimationgenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + offsetanimationgenerator::key + to_string(functionlayer) + ">";
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

			if (num_thread < boost::thread::hardware_concurrency())
			{
				boost::thread_group multithreads;

				if (pDefaultGenerator) multithreads.create_thread(boost::bind(&bsoffsetanimationgenerator::threadedNextNode, this, pDefaultGenerator, filepath, curadd + "0",
					functionlayer, graphroot));

				if (pOffsetClipGenerator) multithreads.create_thread(boost::bind(&bsoffsetanimationgenerator::threadedNextNode, this, pOffsetClipGenerator, filepath,
					curadd + "1", functionlayer, graphroot));

				num_thread += multithreads.size();
				multithreads.join_all();
				num_thread -= multithreads.size();
			}
			else
			{
				if (pDefaultGenerator) threadedNextNode(pDefaultGenerator, filepath, curadd + "0", functionlayer, graphroot);

				if (pOffsetClipGenerator) threadedNextNode(pOffsetClipGenerator, filepath, curadd + "1", functionlayer, graphroot);
			}
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
				bsoffsetanimationgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsoffsetanimationgeneratorList_E[ID] = protect;
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
				bsoffsetanimationgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsoffsetanimationgeneratorList_E[ID] = protect;
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

string bsoffsetanimationgenerator::getClassCode()
{
	return offsetanimationgenerator::key;
}

void bsoffsetanimationgenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(10);
	usize base = 2;
	bsoffsetanimationgenerator* ctrpart = static_cast<bsoffsetanimationgenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, offsetanimationgenerator::classname, offsetanimationgenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("pDefaultGenerator", pDefaultGenerator, ctrpart->pDefaultGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("pOffsetClipGenerator", pOffsetClipGenerator, ctrpart->pOffsetClipGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("fOffsetVariable", fOffsetVariable, ctrpart->fOffsetVariable, output, storeline, base, false, open, isEdited);
	paramMatch("fOffsetRangeStart", fOffsetRangeStart, ctrpart->fOffsetRangeStart, output, storeline, base, false, open, isEdited);
	paramMatch("fOffsetRangeEnd", fOffsetRangeEnd, ctrpart->fOffsetRangeEnd, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", offsetanimationgenerator::classname, output, isEdited);
}

void bsoffsetanimationgenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, offsetanimationgenerator::classname, offsetanimationgenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "pDefaultGenerator", pDefaultGenerator));
	output.push_back(autoParam(base, "pOffsetClipGenerator", pOffsetClipGenerator));
	output.push_back(autoParam(base, "fOffsetVariable", fOffsetVariable));
	output.push_back(autoParam(base, "fOffsetRangeStart", fOffsetRangeStart));
	output.push_back(autoParam(base, "fOffsetRangeEnd", fOffsetRangeEnd));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, offsetanimationgenerator::classname, output, true);
}

void bsoffsetanimationgenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pDefaultGenerator) hkb_parent[pDefaultGenerator] = shared_from_this();
	if (pOffsetClipGenerator) hkb_parent[pOffsetClipGenerator] = shared_from_this();
}

void bsoffsetanimationgenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
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
		pDefaultGenerator->connect(filepath, isOld ? address + "0" : address, functionlayer, true, graphroot);
	}

	if (pOffsetClipGenerator)
	{
		parentRefresh();
		pOffsetClipGenerator->connect(filepath, isOld ? address + "1" : address, functionlayer, true, graphroot);
	}
}

void bsoffsetanimationgenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSOffsetAnimationGeneratorExport(string id)
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
		cout << "ERROR: Edit BSOffsetAnimationGenerator Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit BSOffsetAnimationGenerator Output Not Found (File: " << filename << ")" << endl;
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