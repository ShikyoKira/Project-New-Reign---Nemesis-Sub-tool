#include "hkblookatmodifier.h"

using namespace std;

namespace lookatmodifier
{
	string key = "co";
	string classname = "hkbLookAtModifier";
	string signature = "0x3d28e066";
}

hkblookatmodifier::hkblookatmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + lookatmodifier::key + to_string(functionlayer) + ">";

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

void hkblookatmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbLookAtModifier (ID: " << id << ") has been initialized!" << endl;
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
		}
	}
	else
	{
		cout << "ERROR: hkbLookAtModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbLookAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkblookatmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbLookAtModifier(ID: " << id << ") has been initialized!" << endl;
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

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbLookAtModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbLookAtModifier(newID: " << id << ") with hkbLookAtModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
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
			cout << "Comparing hkbLookAtModifier(newID: " << id << ") with hkbLookAtModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbLookAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkblookatmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbLookAtModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: Dummy hkbLookAtModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbLookAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkblookatmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkblookatmodifier::IsBindingNull()
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

string hkblookatmodifier::GetAddress()
{
	return address;
}

bool hkblookatmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkblookatmodifier>> hkblookatmodifierList;
safeStringUMap<shared_ptr<hkblookatmodifier>> hkblookatmodifierList_E;

void hkblookatmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkblookatmodifierList_E[id] = shared_from_this() : hkblookatmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkblookatmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("targetWS", line, targetWS)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("headForwardLS", line, headForwardLS)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("neckForwardLS", line, neckForwardLS)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("neckRightLS", line, neckRightLS)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("eyePositionHS", line, eyePositionHS)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("newTargetGain", line, newTargetGain)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("onGain", line, onGain)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("offGain", line, offGain)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("limitAngleDegrees", line, limitAngleDegrees)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("limitAngleLeft", line, limitAngleLeft)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("limitAngleRight", line, limitAngleRight)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("limitAngleUp", line, limitAngleUp)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("limitAngleDown", line, limitAngleDown)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("headIndex", line, headIndex)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("neckIndex", line, neckIndex)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("isOn", line, isOn)) ++type;

					break;
				}
				case 20:
				{
					if (readParam("individualLimitsOn", line, individualLimitsOn)) ++type;

					break;
				}
				case 21:
				{
					if (readParam("isTargetInsideLimitCone", line, isTargetInsideLimitCone)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("lookAtLastTargetWS", line, lookAtLastTargetWS)) ++type;

					break;
				}
				case 23:
				{
					if (readParam("lookAtWeight", line, lookAtWeight)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << lookatmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkblookatmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + lookatmodifier::key + to_string(functionlayer) + ">";
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
				hkblookatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkblookatmodifierList_E[ID] = protect;
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
				hkblookatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkblookatmodifierList_E[ID] = protect;
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

string hkblookatmodifier::getClassCode()
{
	return lookatmodifier::key;
}

void hkblookatmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(26);
	usize base = 2;
	hkblookatmodifier* ctrpart = static_cast<hkblookatmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, lookatmodifier::classname, lookatmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("targetWS", targetWS, ctrpart->targetWS, output, storeline, base, false, open, isEdited);
	paramMatch("headForwardLS", headForwardLS, ctrpart->headForwardLS, output, storeline, base, false, open, isEdited);
	paramMatch("neckForwardLS", neckForwardLS, ctrpart->neckForwardLS, output, storeline, base, false, open, isEdited);
	paramMatch("neckRightLS", neckRightLS, ctrpart->neckRightLS, output, storeline, base, false, open, isEdited);
	paramMatch("eyePositionHS", eyePositionHS, ctrpart->eyePositionHS, output, storeline, base, false, open, isEdited);
	paramMatch("newTargetGain", newTargetGain, ctrpart->newTargetGain, output, storeline, base, false, open, isEdited);
	paramMatch("onGain", onGain, ctrpart->onGain, output, storeline, base, false, open, isEdited);
	paramMatch("offGain", offGain, ctrpart->offGain, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleDegrees", limitAngleDegrees, ctrpart->limitAngleDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleLeft", limitAngleLeft, ctrpart->limitAngleLeft, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleRight", limitAngleRight, ctrpart->limitAngleRight, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleUp", limitAngleUp, ctrpart->limitAngleUp, output, storeline, base, false, open, isEdited);
	paramMatch("limitAngleDown", limitAngleDown, ctrpart->limitAngleDown, output, storeline, base, false, open, isEdited);
	paramMatch("headIndex", headIndex, ctrpart->headIndex, output, storeline, base, false, open, isEdited);
	paramMatch("neckIndex", neckIndex, ctrpart->neckIndex, output, storeline, base, false, open, isEdited);
	paramMatch("isOn", isOn, ctrpart->isOn, output, storeline, base, false, open, isEdited);
	paramMatch("individualLimitsOn", individualLimitsOn, ctrpart->individualLimitsOn, output, storeline, base, false, open, isEdited);
	paramMatch("isTargetInsideLimitCone", isTargetInsideLimitCone, ctrpart->isTargetInsideLimitCone, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtLastTargetWS", lookAtLastTargetWS, ctrpart->lookAtLastTargetWS, output, storeline, base, false, open, isEdited);
	paramMatch("lookAtWeight", lookAtWeight, ctrpart->lookAtWeight, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", lookatmodifier::classname, output, isEdited);
}

void hkblookatmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(26);

	output.push_back(openObject(base, ID, lookatmodifier::classname, lookatmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "targetWS", targetWS));
	output.push_back(autoParam(base, "headForwardLS", headForwardLS));
	output.push_back(autoParam(base, "neckForwardLS", neckForwardLS));
	output.push_back(autoParam(base, "neckRightLS", neckRightLS));
	output.push_back(autoParam(base, "eyePositionHS", eyePositionHS));
	output.push_back(autoParam(base, "newTargetGain", newTargetGain));
	output.push_back(autoParam(base, "onGain", onGain));
	output.push_back(autoParam(base, "offGain", offGain));
	output.push_back(autoParam(base, "limitAngleDegrees", limitAngleDegrees));
	output.push_back(autoParam(base, "limitAngleLeft", limitAngleLeft));
	output.push_back(autoParam(base, "limitAngleRight", limitAngleRight));
	output.push_back(autoParam(base, "limitAngleUp", limitAngleUp));
	output.push_back(autoParam(base, "limitAngleDown", limitAngleDown));
	output.push_back(autoParam(base, "headIndex", headIndex));
	output.push_back(autoParam(base, "neckIndex", neckIndex));
	output.push_back(autoParam(base, "isOn", isOn));
	output.push_back(autoParam(base, "individualLimitsOn", individualLimitsOn));
	output.push_back(autoParam(base, "isTargetInsideLimitCone", isTargetInsideLimitCone));
	output.push_back(autoParam(base, "lookAtLastTargetWS", lookAtLastTargetWS));
	output.push_back(autoParam(base, "lookAtWeight", lookAtWeight));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, lookatmodifier::classname, output, true);
}

void hkblookatmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkblookatmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbLookAtModifierExport(string id)
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
		cout << "ERROR: Edit hkbLookAtModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbLookAtModifier Output Not Found (File: " << filename << ")" << endl;
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