#include <boost\thread.hpp>
#include "bsmodifyoncemodifier.h"

using namespace std;

namespace modifyoncemodifier
{
	string key = "ay";
	string classname = "BSModifyOnceModifier";
	string signature = "0x1e20a97a";
}

bsmodifyoncemodifier::bsmodifyoncemodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + modifyoncemodifier::key + to_string(functionlayer) + ">";

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

void bsmodifyoncemodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSModifyOnceModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOnActivateModifier\">", 0) != string::npos)
			{
				modifier1 = line.substr(39, line.find("</hkparam>") - 39);

				if (modifier1 != "null")
				{
					referencingIDs[modifier1].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"pOnDeactivateModifier\">", 0) != string::npos)
			{
				modifier2 = line.substr(41, line.find("</hkparam>") - 41);

				if (modifier2 != "null")
				{
					referencingIDs[modifier2].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: BSModifyOnceModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSModifyOnceModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bsmodifyoncemodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSModifyOnceModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOnActivateModifier\">", 0) != string::npos)
			{
				modifier1 = line.substr(39, line.find("</hkparam>") - 39);

				if (modifier1 != "null")
				{
					if (!exchangeID[modifier1].empty())
					{
						int tempint = line.find(modifier1);
						modifier1 = exchangeID[modifier1];
						line.replace(tempint, line.find("</hkparam>") - tempint, modifier1);
					}

					parent[modifier1] = id;
					referencingIDs[modifier1].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"pOnDeactivateModifier\">", 0) != string::npos)
			{
				modifier2 = line.substr(41, line.find("</hkparam>") - 41);

				if (modifier2 != "null")
				{
					if (!exchangeID[modifier2].empty())
					{
						int tempint = line.find(modifier2);
						modifier2 = exchangeID[modifier2];
						line.replace(tempint, line.find("</hkparam>") - tempint, modifier2);
					}

					parent[modifier2] = id;
					referencingIDs[modifier2].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: BSModifyOnceModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSModifyOnceModifier(newID: " << id << ") with BSModifyOnceModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (modifier1 != "null")
		{
			referencingIDs[modifier1].pop_back();
			referencingIDs[modifier1].push_back(tempid);
			parent[modifier1] = tempid;
		}

		if (modifier2 != "null")
		{
			referencingIDs[modifier2].pop_back();
			referencingIDs[modifier2].push_back(tempid);
			parent[modifier2] = tempid;
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
			cout << "Comparing BSModifyOnceModifier(newID: " << id << ") with BSModifyOnceModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSModifyOnceModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bsmodifyoncemodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSModifyOnceModifier(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"pOnActivateModifier\">", 0) != string::npos)
			{
				modifier1 = line.substr(39, line.find("</hkparam>") - 39);

				if (modifier1 != "null")
				{
					if (!exchangeID[modifier1].empty())
					{
						modifier1 = exchangeID[modifier1];
					}

					parent[modifier1] = id;
				}
			}
			else if (line.find("<hkparam name=\"pOnDeactivateModifier\">", 0) != string::npos)
			{
				modifier2 = line.substr(41, line.find("</hkparam>") - 41);

				if (modifier2 != "null")
				{
					if (!exchangeID[modifier2].empty())
					{
						modifier2 = exchangeID[modifier2];
					}

					parent[modifier2] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy BSModifyOnceModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSModifyOnceModifier(ID: " << id << ") is complete!" << endl;
	}
}

string bsmodifyoncemodifier::GetModifier(int number)
{
	if (number == 0)
	{
		return "#" + boost::regex_replace(string(modifier1), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
	}
	else
	{
		return "#" + boost::regex_replace(string(modifier2), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
	}
}

bool bsmodifyoncemodifier::IsModifierNull(int number)
{
	string tempmodifier;

	if (number == 0)
	{
		tempmodifier = modifier1;
	}
	else
	{
		tempmodifier = modifier2;
	}

	if (tempmodifier.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string bsmodifyoncemodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsmodifyoncemodifier::IsBindingNull()
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

string bsmodifyoncemodifier::GetAddress()
{
	return address;
}

bool bsmodifyoncemodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bsmodifyoncemodifier>> bsmodifyoncemodifierList;
safeStringUMap<shared_ptr<bsmodifyoncemodifier>> bsmodifyoncemodifierList_E;

void bsmodifyoncemodifier::regis(string id, bool isEdited)
{
	isEdited ? bsmodifyoncemodifierList_E[id] = shared_from_this() : bsmodifyoncemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsmodifyoncemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("enable", line, enable)) ++type;

					break;
				}
				case 4:
				{
					string output;

					if (readParam("pOnActivateModifier", line, output))
					{
						pOnActivateModifier = (isEdited ? hkbmodifierList_E : hkbmodifierList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					string output;

					if (readParam("pOnDeactivateModifier", line, output))
					{
						pOnDeactivateModifier = (isEdited ? hkbmodifierList_E : hkbmodifierList)[output];
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << modifyoncemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsmodifyoncemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + modifyoncemodifier::key + to_string(functionlayer) + ">";
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

			if (pOnActivateModifier) threadedNextNode(pOnActivateModifier, filepath, curadd + "0", functionlayer, graphroot);

			if (pOnDeactivateModifier) threadedNextNode(pOnDeactivateModifier, filepath, curadd + "1", functionlayer, graphroot);
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
				bsmodifyoncemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsmodifyoncemodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
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
			bsmodifyoncemodifierList_E.erase(ID);
			hkbmodifierList_E.erase(ID);
			editedBehavior.erase(ID);
			IsExist.erase(ID);
			ID = addressID[address];
			bsmodifyoncemodifierList_E[ID] = protect;
			hkbmodifierList_E[ID] = protect;
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

string bsmodifyoncemodifier::getClassCode()
{
	return modifyoncemodifier::key;
}

void bsmodifyoncemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(8);
	output.reserve(8);
	usize base = 2;
	bsmodifyoncemodifier* ctrpart = static_cast<bsmodifyoncemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, modifyoncemodifier::classname, modifyoncemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("pOnActivateModifier", pOnActivateModifier, ctrpart->pOnActivateModifier, output, storeline, base, false, open, isEdited);
	paramMatch("pOnDeactivateModifier", pOnDeactivateModifier, ctrpart->pOnDeactivateModifier, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", modifyoncemodifier::classname, output, isEdited);
}

void bsmodifyoncemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(8);

	output.push_back(openObject(base, ID, modifyoncemodifier::classname, modifyoncemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "pOnActivateModifier", pOnActivateModifier));
	output.push_back(autoParam(base, "pOnDeactivateModifier", pOnDeactivateModifier));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, modifyoncemodifier::classname, output, true);
}

void bsmodifyoncemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pOnActivateModifier) hkb_parent[pOnActivateModifier] = shared_from_this();
	if (pOnDeactivateModifier) hkb_parent[pOnDeactivateModifier] = shared_from_this();
}

void bsmodifyoncemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pOnActivateModifier)
	{
		parentRefresh();
		pOnActivateModifier->connect(filepath, isOld ? address + "0" : address, functionlayer, true, graphroot);
	}

	if (pOnDeactivateModifier)
	{
		parentRefresh();
		pOnDeactivateModifier->connect(filepath, isOld ? address + "1" : address, functionlayer, true, graphroot);
	}
}

void bsmodifyoncemodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSModifyOnceModifierExport(string id)
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
		cout << "ERROR: Edit BSModifyOnceModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit BSModifyOnceModifier Output Not Found (File: " << filename << ")" << endl;
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