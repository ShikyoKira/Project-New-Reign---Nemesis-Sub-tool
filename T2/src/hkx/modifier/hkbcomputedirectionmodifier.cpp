#include "hkbcomputedirectionmodifier.h"

using namespace std;

namespace computedirectionmodifier
{
	string key = "br";
	string classname = "hkbComputeDirectionModifier";
	string signature = "0xdf358bd3";
}

hkbcomputedirectionmodifier::hkbcomputedirectionmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + computedirectionmodifier::key + to_string(functionlayer) + ">";

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

void hkbcomputedirectionmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbComputeDirectionModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbComputeDirectionModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbComputeDirectionModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcomputedirectionmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbComputeDirectionModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbComputeDirectionModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbComputeDirectionModifier(newID: " << id << ") with hkbComputeDirectionModifier(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbComputeDirectionModifier(newID: " << id << ") with hkbComputeDirectionModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbComputeDirectionModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcomputedirectionmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbComputeDirectionModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: Dummy hkbComputeDirectionModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbComputeDirectionModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbcomputedirectionmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbcomputedirectionmodifier::IsBindingNull()
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

string hkbcomputedirectionmodifier::GetAddress()
{
	return address;
}

bool hkbcomputedirectionmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbcomputedirectionmodifier>> hkbcomputedirectionmodifierList;
safeStringUMap<shared_ptr<hkbcomputedirectionmodifier>> hkbcomputedirectionmodifierList_E;

void hkbcomputedirectionmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbcomputedirectionmodifierList_E[id] = shared_from_this() : hkbcomputedirectionmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbcomputedirectionmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("pointIn", line, pointIn)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("pointOut", line, pointOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("groundAngleOut", line, groundAngleOut)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("upAngleOut", line, upAngleOut)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("verticalOffset", line, verticalOffset)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("reverseGroundAngle", line, reverseGroundAngle)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("reverseUpAngle", line, reverseUpAngle)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("projectPoint", line, projectPoint)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("normalizePoint", line, normalizePoint)) ++type;

					break;
				}
				case 13:
				{
					if (readParam("computeOnlyOnce", line, computeOnlyOnce)) ++type;

					break;
				}
				case 14:
				{
					if (readParam("computedOutput", line, computedOutput)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << computedirectionmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcomputedirectionmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + computedirectionmodifier::key + to_string(functionlayer) + ">";
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
				hkbcomputedirectionmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbcomputedirectionmodifierList_E[ID] = protect;
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
				hkbcomputedirectionmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbcomputedirectionmodifierList_E[ID] = protect;
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

string hkbcomputedirectionmodifier::getClassCode()
{
	return computedirectionmodifier::key;
}

void hkbcomputedirectionmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(17);
	output.reserve(17);
	usize base = 2;
	hkbcomputedirectionmodifier* ctrpart = static_cast<hkbcomputedirectionmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, computedirectionmodifier::classname, computedirectionmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("pointIn", pointIn, ctrpart->pointIn, output, storeline, base, false, open, isEdited);
	paramMatch("pointOut", pointOut, ctrpart->pointOut, output, storeline, base, false, open, isEdited);
	paramMatch("groundAngleOut", groundAngleOut, ctrpart->groundAngleOut, output, storeline, base, false, open, isEdited);
	paramMatch("upAngleOut", upAngleOut, ctrpart->upAngleOut, output, storeline, base, false, open, isEdited);
	paramMatch("verticalOffset", verticalOffset, ctrpart->verticalOffset, output, storeline, base, false, open, isEdited);
	paramMatch("reverseGroundAngle", reverseGroundAngle, ctrpart->reverseGroundAngle, output, storeline, base, false, open, isEdited);
	paramMatch("reverseUpAngle", reverseUpAngle, ctrpart->reverseUpAngle, output, storeline, base, false, open, isEdited);
	paramMatch("projectPoint", projectPoint, ctrpart->projectPoint, output, storeline, base, false, open, isEdited);
	paramMatch("normalizePoint", normalizePoint, ctrpart->normalizePoint, output, storeline, base, false, open, isEdited);
	paramMatch("computeOnlyOnce", computeOnlyOnce, ctrpart->computeOnlyOnce, output, storeline, base, false, open, isEdited);
	paramMatch("computedOutput", computedOutput, ctrpart->computedOutput, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", computedirectionmodifier::classname, output, isEdited);
}

void hkbcomputedirectionmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(17);

	output.push_back(openObject(base, ID, computedirectionmodifier::classname, computedirectionmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "pointIn", pointIn));
	output.push_back(autoParam(base, "pointOut", pointOut));
	output.push_back(autoParam(base, "groundAngleOut", groundAngleOut));
	output.push_back(autoParam(base, "upAngleOut", upAngleOut));
	output.push_back(autoParam(base, "verticalOffset", verticalOffset));
	output.push_back(autoParam(base, "reverseGroundAngle", reverseGroundAngle));
	output.push_back(autoParam(base, "reverseUpAngle", reverseUpAngle));
	output.push_back(autoParam(base, "projectPoint", projectPoint));
	output.push_back(autoParam(base, "normalizePoint", normalizePoint));
	output.push_back(autoParam(base, "computeOnlyOnce", computeOnlyOnce));
	output.push_back(autoParam(base, "computedOutput", computedOutput));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, computedirectionmodifier::classname, output, true);
}

void hkbcomputedirectionmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbcomputedirectionmodifier::nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbComputeDirectionModifierExport(string id)
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
		cout << "ERROR: Edit hkbComputeDirectionModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbComputeDirectionModifier Output Not Found (File: " << filename << ")" << endl;
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