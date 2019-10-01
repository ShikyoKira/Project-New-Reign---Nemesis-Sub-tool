#include <boost\thread.hpp>
#include "hkbeventdrivenmodifier.h"

using namespace std;

extern vector<usize> datapacktracker;

namespace eventdrivenmodifier
{
	string key = "aa";
	string classname = "hkbEventDrivenModifier";
	string signature = "0x7ed3f44e";
}

hkbeventdrivenmodifier::hkbeventdrivenmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + eventdrivenmodifier::key + to_string(functionlayer) + ">";

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

void hkbeventdrivenmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventDrivenModifier (ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"modifier\">", 0) != string::npos)
			{
				s_modifier = line.substr(28, line.find("</hkparam>") - 28);

				if (s_modifier != "null")
				{
					referencingIDs[s_modifier].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbEventDrivenModifier Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbEventDrivenModifier (ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventdrivenmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEventDrivenModifier (ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"modifier\">", 0) != string::npos)
			{
				s_modifier = line.substr(28, line.find("</hkparam>") - 28);

				if (s_modifier != "null")
				{
					if (!exchangeID[s_modifier].empty())
					{
						int tempint = line.find(s_modifier);
						s_modifier = exchangeID[s_modifier];
						line.replace(tempint, line.find("</hkparam>") - tempint, s_modifier);
					}

					parent[s_modifier] = id;
					referencingIDs[s_modifier].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbEventDrivenModifier Inputfile (File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbEventDrivenModifier (newID: " << id << ") with hkbEventDrivenModifier (oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (s_modifier != "null")
		{
			referencingIDs[s_modifier].pop_back();
			referencingIDs[s_modifier].push_back(tempid);
			parent[s_modifier] = tempid;
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
			cout << "Comparing hkbEventDrivenModifier (newID: " << id << ") with hkbEventDrivenModifier (oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbEventDrivenModifier (ID: " << id << ") is complete!" << endl;
	}
}

void hkbeventdrivenmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbEventDrivenModifier (ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"modifier\">", 0) != string::npos)
			{
				s_modifier = line.substr(28, line.find("</hkparam>") - 28);

				if (s_modifier != "null")
				{
					if (!exchangeID[s_modifier].empty())
					{
						s_modifier = exchangeID[s_modifier];
					}

					parent[s_modifier] = id;
				}

				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbEventDrivenModifier Inputfile (ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbEventDrivenModifier (ID: " << id << ") is complete!" << endl;
	}
}

string hkbeventdrivenmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbeventdrivenmodifier::IsBindingNull()
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

string hkbeventdrivenmodifier::GetModifier()
{
	return "#" + boost::regex_replace(string(s_modifier), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

string hkbeventdrivenmodifier::GetAddress()
{
	return address;
}

bool hkbeventdrivenmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbeventdrivenmodifier>> hkbeventdrivenmodifierList;
safeStringUMap<shared_ptr<hkbeventdrivenmodifier>> hkbeventdrivenmodifierList_E;

void hkbeventdrivenmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbeventdrivenmodifierList_E[id] = shared_from_this() : hkbeventdrivenmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbeventdrivenmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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

					if (readParam("modifier", line, output))
					{
						modifier = (isEdited ? hkbmodifierList_E : hkbmodifierList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					if (readParam("activateEventId", line, activateEventId)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("deactivateEventId", line, deactivateEventId)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("activeByDefault", line, activeByDefault)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << eventdrivenmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbeventdrivenmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + eventdrivenmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	activateEventId.connectEventInfo(ID, graphroot);
	deactivateEventId.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (modifier) threadedNextNode(modifier, filepath, curadd, functionlayer, graphroot);
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
				hkbeventdrivenmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbeventdrivenmodifierList_E[ID] = protect;
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
				hkbeventdrivenmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbeventdrivenmodifierList_E[ID] = protect;
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

string hkbeventdrivenmodifier::getClassCode()
{
	return eventdrivenmodifier::key;
}

void hkbeventdrivenmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(10);
	usize base = 2;
	hkbeventdrivenmodifier* ctrpart = static_cast<hkbeventdrivenmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, eventdrivenmodifier::classname, eventdrivenmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("modifier", modifier, ctrpart->modifier, output, storeline, base, false, open, isEdited);
	paramMatch("activateEventId", activateEventId, ctrpart->activateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("deactivateEventId", deactivateEventId, ctrpart->deactivateEventId, output, storeline, base, false, open, isEdited);
	paramMatch("activeByDefault", activeByDefault, ctrpart->activeByDefault, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", eventdrivenmodifier::classname, output, isEdited);
}

void hkbeventdrivenmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(10);

	output.push_back(openObject(base, ID, eventdrivenmodifier::classname, eventdrivenmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "modifier", modifier));
	output.push_back(autoParam(base, "activateEventId", activateEventId));
	output.push_back(autoParam(base, "deactivateEventId", deactivateEventId));
	output.push_back(autoParam(base, "activeByDefault", activeByDefault));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, eventdrivenmodifier::classname, output, true);
}

void hkbeventdrivenmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (modifier) hkb_parent[modifier] = shared_from_this();
}

void hkbeventdrivenmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (modifier)
	{
		parentRefresh();
		modifier->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbeventdrivenmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbEventDrivenModifierExport(string id)
{
	//stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	usize tracker = 0;

	{
		int i_id = stoi(id.substr(1));
		for (auto& dataid : datapacktracker)
		{
			if (i_id > int(dataid)) ++tracker;
		}
	}

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
		cout << "ERROR: Edit hkbEventDrivenModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	for (unsigned int j = 0; j < storeline2.size(); j++) // changing newID to modCode ID
	{
		if (storeline2[j].find("#", 0) != string::npos)
		{
			usize tempint = 0;
			usize position = 0;
			usize size = count(storeline2[j].begin(), storeline2[j].end(), '#');

			for (unsigned int i = 0; i < size; i++)
			{
				position = storeline2[j].find("#", tempint);
				tempint = storeline2[j].find("#", position + 1);
				string tempID;

				if (tempint == -1)
				{
					string templine;

					if (storeline2[j].find("signature", 0) != string::npos)
					{
						templine = storeline2[j].substr(0, storeline2[j].find("class"));
					}
					else
					{
						templine = storeline2[j].substr(position, tempint - position - 1);
					}

					tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
				}
				else
				{
					tempID = storeline2[j].substr(position, tempint - position - 1);
				}

				int tempLength = tempID.length();
				string strID = tempID.substr(1, tempLength - 1);
				int intID = stoi(strID);

				if (intID > 10000)
				{
					int position2 = storeline2[j].find(tempID);
					string modID;

					if (!newID[tempID].empty())
					{
						modID = newID[tempID];
					}
					else
					{
						modID = "#" + modcode + "$" + to_string(functioncount);
						newID[tempID] = modID;
						functioncount++;
					}

					storeline2[j].replace(position2, tempLength, modID);
				}
			}

			if (((storeline2[j].find("<hkparam name=\"activateEventId\">", 0) != string::npos) || (storeline2[j].find("<hkparam name=\"deactivateEventId\">", 0) != string::npos)) && (storeline2[j].find("<hkparam name=\"activateEventId\">-1</hkparam>", 0) == string::npos) && (storeline2[j].find("<hkparam name=\"deactivateEventId\">-1</hkparam>", 0) == string::npos))
			{
				usize eventpos = storeline2[j].find("Id\">") + 4;
				string eventid = storeline2[j].substr(eventpos, storeline2[j].find("</hkparam>", eventpos) - eventpos);

				if (eventid != line && eventID[tracker][eventid].length() != 0 && stoi(eventid) > int(eventCount[tracker]))
				{
					storeline2[j].replace(eventpos, eventid.length(), "$eventID[" + eventID[tracker][eventid] + "]$");
				}
			}
		}
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
			cout << "ERROR: Edit hkbEventDrivenModifier Output Not Found (File: " << filename << ")" << endl;
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