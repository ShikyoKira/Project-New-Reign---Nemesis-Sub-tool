#include "bsdirectatmodifier.h"

using namespace std;

namespace directatmodifier
{
	string key = "be";
	string classname = "BSDirectAtModifier";
	string signature = "0x19a005c0";
}

bsdirectatmodifier::bsdirectatmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + directatmodifier::key + to_string(functionlayer) + ">";

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

void bsdirectatmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSDirectAtModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: BSDirectAtModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSDirectAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bsdirectatmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSDirectAtModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: BSDirectAtModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSDirectAtModifier(newID: " << id << ") with BSDirectAtModifier(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing BSDirectAtModifier(newID: " << id << ") with BSDirectAtModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSDirectAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

void bsdirectatmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSDirectAtModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: Dummy BSDirectAtModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSDirectAtModifier(ID: " << id << ") is complete!" << endl;
	}
}

string bsdirectatmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bsdirectatmodifier::IsBindingNull()
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

string bsdirectatmodifier::GetAddress()
{
	return address;
}

bool bsdirectatmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bsdirectatmodifier>> bsdirectatmodifierList;
safeStringUMap<shared_ptr<bsdirectatmodifier>> bsdirectatmodifierList_E;

void bsdirectatmodifier::regis(string id, bool isEdited)
{
	isEdited ? bsdirectatmodifierList_E[id] = shared_from_this() : bsdirectatmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void bsdirectatmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("directAtTarget", line, directAtTarget)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("sourceBoneIndex", line, sourceBoneIndex)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("startBoneIndex", line, startBoneIndex)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("endBoneIndex", line, endBoneIndex)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("limitHeadingDegrees", line, limitHeadingDegrees)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("limitPitchDegrees", line, limitPitchDegrees)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("offsetHeadingDegrees", line, offsetHeadingDegrees)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("offsetPitchDegrees", line, offsetPitchDegrees)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("onGain", line, onGain)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("offGain", line, offGain)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("targetLocation", line, targetLocation)) ++type;

					break;
				}
				case 15:
				{
					if (readParam("userInfo", line, userInfo)) ++type;

					break;
				}
				case 16:
				{
					if (readParam("directAtCamera", line, directAtCamera)) ++type;

					break;
				}
				case 17:
				{
					if (readParam("directAtCameraX", line, directAtCameraX)) ++type;

					break;
				}
				case 18:
				{
					if (readParam("directAtCameraY", line, directAtCameraY)) ++type;

					break;
				}
				case 19:
				{
					if (readParam("directAtCameraZ", line, directAtCameraZ)) ++type;

					break;
				}
				case 20:
				{
					if (readParam("active", line, active)) ++type;

					break;
				}
				case 21:
				{
					if (readParam("currentHeadingOffset", line, currentHeadingOffset)) ++type;

					break;
				}
				case 22:
				{
					if (readParam("currentPitchOffset", line, currentPitchOffset)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << directatmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bsdirectatmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + directatmodifier::key + to_string(functionlayer) + ">";
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
				bsdirectatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bsdirectatmodifierList_E[ID] = protect;
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
				bsdirectatmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bsdirectatmodifierList_E[ID] = protect;
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

string bsdirectatmodifier::getClassCode()
{
	return directatmodifier::key;
}

void bsdirectatmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(25);
	output.reserve(25);
	usize base = 2;
	bsdirectatmodifier* ctrpart = static_cast<bsdirectatmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, directatmodifier::classname, directatmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("directAtTarget", directAtTarget, ctrpart->directAtTarget, output, storeline, base, false, open, isEdited);
	paramMatch("sourceBoneIndex", sourceBoneIndex, ctrpart->sourceBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("startBoneIndex", startBoneIndex, ctrpart->startBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("endBoneIndex", endBoneIndex, ctrpart->endBoneIndex, output, storeline, base, false, open, isEdited);
	paramMatch("limitHeadingDegrees", limitHeadingDegrees, ctrpart->limitHeadingDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("limitPitchDegrees", limitPitchDegrees, ctrpart->limitPitchDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("offsetHeadingDegrees", offsetHeadingDegrees, ctrpart->offsetHeadingDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("offsetPitchDegrees", offsetPitchDegrees, ctrpart->offsetPitchDegrees, output, storeline, base, false, open, isEdited);
	paramMatch("onGain", onGain, ctrpart->onGain, output, storeline, base, false, open, isEdited);
	paramMatch("offGain", offGain, ctrpart->offGain, output, storeline, base, false, open, isEdited);
	paramMatch("targetLocation", targetLocation, ctrpart->targetLocation, output, storeline, base, false, open, isEdited);
	paramMatch("userInfo", userInfo, ctrpart->userInfo, output, storeline, base, false, open, isEdited);
	paramMatch("directAtCamera", directAtCamera, ctrpart->directAtCamera, output, storeline, base, false, open, isEdited);
	paramMatch("directAtCameraX", directAtCameraX, ctrpart->directAtCameraX, output, storeline, base, false, open, isEdited);
	paramMatch("directAtCameraY", directAtCameraY, ctrpart->directAtCameraY, output, storeline, base, false, open, isEdited);
	paramMatch("directAtCameraZ", directAtCameraZ, ctrpart->directAtCameraZ, output, storeline, base, false, open, isEdited);
	paramMatch("active", active, ctrpart->active, output, storeline, base, false, open, isEdited);
	paramMatch("currentHeadingOffset", currentHeadingOffset, ctrpart->currentHeadingOffset, output, storeline, base, false, open, isEdited);
	paramMatch("currentPitchOffset", currentPitchOffset, ctrpart->currentPitchOffset, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", directatmodifier::classname, output, isEdited);
}

void bsdirectatmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(25);

	output.push_back(openObject(base, ID, directatmodifier::classname, directatmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "directAtTarget", directAtTarget));
	output.push_back(autoParam(base, "sourceBoneIndex", sourceBoneIndex));
	output.push_back(autoParam(base, "startBoneIndex", startBoneIndex));
	output.push_back(autoParam(base, "endBoneIndex", endBoneIndex));
	output.push_back(autoParam(base, "limitHeadingDegrees", limitHeadingDegrees));
	output.push_back(autoParam(base, "limitPitchDegrees", limitPitchDegrees));
	output.push_back(autoParam(base, "offsetHeadingDegrees", offsetHeadingDegrees));
	output.push_back(autoParam(base, "offsetPitchDegrees", offsetPitchDegrees));
	output.push_back(autoParam(base, "onGain", onGain));
	output.push_back(autoParam(base, "offGain", offGain));
	output.push_back(autoParam(base, "targetLocation", targetLocation));
	output.push_back(autoParam(base, "userInfo", userInfo));
	output.push_back(autoParam(base, "directAtCamera", directAtCamera));
	output.push_back(autoParam(base, "directAtCameraX", directAtCameraX));
	output.push_back(autoParam(base, "directAtCameraY", directAtCameraY));
	output.push_back(autoParam(base, "directAtCameraZ", directAtCameraZ));
	output.push_back(autoParam(base, "active", active));
	output.push_back(autoParam(base, "currentHeadingOffset", currentHeadingOffset));
	output.push_back(autoParam(base, "currentPitchOffset", currentPitchOffset));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, directatmodifier::classname, output, true);
}

void bsdirectatmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void bsdirectatmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void BSDirectAtModifierExport(string id)
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
		cout << "ERROR: Edit BSDirectAtModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit BSDirectAtModifier Output Not Found (File: " << filename << ")" << endl;
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