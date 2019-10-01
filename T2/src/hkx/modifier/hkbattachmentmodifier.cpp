#include <boost\thread.hpp>
#include "hkbattachmentmodifier.h"

using namespace std;

namespace attachmentmodifier
{
	string key = "ap";
	string classname = "hkbAttachmentModifier";
	string signature = "0xcc0aab32";
}

hkbattachmentmodifier::hkbattachmentmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + attachmentmodifier::key + to_string(functionlayer) + ">";

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

void hkbattachmentmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbAttachmentModifier(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	payload.reserve(4);

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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbAttachmentModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbAttachmentModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbattachmentmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbAttachmentModifier(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	payload.reserve(4);

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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						int tempint = line.find(payload.back());
						payload.back() = exchangeID[payload.back()];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload.back());
					}

					parent[payload.back()] = id;
					referencingIDs[payload.back()].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbAttachmentModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing hkbAttachmentModifier(newID: " << id << ") with hkbAttachmentModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
				parent[payload[i]] = tempid;
			}
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
			cout << "Comparing hkbAttachmentModifier(newID: " << id << ") with hkbAttachmentModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbAttachmentModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbattachmentmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbAttachmentModifier(ID: " << id << ") has been initialized!" << endl;
	}

	payload.reserve(4);
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
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						payload.back() = exchangeID[payload.back()];
					}

					parent[payload.back()] = id;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbAttachmentModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbAttachmentModifier(ID: " << id << ") is complete!" << endl;
	}
}

int hkbattachmentmodifier::GetPayloadCount()
{
	return int(payload.size());
}

string hkbattachmentmodifier::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbattachmentmodifier::IsPayloadNull(int child)
{
	if (payload[child].find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbattachmentmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbattachmentmodifier::IsBindingNull()
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

string hkbattachmentmodifier::GetAddress()
{
	return address;
}

bool hkbattachmentmodifier::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbattachmentmodifier>> hkbattachmentmodifierList;
safeStringUMap<shared_ptr<hkbattachmentmodifier>> hkbattachmentmodifierList_E;

void hkbattachmentmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbattachmentmodifierList_E[id] = shared_from_this() : hkbattachmentmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbattachmentmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					if (readParam("id", line, sendToAttacherOnAttach.id)) ++type;

					break;
				}
				case 5:
				{
					string output;

					if (readParam("payload", line, output))
					{
						sendToAttacherOnAttach.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("id", line, sendToAttacheeOnAttach.id)) ++type;

					break;
				}
				case 7:
				{
					string output;

					if (readParam("payload", line, output))
					{
						sendToAttacheeOnAttach.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 8:
				{
					if (readParam("id", line, sendToAttacherOnDetach.id)) ++type;

					break;
				}
				case 9:
				{
					string output;

					if (readParam("payload", line, output))
					{
						sendToAttacherOnDetach.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 10:
				{
					if (readParam("id", line, sendToAttacheeOnDetach.id)) ++type;

					break;
				}
				case 11:
				{
					string output;

					if (readParam("payload", line, output))
					{
						sendToAttacheeOnDetach.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 12:
				{
					string output;

					if (readParam("attachmentSetup", line, output))
					{
						attachmentSetup = (isEdited ? hkbattachmentsetupList_E : hkbattachmentsetupList)[output];
						++type;
					}

					break;
				}
				case 13:
				{
					string output;

					if (readParam("attacherHandle", line, output))
					{
						attacherHandle = (isEdited ? hkbhandleList_E : hkbhandleList)[output];
						++type;
					}

					break;
				}
				case 14:
				{
					string output;

					if (readParam("attacheeHandle", line, output))
					{
						attacheeHandle = (isEdited ? hkbhandleList_E : hkbhandleList)[output];
						++type;
					}

					break;
				}
				case 15:
				{
					if (readParam("attacheeLayer", line, attacheeLayer)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << attachmentmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbattachmentmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + attachmentmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	sendToAttacherOnAttach.id.connectEventInfo(ID, graphroot);
	sendToAttacheeOnAttach.id.connectEventInfo(ID, graphroot);
	sendToAttacherOnDetach.id.connectEventInfo(ID, graphroot);
	sendToAttacheeOnDetach.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (sendToAttacherOnAttach.payload) threadedNextNode(sendToAttacherOnAttach.payload, filepath, curadd + "0", functionlayer, graphroot);

			if (sendToAttacheeOnAttach.payload) threadedNextNode(sendToAttacheeOnAttach.payload, filepath, curadd + "1", functionlayer, graphroot);

			if (sendToAttacherOnDetach.payload) threadedNextNode(sendToAttacherOnAttach.payload, filepath, curadd + "2", functionlayer, graphroot);

			if (sendToAttacheeOnDetach.payload) threadedNextNode(sendToAttacheeOnAttach.payload, filepath, curadd + "3", functionlayer, graphroot);

			if (attachmentSetup) threadedNextNode(attachmentSetup, filepath, curadd, functionlayer, graphroot);

			if (attacherHandle) threadedNextNode(attacherHandle, filepath, curadd + "0", functionlayer, graphroot);

			if (attacheeHandle) threadedNextNode(attacheeHandle, filepath, curadd + "1", functionlayer, graphroot);
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
				hkbattachmentmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbattachmentmodifierList_E[ID] = protect;
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
				hkbattachmentmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbattachmentmodifierList_E[ID] = protect;
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

string hkbattachmentmodifier::getClassCode()
{
	return attachmentmodifier::key;
}

void hkbattachmentmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(34);
	usize base = 2;
	hkbattachmentmodifier* ctrpart = static_cast<hkbattachmentmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, attachmentmodifier::classname, attachmentmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, true, open, isEdited);
	output.push_back(openParam(base, "sendToAttacherOnAttach"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", sendToAttacherOnAttach.id, ctrpart->sendToAttacherOnAttach.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", sendToAttacherOnAttach.payload, ctrpart->sendToAttacherOnAttach.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "sendToAttacheeOnAttach"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", sendToAttacheeOnAttach.id, ctrpart->sendToAttacheeOnAttach.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", sendToAttacheeOnAttach.payload, ctrpart->sendToAttacheeOnAttach.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "sendToAttacherOnDetach"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", sendToAttacherOnDetach.id, ctrpart->sendToAttacherOnDetach.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", sendToAttacherOnDetach.payload, ctrpart->sendToAttacherOnDetach.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "sendToAttacheeOnDetach"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", sendToAttacheeOnDetach.id, ctrpart->sendToAttacheeOnDetach.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", sendToAttacheeOnDetach.payload, ctrpart->sendToAttacheeOnDetach.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("attachmentSetup", attachmentSetup, ctrpart->attachmentSetup, output, storeline, base, false, open, isEdited);
	paramMatch("attacherHandle", attacherHandle, ctrpart->attacherHandle, output, storeline, base, false, open, isEdited);
	paramMatch("attacheeHandle", attacheeHandle, ctrpart->attacheeHandle, output, storeline, base, false, open, isEdited);
	paramMatch("attacheeLayer", attacheeLayer, ctrpart->attacheeLayer, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", attachmentmodifier::classname, output, isEdited);
}

void hkbattachmentmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(34);

	output.push_back(openObject(base, ID, attachmentmodifier::classname, attachmentmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(openParam(base, "sendToAttacherOnAttach"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", sendToAttacherOnAttach.id));
	output.push_back(autoParam(base, "payload", sendToAttacherOnAttach.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "sendToAttacheeOnAttach"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", sendToAttacheeOnAttach.id));
	output.push_back(autoParam(base, "payload", sendToAttacheeOnAttach.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "sendToAttacherOnDetach"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", sendToAttacherOnDetach.id));
	output.push_back(autoParam(base, "payload", sendToAttacherOnDetach.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "sendToAttacheeOnDetach"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", sendToAttacheeOnDetach.id));
	output.push_back(autoParam(base, "payload", sendToAttacheeOnDetach.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "attachmentSetup", attachmentSetup));
	output.push_back(autoParam(base, "attacherHandle", attacherHandle));
	output.push_back(autoParam(base, "attacheeHandle", attacheeHandle));
	output.push_back(autoParam(base, "attacheeLayer", attacheeLayer));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, attachmentmodifier::classname, output, true);
}

void hkbattachmentmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (sendToAttacherOnAttach.payload) hkb_parent[sendToAttacherOnAttach.payload] = shared_from_this();
	if (sendToAttacheeOnAttach.payload) hkb_parent[sendToAttacheeOnAttach.payload] = shared_from_this();
	if (sendToAttacherOnDetach.payload) hkb_parent[sendToAttacherOnDetach.payload] = shared_from_this();
	if (sendToAttacheeOnDetach.payload) hkb_parent[sendToAttacheeOnDetach.payload] = shared_from_this();
	if (attachmentSetup) hkb_parent[attachmentSetup] = shared_from_this();
	if (attacherHandle) hkb_parent[attacherHandle] = shared_from_this();
	if (attacheeHandle) hkb_parent[attacheeHandle] = shared_from_this();
}

void hkbattachmentmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (sendToAttacherOnAttach.payload)
	{
		parentRefresh();
		sendToAttacherOnAttach.payload->connect(filepath, address + "0", functionlayer, true, graphroot);
	}

	if (sendToAttacheeOnAttach.payload)
	{
		parentRefresh();
		sendToAttacheeOnAttach.payload->connect(filepath, address + "1", functionlayer, true, graphroot);
	}

	if (sendToAttacherOnDetach.payload)
	{
		parentRefresh();
		sendToAttacherOnDetach.payload->connect(filepath, address + "2", functionlayer, true, graphroot);
	}

	if (sendToAttacheeOnDetach.payload)
	{
		parentRefresh();
		sendToAttacheeOnDetach.payload->connect(filepath, address + "3", functionlayer, true, graphroot);
	}

	if (attachmentSetup)
	{
		parentRefresh();
		attachmentSetup->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (attacherHandle)
	{
		parentRefresh();
		attacherHandle->connect(filepath, address + "0", functionlayer, true, graphroot);
	}

	if (attacheeHandle)
	{
		parentRefresh();
		attacheeHandle->connect(filepath, address + "1", functionlayer, true, graphroot);
	}
}

void hkbattachmentmodifier::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void hkbAttachmentModifierExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbAttachmentModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				if ((line.find("sendTo", 0) != string::npos) || (line.find("attachmentSetup", 0) != string::npos))
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

					if (line.find("attachmentSetup", 0) != string::npos)
					{
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
					}
					else
					{
						storeline2.push_back(storeline1[curline]);
					}
				}
				else
				{
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
				}

				curline++;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbAttachmentModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(stoi(id.substr(1)), storeline2);

	if (open) // close unclosed edits
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
				if ((storeline2[i].find("OPEN", 0) != string::npos) && ((storeline2[i + 1].find("sendTo", 0) != string::npos) || (storeline2[i + 1].find("attachmentSetup", 0) != string::npos)))
				{
					fwrite << "				</hkobject>" << "\n";
					fwrite << "			</hkparam>" << "\n";
				}

				fwrite << storeline2[i] << "\n";

				if (storeline2[i].find("sendTo", 0) != string::npos)
				{
					fwrite << "				<hkobject>" << "\n";
				}

				if (i < storeline2.size() - 1)
				{
					if (((storeline2[i].find("OPEN", 0) == string::npos) && (storeline2[i].find("ORIGINAL", 0) == string::npos)) && (((storeline2[i + 1].find("sendTo", 0) != string::npos) && (storeline2[i + 1].find("sendToAttacherOnAttach", 0) == string::npos)) || (storeline2[i + 1].find("attachmentSetup", 0) != string::npos)))
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
					}
				}
			}

			fwrite << "		</hkobject>" << "\n";
			output.close();
		}
		else
		{
			cout << "ERROR: Edit hkbAttachmentModifier Output Not Found (File: " << filename << ")" << endl;
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