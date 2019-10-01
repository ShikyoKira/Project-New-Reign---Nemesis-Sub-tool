#include "hkbcombinetransformsmodifier.h"

using namespace std;

namespace combinetransformsmodifier
{
	string key = "ar";
	string classname = "hkbCombineTransformsModifier";
	string signature = "0xfd1f0b79";
}

hkbcombinetransformsmodifier::hkbcombinetransformsmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + combinetransformsmodifier::key + to_string(functionlayer) + ">";

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

void hkbcombinetransformsmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCombineTransformsModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbCombineTransformsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbCombineTransformsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcombinetransformsmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbCombineTransformsModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbCombineTransformsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbCombineTransformsModifier(newID: " << id << ") with hkbCombineTransformsModifier(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbCombineTransformsModifier(newID: " << id << ") with hkbCombineTransformsModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbCombineTransformsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcombinetransformsmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbCombineTransformsModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: Dummy hkbCombineTransformsModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbCombineTransformsModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbcombinetransformsmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbcombinetransformsmodifier::IsBindingNull()
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

string hkbcombinetransformsmodifier::GetAddress()
{
	return address;
}

bool hkbcombinetransformsmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbcombinetransformsmodifier>> hkbcombinetransformsmodifierList;
safeStringUMap<shared_ptr<hkbcombinetransformsmodifier>> hkbcombinetransformsmodifierList_E;

void hkbcombinetransformsmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbcombinetransformsmodifierList_E[id] = shared_from_this() : hkbcombinetransformsmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbcombinetransformsmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("translationOut", line, translationOut)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("rotationOut", line, rotationOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("leftTranslation", line, leftTranslation)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("leftRotation", line, leftRotation)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("rightTranslation", line, rightTranslation)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("rightRotation", line, rightRotation)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("invertLeftTransform", line, invertLeftTransform)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("invertRightTransform", line, invertRightTransform)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("invertResult", line, invertResult)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << combinetransformsmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcombinetransformsmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + combinetransformsmodifier::key + to_string(functionlayer) + ">";
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
				hkbcombinetransformsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbcombinetransformsmodifierList_E[ID] = protect;
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
				hkbcombinetransformsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbcombinetransformsmodifierList_E[ID] = protect;
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

string hkbcombinetransformsmodifier::getClassCode()
{
	return combinetransformsmodifier::key;
}

void hkbcombinetransformsmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(15);
	output.reserve(15);
	usize base = 2;
	hkbcombinetransformsmodifier* ctrpart = static_cast<hkbcombinetransformsmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, combinetransformsmodifier::classname, combinetransformsmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("translationOut", translationOut, ctrpart->translationOut, output, storeline, base, false, open, isEdited);
	paramMatch("rotationOut", rotationOut, ctrpart->rotationOut, output, storeline, base, false, open, isEdited);
	paramMatch("leftTranslation", leftTranslation, ctrpart->leftTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("leftRotation", leftRotation, ctrpart->leftRotation, output, storeline, base, false, open, isEdited);
	paramMatch("rightTranslation", rightTranslation, ctrpart->rightTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("rightRotation", rightRotation, ctrpart->rightRotation, output, storeline, base, false, open, isEdited);
	paramMatch("invertLeftTransform", invertLeftTransform, ctrpart->invertLeftTransform, output, storeline, base, false, open, isEdited);
	paramMatch("invertRightTransform", invertRightTransform, ctrpart->invertRightTransform, output, storeline, base, false, open, isEdited);
	paramMatch("invertResult", invertResult, ctrpart->invertResult, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", combinetransformsmodifier::classname, output, isEdited);
}

void hkbcombinetransformsmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(15);

	output.push_back(openObject(base, ID, combinetransformsmodifier::classname, combinetransformsmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "translationOut", translationOut));
	output.push_back(autoParam(base, "rotationOut", rotationOut));
	output.push_back(autoParam(base, "leftTranslation", leftTranslation));
	output.push_back(autoParam(base, "leftRotation", leftRotation));
	output.push_back(autoParam(base, "rightTranslation", rightTranslation));
	output.push_back(autoParam(base, "rightRotation", rightRotation));
	output.push_back(autoParam(base, "invertLeftTransform", invertLeftTransform));
	output.push_back(autoParam(base, "invertRightTransform", invertRightTransform));
	output.push_back(autoParam(base, "invertResult", invertResult));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, combinetransformsmodifier::classname, output, true);
}

void hkbcombinetransformsmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbcombinetransformsmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void hkbCombineTransformsModifierExport(string id)
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
		cout << "ERROR: Edit hkbCombineTransformsModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbCombineTransformsModifier Output Not Found (File: " << filename << ")" << endl;
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