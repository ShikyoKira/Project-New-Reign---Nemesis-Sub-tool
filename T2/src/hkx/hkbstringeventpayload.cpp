#include "hkbstringeventpayload.h"
#include "Global.h"

using namespace std;

namespace stringeventpayload
{
	string key = "s";
	string classname = "hkbStringEventPayload";
	string signature = "0xed04256a";
}

hkbstringeventpayload::hkbstringeventpayload(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + stringeventpayload::key + to_string(functionlayer) + ">";

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

void hkbstringeventpayload::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStringEventPayload(ID: " << id << ") has been initialized!" << endl;
	}

	vecstr storeline;
	string line;

	if (FunctionLineOriginal[id].empty())
	{
		cout << "ERROR: hkbStringEventPayload Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbStringEventPayload(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstringeventpayload::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbStringEventPayload(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	if (FunctionLineEdited[id].empty())
	{
		cout << "ERROR: hkbStringEventPayload Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbStringEventPayload(newID: " << id << ") with hkbStringEventPayload(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		{
			vecstr emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < FunctionLineEdited[id].size(); i++)
		{
			FunctionLineNew[tempid].push_back(FunctionLineEdited[id][i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbStringEventPayload(newID: " << id << ") with hkbStringEventPayload(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = FunctionLineEdited[id];
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbStringEventPayload(ID: " << id << ") is complete!" << endl;
	}
}

void hkbstringeventpayload::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbStringEventPayload(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbStringEventPayload Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbStringEventPayload(ID: " << id << ") is complete!" << endl;
	}
}

string hkbstringeventpayload::GetAddress()
{
	return address;
}

bool hkbstringeventpayload::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbstringeventpayload>> hkbstringeventpayloadList;
safeStringUMap<shared_ptr<hkbstringeventpayload>> hkbstringeventpayloadList_E;

void hkbstringeventpayload::regis(string id, bool isEdited)
{
	isEdited ? hkbstringeventpayloadList_E[id] = shared_from_this() : hkbstringeventpayloadList[id] = shared_from_this();
	ID = id;
}

void hkbstringeventpayload::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	for (auto& line : nodelines)
	{
		if (readParam("data", line, data)) break;
	}

	if ((Debug) && (!Error))
	{
		cout << stringeventpayload::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbstringeventpayload::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + stringeventpayload::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
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
				hkbstringeventpayloadList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbstringeventpayloadList_E[ID] = protect;
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
				hkbstringeventpayloadList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbstringeventpayloadList_E[ID] = protect;
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

string hkbstringeventpayload::getClassCode()
{
	return stringeventpayload::key;
}

void hkbstringeventpayload::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	hkbstringeventpayload* ctrpart = static_cast<hkbstringeventpayload*>(counterpart.get());

	output.push_back(openObject(base, ID, stringeventpayload::classname, stringeventpayload::signature));		// 1
	paramMatch("data", data, ctrpart->data, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", stringeventpayload::classname, output, isEdited);
}

void hkbstringeventpayload::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, stringeventpayload::classname, stringeventpayload::signature));		// 1
	output.push_back(autoParam(base, "data", data));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, stringeventpayload::classname, output, true);
}

void hkbStringEventPayloadExport(string id)
{
	// stage 1 reading
	vecstr storeline1 = FunctionLineTemp[id];

	vecstr storeline2 = FunctionLineNew[id];

	// stage 2 identify edits
	vecstr output;
	bool IsEdited = false;

	output.push_back(storeline2[0]);

	if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(storeline2[1]);
		output.push_back("<!-- ORIGINAL -->");
		output.push_back(storeline1[1]);
		output.push_back("<!-- CLOSE -->");
		IsEdited = true;
	}

	output.push_back(storeline2[2]);

	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				fwrite << output[i] << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbStringEventPayload Output Not Found (New Edited File: " << filename << ")" << endl;
			Error = true;
			return;
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