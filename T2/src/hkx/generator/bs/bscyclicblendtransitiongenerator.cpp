#include <boost\thread.hpp>
#include "bscyclicblendtransitiongenerator.h"

using namespace std;

namespace cyclicblendtransitiongenerator
{
	const string key = "c";
	const string classname = "BSCyclicBlendTransitionGenerator";
	const string signature = "0x5119eb06";
}

string bscyclicblendtransitiongenerator::GetAddress()
{
	return address;
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
