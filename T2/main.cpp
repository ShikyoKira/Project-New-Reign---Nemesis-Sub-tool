#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <boost\date_time\posix_time\posix_time.hpp>
#include <boost\filesystem.hpp>
#include "Global.h"
#include "hkbbehaviorgraph.h"
#include "hkrootlevelcontainer.h"
#include "hkbstatemachine.h"
#include "hkbstatemachinestateinfo.h"
#include "hkbstatemachineeventpropertyarray.h"
#include "hkbmanualselectorgenerator.h"
#include "hkbvariablebindingset.h"
#include "hkbblendergenerator.h"
#include "hkbblendergeneratorchild.h"
#include "hkbclipgenerator.h"
#include "hkbcliptriggerarray.h"
#include "hkbstatemachinetransitioninfoarray.h"
#include "hkbbehaviorgraphdata.h"
#include "hkbvariablevalueset.h"
#include "hkbbehaviorgraphstringdata.h"
#include "hkbblendingtransitioneffect.h"
#include "hkbmodifiergenerator.h"
#include "hkbmodifierlist.h"
#include "hkbboneweightarray.h"
#include "hkbexpressioncondition.h"
#include "hkbstringeventpayload.h"
#include "bsboneswitchgenerator.h"
#include "bsboneswitchgeneratorbonedata.h"
#include "bscyclicblendtransitiongenerator.h"
#include "bsistatetagginggenerator.h"
#include "hkbtwistmodifier.h"
#include "hkbeventdrivenmodifier.h"
#include "bsisactivemodifier.h"
#include "bslimbikmodifier.h"
#include "bsinterpvaluemodifier.h"
#include "bsgettimestepmodifier.h"
#include "hkbfootikcontrolsmodifier.h"
#include "hkbgethandleonbonemodifier.h"
#include "hkbtransformvectormodifier.h"
#include "hkbproxymodifier.h"
#include "hkblookatmodifier.h"
#include "hkbmirrormodifier.h"
#include "hkbgetworldfrommodelmodifier.h"
#include "hkbsensehandlemodifier.h"
#include "hkbevaluateexpressionmodifier.h"
#include "hkbexpressiondataarray.h"
#include "hkbevaluatehandlemodifier.h"
#include "hkbattachmentmodifier.h"
#include "hkbattributemodifier.h"
#include "hkbcombinetransformsmodifier.h"
#include "hkbcomputerotationfromaxisanglemodifier.h"
#include "hkbcomputerotationtotargetmodifier.h"
#include "hkbeventsfromrangemodifier.h"
#include "hkbeventrangedataarray.h"
#include "hkbmovecharactermodifier.h"
#include "hkbextractragdollposemodifier.h"
#include "bsmodifyoncemodifier.h"
#include "bseventondeactivatemodifier.h"
#include "bseventeveryneventsmodifier.h"
#include "bsragdollcontactlistenermodifier.h"
#include "hkbpoweredragdollcontrolmodifier.h"
#include "bseventonfalsetotruemodifier.h"
#include "bsdirectatmodifier.h"
#include "bsdisttriggermodifier.h"
#include "bsdecomposevectormodifier.h"
#include "bscomputeaddboneanimmodifier.h"
#include "bstweenermodifier.h"
#include "bsistatemanagermodifier.h"
#include "hkbtimermodifier.h"
#include "hkbrotatecharactermodifier.h"
#include "hkbdampingmodifier.h"
#include "hkbdelayedmodifier.h"
#include "hkbgetupmodifier.h"
#include "hkbkeyframebonesmodifier.h"
#include "hkbboneindexarray.h"
#include "hkbcomputedirectionmodifier.h"
#include "hkbrigidbodyragdollcontrolsmodifier.h"
#include "bsspeedsamplermodifier.h"
#include "hkbdetectclosetogroundmodifier.h"
#include "bslookatmodifier.h"
#include "bstimermodifier.h"
#include "bspassbytargettriggermodifier.h"
#include "hkbhandikcontrolsmodifier.h"
#include "hkbbehaviorreferencegenerator.h"
#include "bssynchronizedclipgenerator.h"

using namespace std;

string GetClass(string id, string inputfile, bool compare); // get class

int GetFunctionLayer(int functionlayer, string tempadd, string preaddress); // get functionlayer

template <typename T>
void GoToMultiGenerator(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // multiple generators function

template <typename T>
void GoToGenerator(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // generator function

template <typename T>
void GoToVariableBinding(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // variablebindingset function

template <typename T>
void GoToModifier(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // modifier function

template <typename T>
void GoToMultiModifier(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // multiple modifier function

template <typename T>
void GoToWildcard(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // wildcard function

template <typename T>
void GoToTransitionBlender(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // transition blender function

template <typename T>
void GoToEnterNotify(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // enter notify event function

template <typename T>
void GoToExitNotify(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // enter notify event function

template <typename T>
void GoToTransition(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // transition array function

template <typename T>
void GoToBoneWeight(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // bone weight function

template <typename T>
void GoToData(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare); // data function

void Clearing(string file, bool edited); // clear and store file in vector
void ClearIgnore(string file1, string file2); // clear serialized ignore
void ClassIdentifier(string inputfile, string classname, string id, string preaddress, int functionlayer, bool compare); // idenfity class and record function
void EditClassIdentifier(string originalfile, string editedfile, string classname, string id); // export edited file
void Initialize(string originalfile); // initialize reading original file procedure
void InitializeComparing(string editedfile); // initialize comparing procedure
void GetEdits(); // getting edits from editedfile
void ModCode(); // enter author code
void DebugMode(); // debug on/off

string GetClass(string id, string inputfile, bool compare)
{
	string line;
	string tempID = id;
	vector<unordered_map<string, string>::iterator> iter;

	if (compare)
	{
		for (unordered_map<string, string>::iterator it = exchangeID.begin(); it != exchangeID.end(); it++)
		{
			if (it->second == id)
			{
				tempID = it->first;
			}

			if (it->second.length() < 1)
			{
				iter.push_back(it);
			}
		}
	}

	for (unsigned int i = 0; i < iter.size(); i++) // erase all empty containers
	{
		exchangeID.erase(iter[i]);
	}

	if (compare)
	{
		string classname = "<hkobject name=\"" + tempID;

		if (!FunctionLineEdited[tempID].empty())
		{
			usize size = FunctionLineEdited[tempID].size();

			for (int i = 0; i < size; i++)
			{
				string line = FunctionLineEdited[tempID][i];

				if (line.find(classname, 0) != string::npos)
				{
					int tempint = line.find("class") + 7;
					return line.substr(tempint, line.find("signature") - tempint - 2);
				}
			}
		}
		else
		{
			cout << "ERROR: GetClass Inputfile(File: " << inputfile << ", ID:" << id << ", tempID: " << tempID << ")" << endl;
			Error = true;
			return "null";
		}
	}
	else
	{
		string classname = "<hkobject name=\"" + tempID;

		if (!FunctionLineOriginal[tempID].empty())
		{
			usize size = FunctionLineOriginal[tempID].size();

			for (int i = 0; i < size; i++)
			{
				string line = FunctionLineOriginal[tempID][i];

				if (line.find(classname, 0) != string::npos)
				{
					int tempint = line.find("class") + 7;
					return line.substr(tempint, line.find("signature") - tempint - 2);
				}
			}
		}
		else
		{
			cout << "ERROR: GetClass Inputfile(File: " << inputfile << ", ID:" << id << ", tempID: " << tempID << ")" << endl;
			Error = true;
			return "null";
		}
	}
	
	cout << "ERROR: GetClass Not Found(File: " << inputfile << ", ID:" << id << ", tempID: " << tempID << ")" << endl;
	Error = true;

	return "null";
}

int GetFunctionLayer(int functionlayer, string tempadd, string preaddress)
{
	if (tempadd == preaddress)
	{
		return functionlayer;
	}
	else
	{
		return functionlayer + 1;
	}
}

template <typename T>
void GoToMultiGenerator(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	int tempchildren = ptr->GetChildren();
	usize tempint = 0;
	for (int i = 0; i < tempchildren; i++)
	{
		string nextclass = ptr->NextGenerator(i);

		if (!Error)
		{
			if (compare)
			{
				if (!exchangeID[nextclass].empty()) // ID change protector
				{
					nextclass = exchangeID[nextclass];
				}

				if (!IsForeign[parent[nextclass]])
				{
					ClassIdentifier(inputfile, GetClass(nextclass, inputfile, compare), nextclass, tempadd + to_string(tempint), GetFunctionLayer(0, tempadd, preaddress), compare);
				}
				else
				{
					ClassIdentifier(inputfile, GetClass(nextclass, inputfile, compare), nextclass, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

				}

				if (!IsForeign[nextclass])
				{
					tempint++;
				}
			}
			else
			{
				ClassIdentifier(inputfile, GetClass(nextclass, inputfile, compare), nextclass, tempadd + to_string(tempint), GetFunctionLayer(0, tempadd, preaddress), compare);

				tempint++;
			}
		}
	}
}

template <typename T>
void GoToGenerator(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string nextclass = ptr->NextGenerator();
	if (!Error)
	{
		if (!exchangeID[nextclass].empty()) // ID change protector
		{
			nextclass = exchangeID[nextclass];
		}

		ClassIdentifier(inputfile, GetClass(nextclass, inputfile, compare), nextclass, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToVariableBinding(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string variablebindingset = ptr->GetVariableBindingSet();
	if ((!ptr->IsBindingNull()) && (!Error))
	{
		if (!exchangeID[variablebindingset].empty()) // ID change protector
		{
			variablebindingset = exchangeID[variablebindingset];
		}

		ClassIdentifier(inputfile, "hkbVariableBindingSet", variablebindingset, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToModifier(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string modifier = ptr->GetModifier();
	if ((modifier.find("null", 0) == string::npos) && (!Error))
	{
		if (!exchangeID[modifier].empty()) // ID change protector
		{
			modifier = exchangeID[modifier];
		}

		ClassIdentifier(inputfile, GetClass(modifier, inputfile, compare), modifier, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToMultiModifier(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	int tempchildren = ptr->GetChildren();
	usize tempint = 0;
	for (int i = 0; i < tempchildren; i++)
	{
		string modifier = ptr->GetModifier(i);

		if (!Error)
		{
			if (compare)
			{
				if (!exchangeID[modifier].empty()) // ID change protector
				{
					modifier = exchangeID[modifier];
				}

				if (!IsForeign[parent[modifier]])
				{
					ClassIdentifier(inputfile, GetClass(modifier, inputfile, compare), modifier, tempadd + to_string(tempint), GetFunctionLayer(0, tempadd, preaddress), compare);
				}
				else
				{
					ClassIdentifier(inputfile, GetClass(modifier, inputfile, compare), modifier, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
				}

				if (!IsForeign[modifier])
				{
					tempint++;
				}
			}
			else
			{
				ClassIdentifier(inputfile, GetClass(modifier, inputfile, compare), modifier, tempadd + to_string(tempint), GetFunctionLayer(0, tempadd, preaddress), compare);

				tempint++;
			}
		}
	}
}

template <typename T>
void GoToWildcard(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string wildcard = ptr->GetWildcard();
	if ((!ptr->IsWildcardNull()) && (!Error))
	{
		if (!exchangeID[wildcard].empty()) // ID change protector
		{
			wildcard = exchangeID[wildcard];
		}

		ClassIdentifier(inputfile, "hkbStateMachineTransitionInfoArray", wildcard, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToTransitionBlender(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	int TransitionCount = ptr->GetTransitionCount();
	if ((TransitionCount != 0) && (!Error))
	{
		usize tempint = 0;
		for (int i = 0; i < TransitionCount; i++)
		{
			string transition = ptr->GetTransition(i);
						
			if (compare)
			{
				if (!exchangeID[transition].empty()) // ID change protector
				{
					transition = exchangeID[transition];
				}

				ClassIdentifier(inputfile, "hkbBlendingTransitionEffect", transition, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

				if (!IsForeign[transition])
				{
					tempint++;
				}
			}
			else
			{
				ClassIdentifier(inputfile, "hkbBlendingTransitionEffect", transition, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

				tempint++;
			}
		}
	}
}

template <typename T>
void GoToEnterNotify(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string enternotifyevent = ptr->GetEnterNotifyEvent();
	if ((!ptr->IsEnterNotifyEventNull()) && (!Error))
	{
		if (!exchangeID[enternotifyevent].empty()) // ID change protector
		{
			enternotifyevent = exchangeID[enternotifyevent];
		}

		ClassIdentifier(inputfile, "hkbStateMachineEventPropertyArray", enternotifyevent, tempadd + "0", GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToExitNotify(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string exitnotifyevent = ptr->GetExitNotifyEvent();
	if ((!ptr->IsExitNotifyEventNull()) && (!Error))
	{
		if (!exchangeID[exitnotifyevent].empty()) // ID change protector
		{
			exitnotifyevent = exchangeID[exitnotifyevent];
		}

		ClassIdentifier(inputfile, "hkbStateMachineEventPropertyArray", exitnotifyevent, tempadd + "1", GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToTransition(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string transitions = ptr->GetTransitions();
	if ((!ptr->IsTransitionsNull()) && (!Error))
	{
		if (!exchangeID[transitions].empty()) // ID change protector
		{
			transitions = exchangeID[transitions];
		}

		ClassIdentifier(inputfile, "hkbStateMachineTransitionInfoArray", transitions, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToBoneWeight(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string boneweights = ptr->GetBoneWeights();

	if ((!ptr->IsBoneWeightsNull()) && (!Error))
	{
		if (!exchangeID[boneweights].empty()) // ID change protector
		{
			boneweights = exchangeID[boneweights];
		}

		ClassIdentifier(inputfile, "hkbBoneWeightArray", boneweights, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToData(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string data = ptr->GetData();
	if (!Error)
	{
		if (!exchangeID[data].empty()) // ID change protector
		{
			data = exchangeID[data];
		}

		ClassIdentifier(inputfile, "hkbBehaviorGraphData", data, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToPayload(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	string payload = ptr->GetPayload();
	if ((!ptr->IsPayloadNull()) && (!Error))
	{
		if (!exchangeID[payload].empty()) // ID change protector
		{
			payload = exchangeID[payload];
		}

		ClassIdentifier(inputfile, "hkbStringEventPayload", payload, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
	}
}

template <typename T>
void GoToMultiPayload(T &ptr, string inputfile, string tempadd, string preaddress, int functionlayer, bool compare)
{
	int payloadcount = ptr->GetPayloadCount();
	if ((payloadcount != 0) && (!Error))
	{
		usize tempint = 0;
		for (int i = 0; i < payloadcount; i++)
		{
			if ((!ptr->IsPayloadNull(i)) && (!Error))
			{
				string payload = ptr->GetPayload(i);

				if (compare)
				{
					if (!exchangeID[payload].empty()) // ID change protector
					{
						payload = exchangeID[payload];
					}

					ClassIdentifier(inputfile, "hkbStringEventPayload", payload, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

					if (!IsForeign[payload])
					{
						tempint++;
					}
				}
				else
				{
					ClassIdentifier(inputfile, "hkbStringEventPayload", payload, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

					tempint++;
				}
			}
		}
	}
}

void testest(string filename, vector<string> &fv)
{
	boost::posix_time::ptime mtime1 = boost::posix_time::microsec_clock::local_time();
	
	string line;
	string namesearch = "SERIALIZE_IGNORED";
	int numline = 0;

	ifstream countbehaviorline(filename);
	if (countbehaviorline.is_open())
	{
		while (getline(countbehaviorline, line))
		{
			if (Error)
			{
				return;
			}

			if (line.find(namesearch, 0) == string::npos)
			{
				numline++;
			}
		}
		countbehaviorline.close();
	}
	else
	{
		cout << "ERROR: Fail to open file while clearing(File: " << filename << ")" << endl;
		Error = true;
	}

	fv.resize(numline);

	numline = 0;

	ifstream behaviorfile(filename);
	if (behaviorfile.is_open())
	{
		while (getline(behaviorfile, line))
		{
			if (Error)
			{
				return;
			}

			if (line.find(namesearch, 0) == string::npos)
			{
				fv[numline] = line;
				numline++;
			}
		}
		behaviorfile.close();
	}
	else
	{
		cout << "ERROR: Fail to open file while clearing(File: " << filename << ")" << endl;
		Error = true;
	}

	boost::posix_time::ptime mtime2 = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration mdiff = mtime2 - mtime1;

	if (Debug)
	{
		cout << "Reading time: " << mdiff.total_milliseconds() << endl;
		cout << mtime2 << endl << mtime1 << endl << endl;
	}


	boost::posix_time::ptime time1 = boost::posix_time::microsec_clock::local_time();

	ofstream output(filename);
	if (output.is_open())
	{
		for (unsigned int i = 0; i < fv.size(); i++)
		{
			output << fv[i] << "\n";
		}
		output.close();
	}
	else
	{
		cout << "ERROR: Fail to open file while clearing(File: " << filename << ")" << endl;
		Error = true;
	}

	boost::posix_time::ptime time2 = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = time2 - time1;

	if (Debug)
	{
		cout << "Writing time: " << diff.total_milliseconds() << endl;
		cout << time2 << endl << time1 << endl << endl;
	}

}

void Clearing(string filename, bool edited)
{
	boost::posix_time::ptime mtime1 = boost::posix_time::microsec_clock::local_time();

	string line;
	string namesearch = "SERIALIZE_IGNORED";
	bool record = false;

	ifstream countbehaviorline(filename);
	if (countbehaviorline.is_open())
	{
		string tempID;
		bool eventOpen = false;
		bool attriOption = false;
		bool varOpen = false;
		bool charOpen = false;
		int counter = 0;
		while (getline(countbehaviorline, line))
		{
			if (Error)
			{
				return;
			}

			if (edited)
			{
				if (line.find("#", 0) != string::npos)
				{
					if (line.find("<hkparam name=\"SyncAnimPrefix\">", 0) == string::npos)
					{
						usize tempint = 0;
						usize size = count(line.begin(), line.end(), '#');

						for (unsigned int i = 0; i < size; i++)
						{
							usize position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
							string tempID;

							if (tempint == -1)
							{
								string templine;

								if (line.find("signature", 0) != string::npos)
								{
									templine = line.substr(0, line.find("class"));
								}
								else if (line.find("toplevelobject", 0) != string::npos)
								{
									templine = line.substr(line.find("toplevelobject"), line.find(">"));
								}
								else
								{
									templine = line.substr(position, tempint - position - 1);
								}

								tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
							}
							else
							{
								tempID = line.substr(position, tempint - position - 1);
							}

							int tempLength = tempID.length();

							if (tempLength > 6)
							{
								cout << "ERROR: File contains invalid data(File: " << filename << ")" << endl;
								Error = true;
								return;
							}

							string strID = tempID.substr(1, tempLength - 1);
							int intID = stoi(strID);

							if (intID < 10000)
							{
								usize position2 = line.find(tempID);
								line.replace(position2, tempLength, "#9" + strID);
							}
						}
					}
				}

				if (line.find("<hkparam name=\"eventNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"eventNames\" numelements=\"0\">") == string::npos)
				{
					eventOpen = true;
				}
				else if (line.find("<hkparam name=\"attributeNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"attributeNames\" numelements=\"0\">") == string::npos)
				{
					attriOption = true;
				}
				else if (line.find("<hkparam name=\"variableNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"variableNames\" numelements=\"0\">") == string::npos)
				{
					varOpen = true;
				}
				else if (line.find("<hkparam name=\"characterPropertyNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"characterPropertyNames\" numelements=\"0\">") == string::npos)
				{
					charOpen = true;
				}
				else if (line.find("</hkparam>") != string::npos)
				{
					if (eventOpen)
					{
						eventOpen = false;
					}
					else if (attriOption)
					{
						attriOption = false;
					}
					else if (varOpen)
					{
						varOpen = false;
					}
					else if (charOpen)
					{
						charOpen = false;
					}

					counter = 0;
				}

				if (line.find("<hkcstring>") != string::npos && line.find("</hkcstring>") != string::npos)
				{
					if (eventOpen)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						eventID[to_string(counter++)] = templine;
					}
					else if (attriOption)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						attributeID[to_string(counter++)] = templine;
					}
					else if (varOpen)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						variableID[to_string(counter++)] = templine;
					}
					else if (charOpen)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						characterID[to_string(counter++)] = templine;
					}
				}
			}

			if ((line.find("<hkobject name=\"", 0) != string::npos) && (record == false))
			{
				tempID = line.substr(line.find("name") + 6, line.find("class") - line.find("name") - 8);
				record = true;
			}

			if (line.length() == 0)
			{
				record = false;
			}


			if ((record == true) && (line.find(namesearch, 0) == string::npos))
			{
				if (!edited)
				{
					FunctionLineOriginal[tempID].push_back(line);
				}
				else
				{
					FunctionLineEdited[tempID].push_back(line);
				}
			}
		}
		countbehaviorline.close();
	}
	else
	{
		cout << "ERROR: Fail to open file while clearing(File: " << filename << ")" << endl;
		Error = true;
	}

	boost::posix_time::ptime mtime2 = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration mdiff = mtime2 - mtime1;

	if (Debug)
	{
		cout << "Reading time: " << mdiff.total_milliseconds() << endl;
		cout << mtime2 << endl << mtime1 << endl << endl;
	}
}

void ClearIgnore(string file1, string file2)
{
	thread t1([=](){Clearing(file1, false); return 1; });
	Clearing(file2, true);

	t1.join();

	if (FunctionLineOriginal.size() < 5)
	{
		cout << "ERROR: Fail to read file (" + file1 + ")" << endl;
		Error = true;
	}

	if (FunctionLineEdited.size() < 5)
	{
		cout << "ERROR: Fail to read file (" + file1 + ")" << endl;
		Error = true;
	}
}

void ClassIdentifier(string inputfile, string classname, string id, string preaddress, int functionlayer, bool compare)
{
	if (classname.find("BSBoneSwitchGeneratorBoneData", 0) != string::npos)
	{
		bsboneswitchgeneratorbonedata bonedata(inputfile, id, preaddress, functionlayer, compare);
		bsboneswitchgeneratorbonedata* tempptr = &bonedata;
		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToBoneWeight(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSBoneSwitchGenerator", 0) != string::npos)
	{
		bsboneswitchgenerator boneswitch(inputfile, id, preaddress, functionlayer, compare);
		bsboneswitchgenerator* tempptr = &boneswitch;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			int children = tempptr->GetChildren();
			if ((children != 0) && (!Error))
			{
				usize tempint = 0;
				for (int i = 0; i < children; i++)
				{
					string bonedata = tempptr->GetBoneData(i);

					ClassIdentifier(inputfile, "BSBoneSwitchGeneratorBoneData", bonedata, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

					if (compare)
					{
						if (!IsForeign[bonedata])
						{
							tempint++;
						}
					}
					else
					{
						tempint++;
					}
				}
			}

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSCyclicBlendTransitionGenerator", 0) != string::npos)
	{
		
		bscyclicblendtransitiongenerator cyclicblend(inputfile, id, preaddress, functionlayer, compare);
		bscyclicblendtransitiongenerator* tempptr = &cyclicblend;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSiStateTaggingGenerator", 0) != string::npos)
	{
		bsistatetagginggenerator istate(inputfile, id, preaddress, functionlayer, compare);
		bsistatetagginggenerator* tempptr = &istate;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbBehaviorGraphStringData", 0) != string::npos)
	{
		hkbbehaviorgraphstringdata graphstringdata(inputfile, id, preaddress, functionlayer, compare);
	}

	else if (classname.find("hkbBehaviorGraphData", 0) != string::npos)
	{
		hkbbehaviorgraphdata graphdata(inputfile, id, preaddress, functionlayer, compare);
		hkbbehaviorgraphdata* tempptr = &graphdata;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			// variable initial values function
			string variableinitialvalues = tempptr->GetVariableInitialValues();

			// string data function
			string stringdata = tempptr->GetStringData();

			if (!Error)
			{
				ClassIdentifier(inputfile, "hkbVariableValueSet", variableinitialvalues, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
				
				ClassIdentifier(inputfile, "hkbBehaviorGraphStringData", stringdata, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("hkbBehaviorGraph", 0) != string::npos)
	{
		hkbbehaviorgraph graph(inputfile, id, preaddress, functionlayer, compare);
		hkbbehaviorgraph* tempptr = &graph;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// data function
			GoToData(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbBlenderGeneratorChild", 0) != string::npos)
	{
		hkbblendergeneratorchild blenderchild(inputfile, id, preaddress, functionlayer, compare);
		hkbblendergeneratorchild* tempptr = &blenderchild;
		
		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// bone weights function
			GoToBoneWeight(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbBlenderGenerator", 0) != string::npos)
	{
		hkbblendergenerator blender(inputfile, id, preaddress, functionlayer, compare);
		hkbblendergenerator* tempptr = &blender;
		
		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbBlendingTransitionEffect", 0) != string::npos)
	{
		hkbblendingtransitioneffect transitioneffect(inputfile, id, preaddress, functionlayer, compare);
		hkbblendingtransitioneffect* tempptr = &transitioneffect;
		
		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbBoneWeightArray", 0) != string::npos)
	{
		hkbboneweightarray boneweight(inputfile, id, preaddress, functionlayer, compare);
		hkbboneweightarray* tempptr = &boneweight;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbExpressionCondition", 0) != string::npos)
	{
		hkbexpressioncondition condition(inputfile, id, preaddress, functionlayer, compare);
	}

	else if (classname.find("hkbManualSelectorGenerator", 0) != string::npos)
	{
		hkbmanualselectorgenerator manualselector(inputfile, id, preaddress, functionlayer, compare);
		hkbmanualselectorgenerator* tempptr = &manualselector;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkRootLevelContainer", 0) != string::npos)
	{
		hkrootlevelcontainer root(inputfile, id, preaddress, 0, compare);
		hkrootlevelcontainer* tempptr = &root;
		

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();
			string generator = tempptr->NextGenerator();

			ClassIdentifier(inputfile, "hkbBehaviorGraph", generator, tempadd, functionlayer, compare);
		}
	}

	else if (classname.find("hkbStateMachineEventPropertyArray", 0) != string::npos)
	{
		hkbstatemachineeventpropertyarray eventarray(inputfile, id, preaddress, functionlayer, compare);
		hkbstatemachineeventpropertyarray* tempptr = &eventarray;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			// payload function
			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbStateMachineStateInfo", 0) != string::npos)
	{
		hkbstatemachinestateinfo state(inputfile, id, preaddress, functionlayer, compare);
		hkbstatemachinestateinfo* tempptr = &state;
		
		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// enter notify event function
			GoToEnterNotify(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// exit notify event function
			GoToExitNotify(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// transition function
			GoToTransition(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbStateMachineTransitionInfoArray", 0) != string::npos)
	{
		hkbstatemachinetransitioninfoarray statetransition(inputfile, id, preaddress, functionlayer, compare);
		hkbstatemachinetransitioninfoarray* tempptr = &statetransition;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			// transition function
			GoToTransitionBlender(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// condition function
			int ConditionCount = tempptr->GetConditionCount();
			if ((ConditionCount != 0) && (!Error))
			{
				usize tempint = 0;
				for (int i = 0; i < ConditionCount; i++)
				{
					string condition = tempptr->GetCondition(i);
										
					if (compare)
					{
						if (!exchangeID[condition].empty()) // ID change protector
						{
							condition = exchangeID[condition];
						}

						ClassIdentifier(inputfile, "hkbExpressionCondition", condition, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

						if (!IsForeign[condition])
						{
							tempint++;
						}
					}
					else
					{
						ClassIdentifier(inputfile, "hkbExpressionCondition", condition, tempadd + to_string(tempint), GetFunctionLayer(functionlayer, tempadd, preaddress), compare);

						tempint++;
					}
				}
			}
		}
	}

	else if (classname.find("hkbStateMachine", 0) != string::npos)
	{
		hkbstatemachine statemachine(inputfile, id, preaddress, functionlayer, compare);
		hkbstatemachine* tempptr = &statemachine;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// payload function
			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// wildcard function
			GoToWildcard(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbStringEventPayload", 0) != string::npos)
	{
		hkbstringeventpayload stringeventpayload(inputfile, id, preaddress, functionlayer, compare);
	}

	else if (classname.find("hkbVariableBindingSet", 0) != string::npos)
	{
		hkbvariablebindingset variablebindingset(inputfile, id, preaddress, functionlayer, compare);
	}

	else if (classname.find("hkbVariableValueSet", 0) != string::npos)
	{
		hkbvariablevalueset variablevalueset(inputfile, id, preaddress, functionlayer, compare);
	}

	else if (classname.find("hkbClipGenerator", 0) != string::npos)
	{
		hkbclipgenerator clipgenerator(inputfile, id, preaddress, functionlayer, compare);
		hkbclipgenerator* tempptr = &clipgenerator;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			// trigger function
			string triggers = tempptr->GetTriggers();
			if ((!tempptr->IsTriggersNull()) && (!Error))
			{
				ClassIdentifier(inputfile, "hkbClipTriggerArray", triggers, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("hkbClipTriggerArray", 0) != string::npos)
	{
		hkbcliptriggerarray cliptriggerarray(inputfile, id, preaddress, functionlayer, compare);
		hkbcliptriggerarray* tempptr = &cliptriggerarray;
		
		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();
			
			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}
	
	else if (classname.find("hkbBehaviorReferenceGenerator", 0) != string::npos)
	{
		hkbbehaviorreferencegenerator behaviorgenerator(inputfile, id, preaddress, functionlayer, compare);
		hkbbehaviorreferencegenerator* tempptr = &behaviorgenerator;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbModifierGenerator", 0) != string::npos)
	{
		hkbmodifiergenerator modifiergenerator(inputfile, id, preaddress, functionlayer, compare);
		hkbmodifiergenerator* tempptr = &modifiergenerator;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToModifier(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}
	
	else if (classname.find("hkbModifierList", 0) != string::npos)
	{
		hkbmodifierlist modifierlist(inputfile, id, preaddress, functionlayer, compare);
		hkbmodifierlist* tempptr = &modifierlist;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiModifier(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSSynchronizedClipGenerator", 0) != string::npos)
	{
		bssynchronizedclipgenerator synchronizedclip(inputfile, id, preaddress, functionlayer, compare);
		bssynchronizedclipgenerator* tempptr = &synchronizedclip;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToGenerator(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbTwistModifier", 0) != string::npos)
	{
		hkbtwistmodifier twistmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbtwistmodifier* tempptr = &twistmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbEventDrivenModifier", 0) != string::npos)
	{
		hkbeventdrivenmodifier eventdrivenmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbeventdrivenmodifier* tempptr = &eventdrivenmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToModifier(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			
		}
	}

	else if (classname.find("BSIsActiveModifier", 0) != string::npos)
	{
		bsisactivemodifier isactivemodifier(inputfile, id, preaddress, functionlayer, compare);
		bsisactivemodifier* tempptr = &isactivemodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSLimbIKModifier", 0) != string::npos)
	{
		bslimbikmodifier limbik(inputfile, id, preaddress, functionlayer, compare);
		bslimbikmodifier* tempptr = &limbik;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSInterpValueModifier", 0) != string::npos)
	{
		bsinterpvaluemodifier interpvaluemodifier(inputfile, id, preaddress, functionlayer, compare);
		bsinterpvaluemodifier* tempptr = &interpvaluemodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSGetTimeStepModifier", 0) != string::npos)
	{
		bsgettimestepmodifier gettimestep(inputfile, id, preaddress, functionlayer, compare);
		bsgettimestepmodifier* tempptr = &gettimestep;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbFootIkControlsModifier", 0) != string::npos)
	{
		hkbfootikcontrolsmodifier footik(inputfile, id, preaddress, functionlayer, compare);
		hkbfootikcontrolsmodifier* tempptr = &footik;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbGetHandleOnBoneModifier", 0) != string::npos)
	{
		hkbgethandleonbonemodifier gethandleonbone(inputfile, id, preaddress, functionlayer, compare);
		hkbgethandleonbonemodifier* tempptr = &gethandleonbone;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}
	
	else if (classname.find("hkbTransformVectorModifier", 0) != string::npos)
	{
		hkbtransformvectormodifier transformvector(inputfile, id, preaddress, functionlayer, compare);
		hkbtransformvectormodifier* tempptr = &transformvector;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbProxyModifier", 0) != string::npos)
	{
		hkbproxymodifier proxymodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbproxymodifier* tempptr = &proxymodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbLookAtModifier", 0) != string::npos)
	{
		hkblookatmodifier hkblookat(inputfile, id, preaddress, functionlayer, compare);
		hkblookatmodifier* tempptr = &hkblookat;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbMirrorModifier", 0) != string::npos)
	{
		hkbmirrormodifier mirrormodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbmirrormodifier* tempptr = &mirrormodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbGetWorldFromModelModifier", 0) != string::npos)
	{
		hkbgetworldfrommodelmodifier getworld(inputfile, id, preaddress, functionlayer, compare);
		hkbgetworldfrommodelmodifier* tempptr = &getworld;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbSenseHandleModifier", 0) != string::npos)
	{
		hkbsensehandlemodifier sensehandle(inputfile, id, preaddress, functionlayer, compare);
		hkbsensehandlemodifier* tempptr = &sensehandle;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbEvaluateExpressionModifier", 0) != string::npos)
	{
		hkbevaluateexpressionmodifier evaluateexpression(inputfile, id, preaddress, functionlayer, compare);
		hkbevaluateexpressionmodifier* tempptr = &evaluateexpression;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			if (!tempptr->IsExpressionNull())
			{
				string expression = tempptr->GetExpression();
				ClassIdentifier(inputfile, "hkbExpressionDataArray", expression, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}

	}

	else if (classname.find("hkbExpressionDataArray", 0) != string::npos)
	{
		hkbexpressiondataarray expressiondataarray(inputfile, id, preaddress, functionlayer, compare);
	}

	else if (classname.find("hkbEvaluateHandleModifier", 0) != string::npos)
	{
		hkbevaluatehandlemodifier evaluatehandle(inputfile, id, preaddress, functionlayer, compare);
		hkbevaluatehandlemodifier* tempptr = &evaluatehandle;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbAttachmentModifier", 0) != string::npos)
	{
		hkbattachmentmodifier attachmentmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbattachmentmodifier* tempptr = &attachmentmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbAttributeModifier", 0) != string::npos)
	{
		hkbattributemodifier attributemodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbattributemodifier* tempptr = &attributemodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbCombineTransformsModifier", 0) != string::npos)
	{
		hkbcombinetransformsmodifier combinetransforms(inputfile, id, preaddress, functionlayer, compare);
		hkbcombinetransformsmodifier* tempptr = &combinetransforms;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbComputeRotationFromAxisAngleModifier", 0) != string::npos)
	{
		hkbcomputerotationfromaxisanglemodifier computerotationfromaxis(inputfile, id, preaddress, functionlayer, compare);
		hkbcomputerotationfromaxisanglemodifier* tempptr = &computerotationfromaxis;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbComputeRotationToTargetModifier", 0) != string::npos)
	{
		hkbcomputerotationtotargetmodifier computerotationtotarget(inputfile, id, preaddress, functionlayer, compare);
		hkbcomputerotationtotargetmodifier* tempptr = &computerotationtotarget;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbEventsFromRangeModifier", 0) != string::npos)
	{
		hkbeventsfromrangemodifier eventsfromrange(inputfile, id, preaddress, functionlayer, compare);
		hkbeventsfromrangemodifier* tempptr = &eventsfromrange;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			if (!tempptr->IsEventRangesNull())
			{
				string eventranges = tempptr->GetEventRanges();
				ClassIdentifier(inputfile, "hkbEventRangeDataArray", eventranges, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("hkbEventRangeDataArray", 0) != string::npos)
	{
		hkbeventrangedataarray eventrangedata(inputfile, id, preaddress, functionlayer, compare);
		hkbeventrangedataarray* tempptr = &eventrangedata;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbMoveCharacterModifier", 0) != string::npos)
	{
		hkbmovecharactermodifier movecharactermodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbmovecharactermodifier* tempptr = &movecharactermodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbExtractRagdollPoseModifier", 0) != string::npos)
	{
		hkbextractragdollposemodifier extractragdoll(inputfile, id, preaddress, functionlayer, compare);
		hkbextractragdollposemodifier* tempptr = &extractragdoll;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSModifyOnceModifier", 0) != string::npos)
	{
		bsmodifyoncemodifier modifyoncemodifier(inputfile, id, preaddress, functionlayer, compare);
		bsmodifyoncemodifier* tempptr = &modifyoncemodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
			
			if (!tempptr->IsModifierNull(0))
			{
				string modifier = tempptr->GetModifier(0);
				ClassIdentifier(inputfile, GetClass(modifier, inputfile, compare), modifier, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}

			if (!tempptr->IsModifierNull(1))
			{
				string modifier = tempptr->GetModifier(1);
				ClassIdentifier(inputfile, GetClass(modifier, inputfile, compare), modifier, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("BSEventOnDeactivateModifier", 0) != string::npos)
	{
		bseventondeactivatemodifier eventondeactivate(inputfile, id, preaddress, functionlayer, compare);
		bseventondeactivatemodifier* tempptr = &eventondeactivate;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSEventEveryNEventsModifier", 0) != string::npos)
	{
		bseventeveryneventsmodifier eventeverynevents(inputfile, id, preaddress, functionlayer, compare);
		bseventeveryneventsmodifier* tempptr = &eventeverynevents;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSRagdollContactListenerModifier", 0) != string::npos)
	{
		bsragdollcontactlistenermodifier ragdollcontactlistener(inputfile, id, preaddress, functionlayer, compare);
		bsragdollcontactlistenermodifier* tempptr = &ragdollcontactlistener;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			if (!tempptr->IsBoneNull())
			{
				string bones = tempptr->GetBone();
				ClassIdentifier(inputfile, "hkbBoneIndexArray", bones, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("hkbPoweredRagdollControlsModifier", 0) != string::npos)
	{
		hkbpoweredragdollcontrolmodifier poweredragdoll(inputfile, id, preaddress, functionlayer, compare);
		hkbpoweredragdollcontrolmodifier* tempptr = &poweredragdoll;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSEventOnFalseToTrueModifier", 0) != string::npos)
	{
		bseventonfalsetotruemodifier eventonfalsetotrue(inputfile, id, preaddress, functionlayer, compare);
		bseventonfalsetotruemodifier* tempptr = &eventonfalsetotrue;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToMultiPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSDirectAtModifier", 0) != string::npos)
	{
		bsdirectatmodifier directatmodifier(inputfile, id, preaddress, functionlayer, compare);
		bsdirectatmodifier* tempptr = &directatmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSDistTriggerModifier", 0) != string::npos)
	{
		bsdisttriggermodifier distmodifier(inputfile, id, preaddress, functionlayer, compare);
		bsdisttriggermodifier* tempptr = &distmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSDecomposeVectorModifier", 0) != string::npos)
	{
		bsdecomposevectormodifier decomposevector(inputfile, id, preaddress, functionlayer, compare);
		bsdecomposevectormodifier* tempptr = &decomposevector;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSComputeAddBoneAnimModifier", 0) != string::npos)
	{
		bscomputeaddboneanimmodifier computeboneanim(inputfile, id, preaddress, functionlayer, compare);
		bscomputeaddboneanimmodifier* tempptr = &computeboneanim;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSTweenerModifier", 0) != string::npos)
	{
		bstweenermodifier tweenermodifier(inputfile, id, preaddress, functionlayer, compare);
		bstweenermodifier* tempptr = &tweenermodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSIStateManagerModifier", 0) != string::npos)
	{
		bsistatemanagermodifier istatemanager(inputfile, id, preaddress, functionlayer, compare);
		bsistatemanagermodifier* tempptr = &istatemanager;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbTimerModifier", 0) != string::npos)
	{
		hkbtimermodifier timer(inputfile, id, preaddress, functionlayer, compare);
		hkbtimermodifier* tempptr = &timer;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbRotateCharacterModifier", 0) != string::npos)
	{
		hkbrotatecharactermodifier rotatecharactermodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbrotatecharactermodifier* tempptr = &rotatecharactermodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbDampingModifier", 0) != string::npos)
	{
		hkbdampingmodifier dampingmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbdampingmodifier* tempptr = &dampingmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbDelayedModifier", 0) != string::npos)
	{
		hkbdelayedmodifier delayedmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbdelayedmodifier* tempptr = &delayedmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToModifier(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbGetUpModifier", 0) != string::npos)
	{
		hkbgetupmodifier getupmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbgetupmodifier* tempptr = &getupmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbKeyframeBonesModifier", 0) != string::npos)
	{
		hkbkeyframebonesmodifier keybonesmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbkeyframebonesmodifier* tempptr = &keybonesmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();
			
			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			if (!tempptr->IsKeyframedBonesListNull())
			{
				string keyframedboneslist = tempptr->GetKeyframedBonesList();
				ClassIdentifier(inputfile, "hkbBoneIndexArray", keyframedboneslist, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("hkbBoneIndexArray", 0) != string::npos)
	{
		hkbboneindexarray boneindex(inputfile, id, preaddress, functionlayer, compare);
		hkbboneindexarray* tempptr = &boneindex;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbComputeDirectionModifier", 0) != string::npos)
	{
		hkbcomputedirectionmodifier computedirection(inputfile, id, preaddress, functionlayer, compare);
		hkbcomputedirectionmodifier* tempptr = &computedirection;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbRigidBodyRagdollControlsModifier", 0) != string::npos)
	{
		hkbrigidbodyragdollcontrolsmodifier rigidbodyragdoll(inputfile, id, preaddress, functionlayer, compare);
		hkbrigidbodyragdollcontrolsmodifier* tempptr = &rigidbodyragdoll;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			if (!tempptr->IsBonesNull())
			{
				string bones = tempptr->GetBones();
				ClassIdentifier(inputfile, "hkbBoneIndexArray", bones, tempadd, GetFunctionLayer(functionlayer, tempadd, preaddress), compare);
			}
		}
	}

	else if (classname.find("BSSpeedSamplerModifier", 0) != string::npos)
	{
		bsspeedsamplermodifier speedsampler(inputfile, id, preaddress, functionlayer, compare);
		bsspeedsamplermodifier* tempptr = &speedsampler;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbDetectCloseToGroundModifier", 0) != string::npos)
	{
		hkbdetectclosetogroundmodifier detectoclosetoground(inputfile, id, preaddress, functionlayer, compare);
		hkbdetectclosetogroundmodifier* tempptr = &detectoclosetoground;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSLookAtModifier", 0) != string::npos)
	{
		bslookatmodifier lookatmodifier(inputfile, id, preaddress, functionlayer, compare);
		bslookatmodifier* tempptr = &lookatmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSTimerModifier", 0) != string::npos)
	{
		bstimermodifier timermodifier(inputfile, id, preaddress, functionlayer, compare);
		bstimermodifier* tempptr = &timermodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("BSPassByTargetTriggerModifier", 0) != string::npos)
	{
		bspassbytargettriggermodifier passbytargetmodifier(inputfile, id, preaddress, functionlayer, compare);
		bspassbytargettriggermodifier* tempptr = &passbytargetmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);

			GoToPayload(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else if (classname.find("hkbHandIkControlsModifier", 0) != string::npos)
	{
		hkbhandikcontrolsmodifier handikmodifier(inputfile, id, preaddress, functionlayer, compare);
		hkbhandikcontrolsmodifier* tempptr = &handikmodifier;

		if (!tempptr->IsNegate())
		{
			string tempadd = tempptr->GetAddress();

			GoToVariableBinding(tempptr, inputfile, tempadd, preaddress, functionlayer, compare);
		}
	}

	else
	{
		cout << "ERROR: Class Not Found (File: " << inputfile << ", Classname: " << classname << ", ID: " << id << ", Preaddress: " << preaddress << ")" << endl;
		Error = true;
	}
}

void EditClassIdentifier(string originalfile, string editedfile, string classname, string id)
{
	if (classname.find("BSBoneSwitchGeneratorBoneData", 0) != string::npos)
	{
		BSBoneSwitchGeneratorBoneDataExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSBoneSwitchGenerator", 0) != string::npos)
	{
		BSBoneSwitchGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSCyclicBlendTransitionGenerator", 0) != string::npos)
	{
		BSCyclicBlendTransitionGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSiStateTaggingGenerator", 0) != string::npos)
	{
		BSiStateTaggingGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBehaviorGraphStringData", 0) != string::npos)
	{
		hkbBehaviorGraphStringDataExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBehaviorGraphData", 0) != string::npos)
	{
		hkbBehaviorGraphDataExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBehaviorGraph", 0) != string::npos)
	{
		hkbBehaviorGraphExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBlenderGeneratorChild", 0) != string::npos)
	{
		hkbBlenderGeneratorChildExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBlenderGenerator", 0) != string::npos)
	{
		hkbBlenderGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBlendingTransitionEffect", 0) != string::npos)
	{
		hkbBlendingTransitionEffectExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBoneWeightArray", 0) != string::npos)
	{
		hkbBoneWeightArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbExpressionCondition", 0) != string::npos)
	{
		hkbExpressionConditionExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbManualSelectorGenerator", 0) != string::npos)
	{
		hkbManualSelectorGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkRootLevelContainer", 0) != string::npos)
	{
		hkRootLevelContainerExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbStateMachineEventPropertyArray", 0) != string::npos)
	{
		hkbStateMachineEventPropertyArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbStateMachineStateInfo", 0) != string::npos)
	{
		hkbStateMachineStateInfoExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbStateMachineTransitionInfoArray", 0) != string::npos)
	{
		hkbStateMachineTransitionInfoArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbStateMachine", 0) != string::npos)
	{
		hkbStateMachineExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbStringEventPayload", 0) != string::npos)
	{
		hkbStringEventPayloadExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbVariableBindingSet", 0) != string::npos)
	{
		hkbVariableBindingSetExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbVariableValueSet", 0) != string::npos)
	{
		hkbVariableValueSetExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbClipGenerator", 0) != string::npos)
	{
		hkbClipGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbClipTriggerArray", 0) != string::npos)
	{
		hkbClipTriggerArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBehaviorReferenceGenerator", 0) != string::npos)
	{
		hkbBehaviorReferenceGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbModifierGenerator", 0) != string::npos)
	{
		hkbModifierGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbModifierList", 0) != string::npos)
	{
		hkbModifierListExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSSynchronizedClipGenerator", 0) != string::npos)
	{
		BSSynchronizedClipGeneratorExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbTwistModifier", 0) != string::npos)
	{
		hkbTwistModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbEventDrivenModifier", 0) != string::npos)
	{
		hkbEventDrivenModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSIsActiveModifier", 0) != string::npos)
	{
		BSIsActiveModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSLimbIKModifier", 0) != string::npos)
	{
		BSLimbIKModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSInterpValueModifier", 0) != string::npos)
	{
		BSInterpValueModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSGetTimeStepModifier", 0) != string::npos)
	{
		BSGetTimeStepModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbFootIkControlsModifier", 0) != string::npos)
	{
		hkbFootIkControlsModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbGetHandleOnBoneModifier", 0) != string::npos)
	{
		hkbGetHandleOnBoneModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbTransformVectorModifier", 0) != string::npos)
	{
		hkbTransformVectorModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbProxyModifier", 0) != string::npos)
	{
		hkbProxyModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbLookAtModifier", 0) != string::npos)
	{
		hkbLookAtModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbMirrorModifier", 0) != string::npos)
	{
		hkbMirrorModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbGetWorldFromModelModifier", 0) != string::npos)
	{
		hkbGetWorldFromModelModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbSenseHandleModifier", 0) != string::npos)
	{
		hkbSenseHandleModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbEvaluateExpressionModifier", 0) != string::npos)
	{
		hkbEvaluateExpressionModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbExpressionDataArray", 0) != string::npos)
	{
		hkbExpressionDataArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbEvaluateHandleModifier", 0) != string::npos)
	{
		hkbEvaluateHandleModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbAttachmentModifier", 0) != string::npos)
	{
		hkbAttachmentModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbAttributeModifier", 0) != string::npos)
	{
		hkbAttributeModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbCombineTransformsModifier", 0) != string::npos)
	{
		hkbCombineTransformsModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbComputeRotationFromAxisAngleModifier", 0) != string::npos)
	{
		hkbComputeRotationFromAxisAngleModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbComputeRotationToTargetModifier", 0) != string::npos)
	{
		hkbComputeRotationToTargetModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbEventsFromRangeModifier", 0) != string::npos)
	{
		hkbEventsFromRangeModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbEventRangeDataArray", 0) != string::npos)
	{
		hkbEventRangeDataArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbMoveCharacterModifier", 0) != string::npos)
	{
		hkbMoveCharacterModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbExtractRagdollPoseModifier", 0) != string::npos)
	{
		hkbExtractRagdollPoseModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSModifyOnceModifier", 0) != string::npos)
	{
		BSModifyOnceModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSEventOnDeactivateModifier", 0) != string::npos)
	{
		BSEventOnDeactivateModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSEventEveryNEventsModifier", 0) != string::npos)
	{
		BSEventEveryNEventsModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSRagdollContactListenerModifier", 0) != string::npos)
	{
		BSRagdollContactListenerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbPoweredRagdollControlsModifier", 0) != string::npos)
	{
		hkbPoweredRagdollControlsModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSEventOnFalseToTrueModifier", 0) != string::npos)
	{
		BSEventOnFalseToTrueModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSDirectAtModifier", 0) != string::npos)
	{
		BSDirectAtModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSDistTriggerModifier", 0) != string::npos)
	{
		BSDistTriggerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSDecomposeVectorModifier", 0) != string::npos)
	{
		BSDecomposeVectorModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSComputeAddBoneAnimModifier", 0) != string::npos)
	{
		BSComputeAddBoneAnimModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSTweenerModifier", 0) != string::npos)
	{
		BSTweenerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSIStateManagerModifier", 0) != string::npos)
	{
		BSIStateManagerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbTimerModifier", 0) != string::npos)
	{
		hkbTimerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbRotateCharacterModifier", 0) != string::npos)
	{
		hkbRotateCharacterModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbDampingModifier", 0) != string::npos)
	{
		hkbDampingModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbDelayedModifier", 0) != string::npos)
	{
		hkbDelayedModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbGetUpModifier", 0) != string::npos)
	{
		hkbGetUpModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbKeyframeBonesModifier", 0) != string::npos)
	{
		hkbKeyframeBonesModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbBoneIndexArray", 0) != string::npos)
	{
		hkbBoneIndexArrayExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbComputeDirectionModifier", 0) != string::npos)
	{
		hkbComputeDirectionModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbRigidBodyRagdollControlsModifier", 0) != string::npos)
	{
		hkbRigidBodyRagdollControlsModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSSpeedSamplerModifier", 0) != string::npos)
	{
		BSSpeedSamplerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbDetectCloseToGroundModifier", 0) != string::npos)
	{
		hkbDetectCloseToGroundModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSLookAtModifier", 0) != string::npos)
	{
		BSLookAtModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSTimerModifier", 0) != string::npos)
	{
		BSTimerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("BSPassByTargetTriggerModifier", 0) != string::npos)
	{
		BSPassByTargetTriggerModifierExport(originalfile, editedfile, id);
	}

	else if (classname.find("hkbHandIkControlsModifier", 0) != string::npos)
	{
		hkbHandIkControlsModifierExport(originalfile, editedfile, id);
	}

	else
	{
		cout << "ERROR: Edit Class Not Found (Original File: " << originalfile << ", Edited File: " << editedfile << ", Classname: " << classname << ", ID: " << id << ")" << endl;
		Error = true;
		return;
	}
}

void Initialize(string originalfilename)
{
	string line;
	string search = "toplevelobject";
	ifstream behaviorfile(originalfilename);

	if (behaviorfile.is_open())
	{
		while (getline(behaviorfile, line))
		{
			if (line.find(search, 0) != string::npos)
			{
				int tempint = line.find(search) + 16;
				search = line.substr(tempint, line.length() - tempint - 2);
				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Initialization Input(File: " << originalfilename << ")" << endl;
		Error = true;
	}

	ClassIdentifier(originalfilename, "hkRootLevelContainer", search, "root=", 0, false);

	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl;
	}
}

void ContainerClearing()
{
	if (Debug)
	{
		cout << "Clearing container" << endl;
	}

	idcount.clear();
	referencingIDs.clear();
	exchangeID.clear();
	IsExist.clear();

	if (Debug)
	{
		cout << "Clearing Done" << endl;
	}
}

void InitializeComparing(string editedfilename)
{
	ContainerClearing();

	if ((Debug) && (!Error))
	{
		cout << endl << endl << endl << endl << "Initializing Phase 2....." << endl << endl << endl;
		Sleep(1000);
	}

	if (Error)
	{
		return;
	}

	string line;
	string search = "toplevelobject";
	ifstream behaviorfile(editedfilename);

	if (behaviorfile.is_open())
	{
		while (getline(behaviorfile, line))
		{
			if (line.find(search, 0) != string::npos)
			{
				int tempint = line.find(search) + 16;
				search = line.substr(tempint, line.length() - tempint - 2);
				string tempID = search.substr(1, search.length() - 1);
				if (stoi(tempID) < 10000)
				{
					search = "#9" + tempID;
				}
				break;
			}
		}
	}
	else
	{
		cout << "ERROR: Initialization Input(File: " << editedfilename << ")" << endl;
		Error = true;
	}

	ClassIdentifier(editedfilename, "hkRootLevelContainer", search, "root=", 0, true);

	if (Debug == true)
	{
		cout << "--------------------------------------------------------------" << endl;
	}
}

void GetEdits()
{
	if ((Debug) && (!Error))
	{
		cout << "Identifying changes made to the behavior" << endl;
	}
	
	for (unordered_map<string, bool>::iterator it = IsExist.begin(); it != IsExist.end(); it++)
	{
		if (it->second == true)
		{
			string id = it->first;
			string originalfile = "temp/" + id + ".txt";
			string editedfile = "new/" + id + ".txt";
			if (IsFileExist(originalfile) && IsFileExist(editedfile))
			{
				EditClassIdentifier(originalfile, editedfile, GetClass(id, originalfile, false), id);
			}
			else if (!IsFileExist(originalfile) && IsFileExist(editedfile))
			{
				vector<string> storeline;
				string line;
				ifstream input(editedfile);
				if (input.is_open())
				{
					while (getline(input, line))
					{
						storeline.push_back(line);
					}
					input.close();
				}
				else
				{
					cout << "ERROR: Fail to copy edited file (File: " << editedfile << ", ModCode: " << modcode << ")" << endl;
					Error = true;
					return;
				}

				for (unsigned int j = 0; j < storeline.size(); j++) // changing newID to modCode ID
				{
					if (storeline[j].find("#", 0) != string::npos)
					{
						usize tempint = 0;
						int position = 0;
						usize size = count(storeline[j].begin(), storeline[j].end(), '#');
						for (unsigned int i = 0; i < size; i++)
						{
							position = storeline[j].find("#", tempint);
							tempint = storeline[j].find("#", position + 1);
							string tempID;

							if (tempint == -1)
							{
								string templine;

								if (storeline[j].find("signature", 0) != string::npos)
								{
									templine = storeline[j].substr(0, storeline[j].find("class"));
								}
								else if (line.find("toplevelobject", 0) != string::npos)
								{
									templine = storeline[j].substr(storeline[j].find("toplevelobject"), storeline[j].find(">"));
								}
								else
								{
									templine = storeline[j].substr(position, tempint - position - 1);
								}

								tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
							}
							else
							{
								tempID = storeline[j].substr(position, tempint - position - 1);
							}

							int tempLength = tempID.length();
							string strID = tempID.substr(1, tempLength - 1);
							int intID = stoi(strID);

							if (intID > 10000)
							{
								int position2 = storeline[j].find(tempID);
								string modID;

								if (!newID[tempID].empty())
								{
									modID = newID[tempID];
								}
								else
								{
									modID = "#" + modcode + "$" + to_string(functioncount);
									newID[tempID] = modID;
									functioncount++;
								}

								storeline[j].replace(position2, tempLength, modID);
							}
						}
					}
				}

				string modID2;

				if (!newID[id].empty())
				{
					modID2 = newID[id];
				}
				else
				{
					modID2 = "#" + modcode + "$" + to_string(functioncount);
					newID[id] = modID2;
					functioncount++;
				}

				ofstream output("cache/" + modcode + "/" + shortFileName + "/" + modID2 + ".txt"); // changing newID to modCode ID
				if (output.is_open())
				{
					for (unsigned int i = 0; i < storeline.size(); i++)
					{
						output << storeline[i] << "\n";
					}
					output.close();
				}
				else
				{
					cout << "ERROR: Fail to paste edited file (File: " << editedfile << ", ModCode: " << modcode << ")" << endl;
					Error = true;
					return;
				}

			}
			else
			{
				cout << "ERROR: Missing File (File: " << editedfile << ")" << endl;
				Error = true;
				return;
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << "Identification of changes is complete" << endl;
	}
}

void ModCode()
{
	string tempcode;
	system("cls");
	cout << "Mod code must be at least 4 but not more than 6 characters long and must not be \"vanilla\"" << endl << "Following characters cannot be used: \\ / : * ? \" < > |" << endl << endl;
	cout << "Enter \"-1\" to exit" << endl << "--------------------------------------------------------------------------------" << endl;
	cout << "Enter your unique mod code: ";
	cin >> tempcode;
	if (((tempcode.length() < 4) && (tempcode != "-1")) || (tempcode.length() > 6) || (tempcode.find("\\", 0) != string::npos) || (tempcode.find("/", 0) != string::npos) || (tempcode.find(":", 0) != string::npos) || (tempcode.find("*", 0) != string::npos) || (tempcode.find("?", 0) != string::npos) || (tempcode.find("\"", 0) != string::npos) || (tempcode.find("<", 0) != string::npos) || (tempcode.find(">", 0) != string::npos) || (tempcode.find("|", 0) != string::npos) || (tempcode == "vanilla"))
	{
		ModCode();
	}
	else
	{
		modcode = tempcode;
		cout << endl << "--------------------------------------------------------------------------------" << endl;
	}
}

void DebugMode()
{
	cout << "Do you want to turn on debug mode? (Y/N)";
	string tempstr;
	cin >> tempstr;
	if ((tempstr == "y") || (tempstr == "Y"))
	{
		Debug = true;
		cout << endl;
		return;
	}
	else if ((tempstr == "n") || (tempstr == "N"))
	{
		Debug = false;
		return;
	}
	DebugMode();
}

void test()
{
	boost::posix_time::ptime time3 = boost::posix_time::microsec_clock::local_time();
	
	modcode = "asdf";
	targetfilename = "output.txt";

	hkbBehaviorGraphDataExport("temp/#0087.txt", "new/#0087.txt", "#0087");
	

	boost::posix_time::ptime time4 = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = time4 - time3;

	cout << "Total time: " << diff.total_milliseconds() << " milliseconds" << endl;
	cout << time4 << endl << time3 << endl << endl;

}

boost::posix_time::ptime time1 = boost::posix_time::microsec_clock::local_time();

void start()
{

	targetfilename = "output.txt";
	shortFileName = targetfilename.substr(0, targetfilename.find_last_of("."));
	targetfilenameedited = "output2.txt";
	shortFileNameEdited = targetfilenameedited.substr(0, targetfilenameedited.find_last_of("."));
	string directory = "cache/";


	string filename = "new/" + targetfilenameedited;

	if (IsFileExist(filename))
	{
		if (remove(filename.c_str()) != 0)
		{
			perror("Error deleting file");
			Error = true;
		}
	}

	thread t([=](){ClearIgnore(targetfilename, targetfilenameedited); return 1; });
	
	if (Error)
	{
		cout << "ERROR: Fail to complete processing files" << endl;
		t.join();
		return;
	}

	ModCode();

	if (modcode == "-1")
	{
		Error = true;
		t.join();
		exit(0);
	}

	DebugMode();

	time1 = boost::posix_time::microsec_clock::local_time();

	if (Error)
	{
		cout << "ERROR: Fail to complete processing files" << endl;
		t.join();
		return;
	}

	t.join();

	string newfolder = "new";
	string tempfolder = "temp";

	if ((CreateDirectory(directory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) && (CreateDirectory(newfolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) && (CreateDirectory(tempfolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
	{
		if (CreateDirectory((directory + modcode).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			if (CreateDirectory((directory + modcode + "/" + shortFileName).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
			{
				Initialize(targetfilename);
				
				if (Error)
				{
					cout << "ERROR: Fail to complete processing files" << endl;
					return;
				}
				
				InitializeComparing(targetfilenameedited);
			}
			else
			{
				cout << "ERROR: Fail to create folder for target file (folder: " << shortFileName << ")" << endl;
				Error = true;
			}
		}
		else
		{
			cout << "ERROR: Fail to create directory for target modcode (ModCode: " << modcode << ")" << endl;
			Error = true;
		}
	}
	else
	{
		cout << "ERROR: Fail to create cache folder for Nemesis (Folder: " << directory << ")" << endl;
		Error = true;
	}
	

	if (Error)
	{
		cout << "ERROR: Fail to complete processing files" << endl;
		return;
	}

	if (IsFileExist(filename))
	{
		if (remove(filename.c_str()) != 0)
		{
			perror("Error deleting file");
			Error = true;
			return;
		}
	}
	
	GetEdits();
	
	if ((!Debug) && (!Error))
	{
		boost::filesystem::path newPath("new/");
		boost::filesystem::path tempPath("temp/");
		boost::filesystem::remove_all(newPath);
		boost::filesystem::remove_all(tempPath);
	}

	if (!Error)
	{
		for (int i = 0; i < 15; i++)
		{
			cout << endl << endl;
		}

		cout << endl << "                                      DONE!" << endl;


		for (int i = 0; i < 10; i++)
		{
			cout << endl;
		}
	}
	else
	{
		cout << "ERROR: Fail to complete processing files" << endl;
	}

}

int main()
{

	// test();

	start();
	
	if (!Error)
	{
		boost::posix_time::ptime time2 = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration diff = time2 - time1;

		double duration = double(diff.total_milliseconds());

		cout << "Total processing time: " << duration / 1000 << " seconds" << endl;
	}
	

	cin.sync();
	cin.get();

	return 0;
}









