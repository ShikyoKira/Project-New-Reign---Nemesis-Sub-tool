#include <boost\thread.hpp>
#include <boost\algorithm\string.hpp>
#include "hkbclipgenerator.h"

using namespace std;

namespace clipgenerator
{
	string key = "v";
	string classname = "hkbClipGenerator";
	string signature = "0x333b85b9";
}

hkbclipgenerator::hkbclipgenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + clipgenerator::key + to_string(functionlayer) + ">";

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

void hkbclipgenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbClipGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"triggers\">", 0) != string::npos)
			{
				s_triggers = line.substr(28, line.find("</hkparam>") - 28);

				if (s_triggers != "null")
				{
					referencingIDs[s_triggers].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbClipGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbClipGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbclipgenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbClipGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"triggers\">", 0) != string::npos)
			{
				s_triggers = line.substr(28, line.find("</hkparam>") - 28);

				if (s_triggers != "null")
				{
					if (!exchangeID[s_triggers].empty())
					{
						int tempint = line.find(s_triggers);
						s_triggers = exchangeID[s_triggers];
						line.replace(tempint, line.find("</hkparam>") - tempint, s_triggers);
					}

					parent[s_triggers] = id;
					referencingIDs[s_triggers].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbClipGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbClipGenerator(newID: " << id << ") with hkbClipGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (s_triggers != "null")
		{
			referencingIDs[s_triggers].pop_back();
			referencingIDs[s_triggers].push_back(tempid);
			parent[s_triggers] = tempid;
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
			cout << "Comparing hkbClipGenerator(newID: " << id << ") with hkbClipGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbClipGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void hkbclipgenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbClipGenerator(ID: " << id << ") has been initialized!" << endl;
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
			else if (line.find("<hkparam name=\"triggers\">", 0) != string::npos)
			{
				s_triggers = line.substr(28, line.find("</hkparam>") - 28);

				if (s_triggers != "null")
				{
					if (!exchangeID[s_triggers].empty())
					{
						s_triggers = exchangeID[s_triggers];
					}

					parent[s_triggers] = id;
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
		cout << "Dummy hkbClipGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string hkbclipgenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbclipgenerator::IsBindingNull()
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

string hkbclipgenerator::GetTriggers()
{
	return "#" + boost::regex_replace(string(s_triggers), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbclipgenerator::IsTriggersNull()
{
	if (s_triggers.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbclipgenerator::GetAddress()
{
	return address;
}

bool hkbclipgenerator::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbclipgenerator>> hkbclipgeneratorList;
safeStringUMap<shared_ptr<hkbclipgenerator>> hkbclipgeneratorList_E;

void hkbclipgenerator::regis(string id, bool isEdited)
{
	isEdited ? hkbclipgeneratorList_E[id] = shared_from_this() : hkbclipgeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void hkbclipgenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("animationName", line, animationName)) ++type;

					break;
				}
				case 4:
				{
					string output;

					if (readParam("triggers", line, output))
					{
						triggers = (isEdited ? hkbcliptriggerarrayList_E : hkbcliptriggerarrayList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					if (readParam("cropStartAmountLocalTime", line, cropStartAmountLocalTime)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("cropEndAmountLocalTime", line, cropEndAmountLocalTime)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("startTime", line, startTime)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("playbackSpeed", line, playbackSpeed)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("enforcedDuration", line, enforcedDuration)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("userControlledTimeFraction", line, userControlledTimeFraction)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("animationBindingIndex", line, animationBindingIndex)) ++type;

					break;
				}
				case 12:
				{
					string data;

					if (readParam("mode", line, data))
					{
						if (data == "MODE_SINGLE_PLAY") mode = MODE_SINGLE_PLAY;
						else if (data == "MODE_LOOPING") mode = MODE_LOOPING;
						else if (data == "MODE_USER_CONTROLLED") mode = MODE_USER_CONTROLLED;
						else if (data == "MODE_PING_PONG") mode = MODE_PING_PONG;
						else mode = MODE_COUNT;

						++type;
					}

					break;
				}
				case 13:
				{
					int output;

					if (readParam("flags", line, output))
					{
						flags = static_cast<clipflags>(output);
						++type;
					}
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << clipgenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbclipgenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + clipgenerator::key + to_string(functionlayer) + ">";
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

			if (triggers) threadedNextNode(triggers, filepath, curadd, functionlayer, graphroot);
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
				hkbclipgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbclipgeneratorList_E[ID] = protect;
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
				hkbclipgeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbclipgeneratorList_E[ID] = protect;
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

string hkbclipgenerator::getClassCode()
{
	return clipgenerator::key;
}

void hkbclipgenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(12);
	output.reserve(16);
	usize base = 2;
	hkbclipgenerator* ctrpart = static_cast<hkbclipgenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, clipgenerator::classname, clipgenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("animationName", animationName, ctrpart->animationName, output, storeline, base, false, open, isEdited, false);
	paramMatch("triggers", triggers, ctrpart->triggers, output, storeline, base, false, open, isEdited);
	paramMatch("cropStartAmountLocalTime", cropStartAmountLocalTime, ctrpart->cropStartAmountLocalTime, output, storeline, base, false, open, isEdited);
	paramMatch("cropEndAmountLocalTime", cropEndAmountLocalTime, ctrpart->cropEndAmountLocalTime, output, storeline, base, false, open, isEdited);
	paramMatch("startTime", startTime, ctrpart->startTime, output, storeline, base, false, open, isEdited);
	paramMatch("playbackSpeed", playbackSpeed, ctrpart->playbackSpeed, output, storeline, base, false, open, isEdited);
	paramMatch("enforcedDuration", enforcedDuration, ctrpart->enforcedDuration, output, storeline, base, false, open, isEdited);
	paramMatch("userControlledTimeFraction", userControlledTimeFraction, ctrpart->userControlledTimeFraction, output, storeline, base, false, open, isEdited);
	paramMatch("animationBindingIndex", animationBindingIndex, ctrpart->animationBindingIndex, output, storeline, base, false, open, isEdited);
	paramMatch("mode", getMode(), ctrpart->getMode(), output, storeline, base, false, open, isEdited);
	paramMatch("flags", static_cast<int>(flags), static_cast<int>(ctrpart->flags), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", clipgenerator::classname, output, isEdited);
}

void hkbclipgenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(16);

	output.push_back(openObject(base, ID, clipgenerator::classname, clipgenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "animationName", animationName));
	output.push_back(autoParam(base, "triggers", triggers));
	output.push_back(autoParam(base, "cropStartAmountLocalTime", cropStartAmountLocalTime));
	output.push_back(autoParam(base, "cropEndAmountLocalTime", cropEndAmountLocalTime));
	output.push_back(autoParam(base, "startTime", startTime));
	output.push_back(autoParam(base, "playbackSpeed", playbackSpeed));
	output.push_back(autoParam(base, "enforcedDuration", enforcedDuration));
	output.push_back(autoParam(base, "userControlledTimeFraction", userControlledTimeFraction));
	output.push_back(autoParam(base, "animationBindingIndex", animationBindingIndex));
	output.push_back(autoParam(base, "mode", getMode()));
	output.push_back(autoParam(base, "flags", static_cast<int>(flags)));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, clipgenerator::classname, output, true);
}

void hkbclipgenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (triggers) hkb_parent[triggers] = shared_from_this();
}

void hkbclipgenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;
	
	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (triggers)
	{
		parentRefresh();
		triggers->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbclipgenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

string hkbclipgenerator::getMode()
{
	switch (mode)
	{
		case MODE_SINGLE_PLAY: return "MODE_SINGLE_PLAY";
		case MODE_LOOPING: return "MODE_LOOPING";
		case MODE_USER_CONTROLLED: return "MODE_USER_CONTROLLED";
		case MODE_PING_PONG: return "MODE_PING_PONG";
		case MODE_COUNT: return "MODE_COUNT";
		default: return "MODE_SINGLE_PLAY";
	}
}

void hkbClipGeneratorExport(string id)
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

			if ((line.find(storeline1[curline], 0) != string::npos && line.length() == storeline1[curline].length()) || (line.find("<hkparam name=\"animationName\">") != string::npos && boost::iequals(line, storeline1[curline])))
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
		cout << "ERROR: Edit hkbClipGenerator Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbClipGenerator Output Not Found (File: " << filename << ")" << endl;
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
