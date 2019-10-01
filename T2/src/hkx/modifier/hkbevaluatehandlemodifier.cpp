#include <boost\thread.hpp>
#include "hkbevaluatehandlemodifier.h"

using namespace std;

namespace evaluatehandlemodifier
{
	string key = "ao";
	string classname = "hkbEvaluateHandleModifier";
	string signature = "0x79757102";
}

hkbevaluatehandlemodifier::hkbevaluatehandlemodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + evaluatehandlemodifier::key + to_string(functionlayer) + ">";

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

void hkbevaluatehandlemodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEvaluateHandleModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbEvaluateHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbEvaluateHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbevaluatehandlemodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbEvaluateHandleModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbEvaluateHandleModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbEvaluateHandleModifier(newID: " << id << ") with hkbEvaluateHandleModifier(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbEvaluateHandleModifier(newID: " << id << ") with hkbEvaluateHandleModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbEvaluateHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbevaluatehandlemodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbEvaluateHandleModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: Dummy hkbEvaluateHandleModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbEvaluateHandleModifier(ID: " << id << ") is complete!" << endl;
	}
}

string hkbevaluatehandlemodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbevaluatehandlemodifier::IsBindingNull()
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

string hkbevaluatehandlemodifier::GetAddress()
{
	return address;
}

bool hkbevaluatehandlemodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbevaluatehandlemodifier>> hkbevaluatehandlemodifierList;
safeStringUMap<shared_ptr<hkbevaluatehandlemodifier>> hkbevaluatehandlemodifierList_E;

void hkbevaluatehandlemodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbevaluatehandlemodifierList_E[id] = shared_from_this() : hkbevaluatehandlemodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbevaluatehandlemodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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

					if (readParam("handle", line, output))
					{
						handle = (isEdited ? hkbhandleList_E : hkbhandleList)[output];
						++type;
					}

					break;
				}
				case 5:
				{
					if (readParam("handlePositionOut", line, handlePositionOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("handleRotationOut", line, handleRotationOut)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("isValidOut", line, isValidOut)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("extrapolationTimeStep", line, extrapolationTimeStep)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("handleChangeSpeed", line, handleChangeSpeed)) ++type;

					break;
				}
				case 10:
				{
					string output;

					if (readParam("handleChangeMode", line, output))
					{
						handleChangeMode = output == "HANDLE_CHANGE_MODE_ABRUPT" ? HANDLE_CHANGE_MODE_ABRUPT : HANDLE_CHANGE_MODE_CONSTANT_VELOCITY;
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
				else if (header == "enable") enable = itr->str(2) == "true" ? true : false;
				else if (header == "handle") handle = isEdited ? hkbhandleList_E[itr->str(2)] : hkbhandleList[itr->str(2)];
				else if (header == "handlePositionOut") handlePositionOut.update(itr->str(2));
				else if (header == "handleRotationOut") handleRotationOut.update(itr->str(2));
				else if (header == "isValidOut") isValidOut = itr->str(2) == "true" ? true : false;
				else if (header == "extrapolationTimeStep") extrapolationTimeStep = stod(itr->str(2));
				else if (header == "handleChangeSpeed") handleChangeSpeed = stod(itr->str(2));
				else if (header == "handleChangeMode") handleChangeMode = itr->str(2) == "HANDLE_CHANGE_MODE_ABRUPT" ? HANDLE_CHANGE_MODE_ABRUPT :
					HANDLE_CHANGE_MODE_CONSTANT_VELOCITY;

				break;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << evaluatehandlemodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbevaluatehandlemodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + evaluatehandlemodifier::key + to_string(functionlayer) + ">";
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

			if (handle) threadedNextNode(handle, filepath, curadd, functionlayer, graphroot);
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
				hkbevaluatehandlemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbevaluatehandlemodifierList_E[ID] = protect;
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
				hkbevaluatehandlemodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbevaluatehandlemodifierList_E[ID] = protect;
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

string hkbevaluatehandlemodifier::getClassCode()
{
	return evaluatehandlemodifier::key;
}

void hkbevaluatehandlemodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(13);
	output.reserve(13);
	usize base = 2;
	hkbevaluatehandlemodifier* ctrpart = static_cast<hkbevaluatehandlemodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, evaluatehandlemodifier::classname, evaluatehandlemodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("handle", handle, ctrpart->handle, output, storeline, base, false, open, isEdited);
	paramMatch("handlePositionOut", handlePositionOut, ctrpart->handlePositionOut, output, storeline, base, false, open, isEdited);
	paramMatch("handleRotationOut", handleRotationOut, ctrpart->handleRotationOut, output, storeline, base, false, open, isEdited);
	paramMatch("isValidOut", isValidOut, ctrpart->isValidOut, output, storeline, base, false, open, isEdited);
	paramMatch("extrapolationTimeStep", extrapolationTimeStep, ctrpart->extrapolationTimeStep, output, storeline, base, false, open, isEdited);
	paramMatch("handleChangeSpeed", handleChangeSpeed, ctrpart->handleChangeSpeed, output, storeline, base, false, open, isEdited);
	paramMatch("handleChangeMode", getHandleChangeMode(), ctrpart->getHandleChangeMode(), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", evaluatehandlemodifier::classname, output, isEdited);
}

void hkbevaluatehandlemodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(13);

	output.push_back(openObject(base, ID, evaluatehandlemodifier::classname, evaluatehandlemodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "handle", handle));
	output.push_back(autoParam(base, "handlePositionOut", handlePositionOut));
	output.push_back(autoParam(base, "handleRotationOut", handleRotationOut));
	output.push_back(autoParam(base, "isValidOut", isValidOut));
	output.push_back(autoParam(base, "extrapolationTimeStep", extrapolationTimeStep));
	output.push_back(autoParam(base, "handleChangeSpeed", handleChangeSpeed));
	output.push_back(autoParam(base, "handleChangeMode", getHandleChangeMode()));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, evaluatehandlemodifier::classname, output, true);
}

void hkbevaluatehandlemodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (handle) hkb_parent[handle] = shared_from_this();
}

void hkbevaluatehandlemodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (handle)
	{
		parentRefresh();
		handle->connect(filepath, address, functionlayer, true, graphroot);
	}
}

string hkbevaluatehandlemodifier::getHandleChangeMode()
{
	switch (handleChangeMode)
	{
		case HANDLE_CHANGE_MODE_ABRUPT: return "HANDLE_CHANGE_MODE_ABRUPT";
		case HANDLE_CHANGE_MODE_CONSTANT_VELOCITY: return "HANDLE_CHANGE_MODE_CONSTANT_VELOCITY";
		default: return "HANDLE_CHANGE_MODE_ABRUPT";
	}
}

void hkbevaluatehandlemodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbEvaluateHandleModifierExport(string id)
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
		cout << "ERROR: Edit hkbEvaluateHandleModifier Output Not Found (ID: " << id << ")" << endl;
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
			cout << "ERROR: Edit hkbEvaluateHandleModifier Output Not Found (File: " << filename << ")" << endl;
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