#include <boost\thread.hpp>
#include "bscyclicblendtransitiongenerator.h"

using namespace std;

namespace cyclicblendtransitiongenerator
{
	string key = "c";
	string classname = "BSCyclicBlendTransitionGenerator";
	string signature = "0x5119eb06";
}

bscyclicblendtransitiongenerator::bscyclicblendtransitiongenerator(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + cyclicblendtransitiongenerator::key + to_string(functionlayer) + ">";

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

void bscyclicblendtransitiongenerator::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSCyclicBlendTransitionGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	payload.reserve(2);

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
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (generator != "null")
				{
					referencingIDs[generator].push_back(id);
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
		cout << "ERROR: BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "BSCyclicBlendTransitionGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bscyclicblendtransitiongenerator::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "BSCyclicBlendTransitionGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;
	string line;
	payload.reserve(2);

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
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (!exchangeID[generator].empty())
				{
					int tempint = line.find(generator);
					line.replace(tempint, line.find("</hkparam>") - tempint, exchangeID[generator]);
					generator = exchangeID[generator];
				}

				if (generator != "null")
				{
					parent[generator] = id;
					referencingIDs[generator].push_back(id);
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
		cout << "ERROR: BSCyclicBlendTransitionGenerator Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			cout << "Comparing BSCyclicBlendTransitionGenerator(newID: " << id << ") with BSCyclicBlendTransitionGenerator(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		if (generator != "null")
		{
			referencingIDs[generator].pop_back();
			referencingIDs[generator].push_back(tempid);
			parent[generator] = tempid;
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

		// stage 3
		bool IsNewChild = false;
		vecstr newstoreline;
		vecstr newchild;

		newstoreline.push_back(FunctionLineTemp[tempid][0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++) // store function body
		{
				newstoreline.push_back(newline[i]);
		}

		FunctionLineNew[tempid] = newstoreline;

		if ((Debug) && (!Error))
		{
			cout << "Comparing BSCyclicBlendTransitionGenerator(newID: " << id << ") with BSCyclicBlendTransitionGenerator(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "BSCyclicBlendTransitionGenerator(ID: " << id << ") is complete!" << endl;
	}
}

void bscyclicblendtransitiongenerator::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy BSCyclicBlendTransitionGenerator(ID: " << id << ") has been initialized!" << endl;
	}

	payload.reserve(2);
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
			else if (line.find("<hkparam name=\"pBlenderGenerator\">", 0) != string::npos)
			{
				generator = line.substr(37, line.find("</hkparam>") - 37);

				if (!exchangeID[generator].empty())
				{
					generator = exchangeID[generator];
				}

				if (generator != "null")
				{
					parent[generator] = id;
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
		cout << "ERROR: Dummy BSCyclicBlendTransitionGenerator Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy BSCyclicBlendTransitionGenerator(ID: " << id << ") is complete!" << endl;
	}
}

string bscyclicblendtransitiongenerator::NextGenerator()
{
	return "#" + boost::regex_replace(string(generator), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

int bscyclicblendtransitiongenerator::GetPayloadCount()
{
	return int(payload.size());
}

string bscyclicblendtransitiongenerator::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bscyclicblendtransitiongenerator::IsPayloadNull(int child)
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

string bscyclicblendtransitiongenerator::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool bscyclicblendtransitiongenerator::IsBindingNull()
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

string bscyclicblendtransitiongenerator::GetAddress()
{
	return address;
}

bool bscyclicblendtransitiongenerator::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<bscyclicblendtransitiongenerator>> bscyclicblendtransitiongeneratorList;
safeStringUMap<shared_ptr<bscyclicblendtransitiongenerator>> bscyclicblendtransitiongeneratorList_E;

void bscyclicblendtransitiongenerator::regis(string id, bool isEdited)
{
	isEdited ? bscyclicblendtransitiongeneratorList_E[id] = shared_from_this() : bscyclicblendtransitiongeneratorList[id] = shared_from_this();
	isEdited ? hkbgeneratorList_E[id] = shared_from_this() : hkbgeneratorList[id] = shared_from_this();
	ID = id;
}

void bscyclicblendtransitiongenerator::dataBake(string filepath, vecstr& nodelines, bool isEdited)
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
					string output;

					if (readParam("pBlenderGenerator", line, output))
					{
						pBlenderGenerator = (isEdited ? hkbblendergeneratorList_E : hkbblendergeneratorList)[output];
						++type;
					}

					break;
				}
				case 4:
				{
					if (readParam("id", line, EventToFreezeBlendValue.id)) ++type;

					break;
				}
				case 5:
				{
					string output;

					if (readParam("payload", line, output))
					{
						EventToFreezeBlendValue.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 6:
				{
					if (readParam("id", line, EventToCrossBlend.id)) ++type;

					break;
				}
				case 7:
				{
					string output;

					if (readParam("payload", line, output))
					{
						EventToCrossBlend.payload = (isEdited ? hkbstringeventpayloadList_E : hkbstringeventpayloadList)[output];
						++type;
					}

					break;
				}
				case 8:
				{
					if (readParam("fBlendParameter", line, fBlendParameter)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("fTransitionDuration", line, fTransitionDuration)) ++type;

					break;
				}
				case 10:
				{
					string data;

					if (readParam("eBlendCurve", line, data))
					{
						if (data == "BLEND_CURVE_SMOOTH") eBlendCurve = BLEND_CURVE_SMOOTH;
						else if (data == "BLEND_CURVE_LINEAR") eBlendCurve = BLEND_CURVE_LINEAR;
						else if (data == "BLEND_CURVE_LINEAR_TO_SMOOTH") eBlendCurve = BLEND_CURVE_LINEAR_TO_SMOOTH;
						else eBlendCurve = BLEND_CURVE_SMOOTH_TO_LINEAR;

						++type;
					}

					break;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << cyclicblendtransitiongenerator::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void bscyclicblendtransitiongenerator::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + cyclicblendtransitiongenerator::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);
	EventToFreezeBlendValue.id.connectEventInfo(ID, graphroot);
	EventToCrossBlend.id.connectEventInfo(ID, graphroot);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			++functionlayer;
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) threadedNextNode(variableBindingSet, filepath, curadd, functionlayer, graphroot);

			if (pBlenderGenerator) threadedNextNode(pBlenderGenerator, filepath, curadd, functionlayer, graphroot);

			if (EventToFreezeBlendValue.payload) threadedNextNode(EventToFreezeBlendValue.payload, filepath, curadd + "0", functionlayer, graphroot);

			if (EventToCrossBlend.payload) threadedNextNode(EventToCrossBlend.payload, filepath, curadd + "1", functionlayer, graphroot);
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
				bscyclicblendtransitiongeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				bscyclicblendtransitiongeneratorList_E[ID] = protect;
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
				bscyclicblendtransitiongeneratorList_E.erase(ID);
				hkbgeneratorList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				bscyclicblendtransitiongeneratorList_E[ID] = protect;
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

string bscyclicblendtransitiongenerator::getClassCode()
{
	return cyclicblendtransitiongenerator::key;
}

void bscyclicblendtransitiongenerator::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(21);
	output.reserve(21);
	usize base = 2;
	bscyclicblendtransitiongenerator* ctrpart = static_cast<bscyclicblendtransitiongenerator*>(counterpart.get());

	output.push_back(openObject(base, ID, cyclicblendtransitiongenerator::classname, cyclicblendtransitiongenerator::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("pBlenderGenerator", pBlenderGenerator, ctrpart->pBlenderGenerator, output, storeline, base, false, open, isEdited);
	output.push_back(openParam(base, "EventToFreezeBlendValue"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", EventToFreezeBlendValue.id, ctrpart->EventToFreezeBlendValue.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", EventToFreezeBlendValue.payload, ctrpart->EventToFreezeBlendValue.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "EventToCrossBlend"));		// 2
	output.push_back(openObject(base));		// 3
	paramMatch("id", EventToCrossBlend.id, ctrpart->EventToCrossBlend.id, output, storeline, base, false, open, isEdited);
	paramMatch("payload", EventToCrossBlend.payload, ctrpart->EventToCrossBlend.payload, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	paramMatch("fBlendParameter", fBlendParameter, ctrpart->fBlendParameter, output, storeline, base, false, open, isEdited);
	paramMatch("fTransitionDuration", fTransitionDuration, ctrpart->fTransitionDuration, output, storeline, base, false, open, isEdited);
	paramMatch("eBlendCurve", getBlendCurve(eBlendCurve), getBlendCurve(ctrpart->eBlendCurve), output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", cyclicblendtransitiongenerator::classname, output, isEdited);
}

void bscyclicblendtransitiongenerator::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(21);

	output.push_back(openObject(base, ID, cyclicblendtransitiongenerator::classname, cyclicblendtransitiongenerator::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "pBlenderGenerator", pBlenderGenerator));
	output.push_back(openParam(base, "EventToFreezeBlendValue"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", EventToFreezeBlendValue.id));
	output.push_back(autoParam(base, "payload", EventToFreezeBlendValue.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(openParam(base, "EventToCrossBlend"));		// 2
	output.push_back(openObject(base));		// 3
	output.push_back(autoParam(base, "id", EventToCrossBlend.id));
	output.push_back(autoParam(base, "payload", EventToCrossBlend.payload));
	output.push_back(closeObject(base));		// 3
	output.push_back(closeParam(base));		// 2
	output.push_back(autoParam(base, "fBlendParameter", fBlendParameter));
	output.push_back(autoParam(base, "fTransitionDuration", fTransitionDuration));
	output.push_back(autoParam(base, "eBlendCurve", getBlendCurve(eBlendCurve)));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, cyclicblendtransitiongenerator::classname, output, true);
}

void bscyclicblendtransitiongenerator::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
	if (pBlenderGenerator) hkb_parent[pBlenderGenerator] = shared_from_this();
	if (EventToFreezeBlendValue.payload) hkb_parent[EventToFreezeBlendValue.payload] = shared_from_this();
	if (EventToCrossBlend.payload) hkb_parent[EventToCrossBlend.payload] = shared_from_this();
}

void bscyclicblendtransitiongenerator::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (pBlenderGenerator)
	{
		parentRefresh();
		pBlenderGenerator->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (EventToFreezeBlendValue.payload)
	{
		parentRefresh();
		EventToFreezeBlendValue.payload->connect(filepath, address, functionlayer, true, graphroot);
	}

	if (EventToCrossBlend.payload)
	{
		parentRefresh();
		EventToCrossBlend.payload->connect(filepath, address, functionlayer, true, graphroot);
	}
}

void bscyclicblendtransitiongenerator::threadedNextNode(shared_ptr<hkbobject> hkb_obj, string filepath, string address, int functionlayer, hkbbehaviorgraph* graphroot)
{
	hkb_obj->connect(filepath, address, functionlayer, false, graphroot);
}

void BSCyclicBlendTransitionGeneratorExport(string id)
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
		cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vecstr output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	int openpoint;
	int closepoint;
	int curline = 0;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if ((storeline1[i].find(storeline2[i], 0) != string::npos) && (storeline1[i].length() == storeline2[i].length()))
		{
			if (open)
			{
				if (IsChanged)
				{
					closepoint = i;

					if (closepoint != openpoint)
					{
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}
					}

					IsChanged = false;
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}
		}
		else
		{
			if (!open)
			{

				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = i;
				IsChanged = true;
				IsEdited = true;
				open = true;
			}
		}

		curline++;
		output.push_back(storeline2[i]);
	}
	
	if (open) // close unclosed edits
	{
		if (IsChanged)
		{
			closepoint = curline;

			if (closepoint != openpoint)
			{
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
			}

			IsChanged = false;
		}

		output.push_back("<!-- CLOSE -->");
		open = false;
	}

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
				if (output[i].find("<hkparam name=\"EventToFreezeBlendValue\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
						fwrite << "				<hkobject>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"id\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << "				<hkobject>" << "\n";
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
				{
					if ((output[i + 1].find("OPEN", 0) != string::npos) && (output[i + 1].find("MOD_CODE", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"EventToCrossBlend\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}

					if (output[i + 1].find("OPEN", 0) != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
				}
				else if (output[i].find("<hkparam name=\"fBlendParameter\">", 0) != string::npos)
				{
					if (((output[i - 1].find("OPEN", 0) != string::npos) && (output[i - 1].find("MOD_CODE", 0) != string::npos)) || (output[i - 1].find("ORIGINAL", 0) != string::npos))
					{
						fwrite << output[i] << "\n";
					}
					else
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
						fwrite << output[i] << "\n";
					}
				}
				else
				{
					fwrite << output[i] << "\n";
				}
			}

			fwrite << "		</hkobject>" << "\n";
			outputfile.close();

		}
		else
		{
			cout << "ERROR: Edit BSCyclicBlendTransitionGenerator Output Not Found (File: " << filename << ")" << endl;
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