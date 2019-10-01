#include <boost\thread.hpp>
#include "hkbbehaviorgraph.h"

using namespace std;

boost::atomic_flag tracklock = BOOST_ATOMIC_FLAG_INIT;

namespace behaviorgraph
{
	string key = "g";
	string classname = "hkbBehaviorGraph";
	string signature = "0xb1218f86";
}

hkbbehaviorgraph::hkbbehaviorgraph(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + behaviorgraph::key + to_string(functionlayer) + ">";

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

void hkbbehaviorgraph::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraph(" << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"rootGenerator\">", 0) != string::npos)
			{
				generator = line.substr(33, line.find("</hkparam>") - 33);
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"data\">", 0) != string::npos)
			{
				s_data = line.substr(24, line.find("</hkparam>") - 24);
				referencingIDs[s_data].push_back(id);
			}
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraph Inputfile(" << id << ", " << filepath << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbBehaviorGraph(" << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraph::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbBehaviorGraph(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"rootGenerator\">", 0) != string::npos)
			{
				generator = line.substr(33, line.find("</hkparam>") - 33);

				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					generator = exchangeID[generator];
					line.replace(tempint, line.find("</hkparam>") - tempint, generator);
				}

				parent[generator] = id;
				referencingIDs[generator].push_back(id);
			}
			else if (line.find("<hkparam name=\"data\">", 0) != string::npos)
			{
				s_data = line.substr(24, line.find("</hkparam>") - 24);

				if (!exchangeID[s_data].empty())
				{
					int tempint = line.find(s_data);
					s_data = exchangeID[s_data];
					line.replace(tempint, line.find("</hkparam>") - tempint, s_data);
				}

				parent[s_data] = id;
				referencingIDs[s_data].push_back(id);
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbBehaviorGraph Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbBehaviorGraph(newID: " << id << ") with hkbBehaviorGraph(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		referencingIDs[generator].pop_back();
		referencingIDs[generator].push_back(tempid);
		parent[generator] = tempid;

		referencingIDs[s_data].pop_back();
		referencingIDs[s_data].push_back(tempid);
		parent[s_data] = tempid;

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
			cout << "Comparing hkbBehaviorGraph(newID: " << id << ") with hkbBehaviorGraph(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbBehaviorGraph(ID: " << id << ") is complete!" << endl;
	}
}

void hkbbehaviorgraph::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbBehaviorGraph(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"rootGenerator\">", 0) != string::npos)
			{
				generator = line.substr(33, line.find("</hkparam>") - 33);

				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}

				parent[generator] = id;
			}
			else if (line.find("<hkparam name=\"data\">", 0) != string::npos)
			{
				s_data = line.substr(24, line.find("</hkparam>") - 24);

				if (!exchangeID[s_data].empty())
				{
					s_data = exchangeID[s_data];
				}

				parent[s_data] = id;
				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbBehaviorGraph Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbBehaviorGraph(ID: " << id << ") is complete!" << endl;
	}
}

string hkbbehaviorgraph::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbbehaviorgraph::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbbehaviorgraph::IsBindingNull()
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

string hkbbehaviorgraph::GetData()
{
	return s_data;
}

string hkbbehaviorgraph::GetAddress()
{
	return address;
}

bool hkbbehaviorgraph::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbbehaviorgraph>> hkbbehaviorgraphList;
safeStringUMap<shared_ptr<hkbbehaviorgraph>> hkbbehaviorgraphList_E;

void hkbbehaviorgraph::regis(string id, bool isEdited)
{
	isEdited ? hkbbehaviorgraphList_E[id] = shared_from_this() : hkbbehaviorgraphList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();

	while (tracklock.test_and_set(boost::memory_order_acquire));
	datapacktracker.push_back(stoi(id.substr(1)));
	tracklock.clear(boost::memory_order_release);

	ID = id;
}

void hkbbehaviorgraph::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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

					if (readParam("variableMode", line, output))
					{
						variableMode = output == "VARIABLE_MODE_DISCARD_WHEN_INACTIVE" ? VARIABLE_MODE_DISCARD_WHEN_INACTIVE : VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE;
					}

					break;
				}
				case 4:
				{
					string output;

					if (readParam("rootGenerator", line, output))
					{
						rootGenerator = (isEdited ? hkbgeneratorList_E : hkbgeneratorList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("data", line, output))
					{
						data = (isEdited ? hkbbehaviorgraphdataList_E : hkbbehaviorgraphdataList)[output];
						++type;
					}
				}
			}
		}
		if (line.find("<hkparam name=\"") != string::npos)
		{
			for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
				itr != boost::sregex_iterator(); ++itr)
			{
				string header = itr->str(1);

				if (header == "variableBindingSet") variableBindingSet = isEdited ? hkbvariablebindingsetList_E[itr->str(2)] : hkbvariablebindingsetList[itr->str(2)];
				else if (header == "userData") userData = stoi(itr->str(2));
				else if (header == "name") name = itr->str(2);
				else if (header == "variableMode") variableMode = itr->str(2) == "VARIABLE_MODE_DISCARD_WHEN_INACTIVE" ? VARIABLE_MODE_DISCARD_WHEN_INACTIVE :
					VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE;
				else if (header == "rootGenerator") rootGenerator = isEdited ? hkbgeneratorList_E[itr->str(2)] : hkbgeneratorList[itr->str(2)];
				else if (header == "data") data = isEdited ? hkbbehaviorgraphdataList_E[itr->str(2)] : hkbbehaviorgraphdataList[itr->str(2)];

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << behaviorgraph::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbbehaviorgraph::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + behaviorgraph::key + to_string(functionlayer) + ">";
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

			if (rootGenerator) threadedNextNode(rootGenerator, filepath, curadd, functionlayer, graphroot);

			if (data) threadedNextNode(data, filepath, curadd, functionlayer, graphroot);
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
				hkbbehaviorgraphList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);

				while (tracklock.test_and_set(boost::memory_order_acquire));
				vector<usize> newdatapacktracker;
				newdatapacktracker.reserve(datapacktracker.size() - 1);

				{
					usize tempIntID = stoi(ID.substr(1));

					for (auto& tracker : datapacktracker)
					{
						if (tracker != tempIntID) newdatapacktracker.push_back(tracker);
					}
				}

				datapacktracker = newdatapacktracker;
				tracklock.clear(boost::memory_order_release);

				ID = addressID[address];
				hkbbehaviorgraphList_E[ID] = protect;
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
				hkbbehaviorgraphList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbbehaviorgraphList_E[ID] = protect;
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

string hkbbehaviorgraph::getClassCode()
{
	return behaviorgraph::key;
}

void hkbbehaviorgraph::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(8);
	output.reserve(8);
	usize base = 2;
	hkbbehaviorgraph* ctrpart = static_cast<hkbbehaviorgraph*>(counterpart.get());

	output.push_back(openObject(base, ID, behaviorgraph::classname, behaviorgraph::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("variableMode", getVariableMode(), ctrpart->getVariableMode(), output, storeline, base, false, open, isEdited);
	paramMatch("rootGenerator", rootGenerator, ctrpart->rootGenerator, output, storeline, base, false, open, isEdited);
	paramMatch("data", data, ctrpart->data, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", behaviorgraph::classname, output, isEdited);
}

void hkbbehaviorgraph::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(8);

	output.push_back(openObject(base, ID, behaviorgraph::classname, behaviorgraph::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "variableMode", getVariableMode()));
	output.push_back(autoParam(base, "rootGenerator", rootGenerator));
	output.push_back(autoParam(base, "data", data));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, behaviorgraph::classname, output, true);
}

void hkbbehaviorgraph::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (rootGenerator) hkb_parent[rootGenerator] = shared_from_this();
	if (data) hkb_parent[data] = shared_from_this();
}

void hkbbehaviorgraph::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, this);
	}

	if (rootGenerator)
	{
		parentRefresh();
		rootGenerator->connect(filepath, address, functionlayer, true, this);
	}

	if (data)
	{
		parentRefresh();
		data->connect(filepath, address, functionlayer, true, this);
	}
}

string hkbbehaviorgraph::getVariableMode()
{
	switch (variableMode)
	{
		case VARIABLE_MODE_DISCARD_WHEN_INACTIVE: return "VARIABLE_MODE_DISCARD_WHEN_INACTIVE";
		case VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE: return "VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE";
		default: return "VARIABLE_MODE_DISCARD_WHEN_INACTIVE";
	}
}

void hkbbehaviorgraph::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, this);
}

void hkbBehaviorGraphExport(string id)
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
		cout << "ERROR: Edit hkbBehaviorGraph Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

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
			cout << "ERROR: Edit hkbBehaviorGraph Output Not Found (File: " << filename << ")" << endl;
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