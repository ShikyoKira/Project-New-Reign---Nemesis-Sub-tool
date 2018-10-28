#include "hkbcliptriggerarray.h"
#include "Global.h"
#include "highestscore.h"
#include "boolstring.h"

using namespace std;

struct triggerinfo
{
	bool proxy = true;

	string localtime;
	string id;
	string payload;
	bool relativetoend;
	bool acyclic;
	bool isannotation;
};

void inputTrigger(vector<string>& input, shared_ptr<triggerinfo> trigger);
void triggerInfoProcess(string line, vector<shared_ptr<triggerinfo>>& triggers, shared_ptr<triggerinfo>& curTrigger);
bool matchScoring(vector<shared_ptr<triggerinfo>>& ori, vector<shared_ptr<triggerinfo>>& edit, string id);

hkbcliptriggerarray::hkbcliptriggerarray(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "w" + to_string(functionlayer) + ">";

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
}

void hkbcliptriggerarray::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbClipTriggerArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbClipTriggerArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbClipTriggerArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcliptriggerarray::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbClipTriggerArray(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

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
		cout << "ERROR: hkbClipTriggerArray Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			addressChange.erase(addressChange.find(address));
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbClipTriggerArray(newID: " << id << ") with hkbClipTriggerArray(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID
		
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
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbClipTriggerArray(newID: " << id << ") with hkbClipTriggerArray(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbClipTriggerArray(ID: " << id << ") is complete!" << endl;
	}
}

void hkbcliptriggerarray::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbClipTriggerArray(ID: " << id << ") has been initialized!" << endl;
	}

	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(31, line.find("</hkparam>") - 31));

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
		cout << "ERROR: Dummy hkbClipTriggerArray Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbClipTriggerArray(ID: " << id << ") is complete!" << endl;
	}
}

int hkbcliptriggerarray::GetPayloadCount()
{
	return int(payload.size());
}

string hkbcliptriggerarray::GetPayload(int child)
{
	return payload[child];
}

bool hkbcliptriggerarray::IsPayloadNull(int child)
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

string hkbcliptriggerarray::GetAddress()
{
	return address;
}

bool hkbcliptriggerarray::IsNegate()
{
	return IsNegated;
}

void hkbClipTriggerArrayExport(string id)
{
	// stage 1 reading
	vector<shared_ptr<triggerinfo>> oriTriggers;
	shared_ptr<triggerinfo> curTrigger;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			triggerInfoProcess(FunctionLineTemp[id][i], oriTriggers, curTrigger);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbClipTriggerArray Input Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curTrigger)
	{
		curTrigger->proxy = false;
		oriTriggers.push_back(curTrigger);
	}

	curTrigger = nullptr;
	vector<shared_ptr<triggerinfo>> newTriggers;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			triggerInfoProcess(FunctionLineNew[id][i], newTriggers, curTrigger);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return;
	}

	if (curTrigger)
	{
		curTrigger->proxy = false;
		newTriggers.push_back(curTrigger);
	}

	if (!matchScoring(oriTriggers, newTriggers, id))
	{
		return;
	}
	
	// stage 2 identify edits
	vector<string> output;
	bool IsEdited = false;

	output.push_back(FunctionLineTemp[id][0]);

	if (FunctionLineTemp[id][1] != FunctionLineNew[id][1])
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back(FunctionLineNew[id][1]);
		output.push_back("<!-- ORIGINAL -->");
		output.push_back(FunctionLineTemp[id][1]);
		output.push_back("<!-- CLOSE -->");
		IsEdited = true;
	}
	else
	{
		output.push_back(FunctionLineNew[id][1]);
	}

	for (unsigned int i = 0; i < oriTriggers.size(); i++)
	{
		vector<string> storeline;
		bool open = false;

		if (newTriggers[i]->proxy)
		{
			vector<string> instore;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			bool nobreak = true;

			while (i < oriTriggers.size())
			{
				if (!newTriggers[i]->proxy)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), instore.begin(), instore.end());
					output.push_back("<!-- CLOSE -->");
					nobreak = false;
					--i;
					break;
				}

				int add = 0;

				while (add < 12)
				{
					output.push_back("");
					++add;
				}

				inputTrigger(instore, oriTriggers[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());
				output.push_back("<!-- CLOSE -->");
			}
		}
		else if (!oriTriggers[i]->proxy)
		{
			output.push_back("				<hkobject>");

			if (oriTriggers[i]->localtime != newTriggers[i]->localtime)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"localTime\">" + newTriggers[i]->localtime + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.push_back("					<hkparam name=\"localTime\">" + oriTriggers[i]->localtime + "</hkparam>");
				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"localTime\">" + oriTriggers[i]->localtime + "</hkparam>");
			}

			output.push_back("					<hkparam name=\"event\">");
			output.push_back("						<hkobject>");

			if (oriTriggers[i]->id != newTriggers[i]->id)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("							<hkparam name=\"id\">" + newTriggers[i]->id + "</hkparam>");
				open = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"id\">" + oriTriggers[i]->id + "</hkparam>");
			}

			if (oriTriggers[i]->payload != newTriggers[i]->payload)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("							<hkparam name=\"payload\">" + newTriggers[i]->payload + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"payload\">" + oriTriggers[i]->payload + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					IsEdited = true;
				}
				else
				{
					output.push_back("							<hkparam name=\"payload\">" + newTriggers[i]->payload + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + oriTriggers[i]->id + "</hkparam>");
					output.push_back("							<hkparam name=\"payload\">" + oriTriggers[i]->payload + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + oriTriggers[i]->id + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back("							<hkparam name=\"payload\">" + oriTriggers[i]->payload + "</hkparam>");
			}

			output.push_back("						</hkobject>");
			output.push_back("					</hkparam>");

			if (oriTriggers[i]->relativetoend != newTriggers[i]->relativetoend)
			{
				open = true;
				IsEdited = true;
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(newTriggers[i]->relativetoend) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(oriTriggers[i]->relativetoend) + "</hkparam>");
			}
			else
			{
				output.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(oriTriggers[i]->relativetoend) + "</hkparam>");
			}

			if (oriTriggers[i]->acyclic != newTriggers[i]->acyclic)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"acyclic\">" + from_bool(newTriggers[i]->acyclic) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"acyclic\">" + from_bool(oriTriggers[i]->acyclic) + "</hkparam>");
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					storeline.clear();
					open = false;
				}

				output.push_back("					<hkparam name=\"acyclic\">" + from_bool(oriTriggers[i]->acyclic) + "</hkparam>");
			}

			if (oriTriggers[i]->isannotation != newTriggers[i]->isannotation)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(newTriggers[i]->isannotation) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(oriTriggers[i]->isannotation) + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
			}
			else
			{
				if (open)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
				}

				output.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(oriTriggers[i]->isannotation) + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;

			while (i < oriTriggers.size())
			{
				inputTrigger(output, newTriggers[i]);
				++i;
			}

			output.push_back("<!-- CLOSE -->");
		}
	}
	
	output.push_back("			</hkparam>");
	output.push_back("		</hkobject>");
	NemesisReaderFormat(output);

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
			cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (File: " << filename << ")" << endl;
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

void triggerInfoProcess(string line, vector<shared_ptr<triggerinfo>>& triggers, shared_ptr<triggerinfo>& curTrigger)
{
	if (line.find("<hkparam name=\"localTime\">") != string::npos)
	{
		if (curTrigger)
		{
			curTrigger->proxy = false;
			triggers.push_back(curTrigger);
		}

		curTrigger = make_shared<triggerinfo>();
		size_t pos = line.find("<hkparam name=\"localTime\">") + 26;
		curTrigger->localtime = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"id\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"id\">") + 19;
		curTrigger->id = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"payload\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"payload\">") + 24;
		curTrigger->payload = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"relativeToEndOfClip\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"relativeToEndOfClip\">") + 36;
		curTrigger->relativetoend = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"acyclic\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"acyclic\">") + 24;
		curTrigger->acyclic = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"isAnnotation\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"isAnnotation\">") + 29;
		curTrigger->isannotation = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
}

bool matchScoring(vector<shared_ptr<triggerinfo>>& ori, vector<shared_ptr<triggerinfo>>& edit, string id)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: hkbClipTriggerArray empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 1;
	map<int, map<int, double>> scorelist;
	map<int, bool> taken;
	vector<shared_ptr<triggerinfo>> newOri;
	vector<shared_ptr<triggerinfo>> newEdit;
	map<int, int> oriRank;
	map<int, int> newRank;
	multimap<double, int> proxyRank;

	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		proxyRank.insert(make_pair(stod(ori[i]->localtime), i));
	}
	
	for (auto& rank : proxyRank)
	{
		oriRank[rank.second] = counter;
		++counter;
	}

	proxyRank.clear();
	counter = 1;

	for (unsigned int i = 0; i < edit.size(); ++i)
	{
		proxyRank.insert(make_pair(stod(edit[i]->localtime), i));
	}

	for (auto& rank : proxyRank)
	{
		newRank[rank.second] = counter;
		++counter;
	}

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->relativetoend == edit[j]->relativetoend)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i]->id == edit[j]->id)
			{
				scorelist[i][j] += 4;
			}

			if (ori[i]->payload == edit[j]->payload)
			{
				scorelist[i][j] += 3;
			}

			if (ori[i]->localtime == edit[j]->localtime)
			{
				scorelist[i][j] += 2.002;
			}
			else
			{
				double num1 = oriRank[i];
				double num2 = newRank[j];
				double num = 1 - (max(num1, num2) - min(num1, num2)) / max(num1, num2);
				scorelist[i][j] += num * 2;
			}

			if (i == j)
			{
				scorelist[i][j] += 0.001;
			}

			if (ori[i]->acyclic == edit[j]->acyclic)
			{
				++scorelist[i][j];
			}

			if (ori[i]->isannotation == edit[j]->isannotation)
			{
				++scorelist[i][j];
			}
		}
	}
	
	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<triggerinfo>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<triggerinfo>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

void inputTrigger(vector<string>& input, shared_ptr<triggerinfo> trigger)
{
	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"localTime\">" + trigger->localtime + "</hkparam>");
	input.push_back("					<hkparam name=\"event\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"id\">" + trigger->id + "</hkparam>");
	input.push_back("							<hkparam name=\"payload\">" + trigger->payload + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"relativeToEndOfClip\">" + from_bool(trigger->relativetoend) + "</hkparam>");
	input.push_back("					<hkparam name=\"acyclic\">" + from_bool(trigger->acyclic) + "</hkparam>");
	input.push_back("					<hkparam name=\"isAnnotation\">" + from_bool(trigger->isannotation) + "</hkparam>");
	input.push_back("				</hkobject>");
}

namespace keepsake
{
	void hkbClipTriggerArrayExport(string id)
	{
		// stage 1 reading
		vector<string> storeline1;
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
			cout << "ERROR: Edit hkbClipTriggerArray Input Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		vector<string> storeline2;
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
			cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (ID: " << id << ")" << endl;
			Error = true;
			return;
		}

		// stage 2 identify edits
		vector<string> output;
		bool newtransition = false;
		bool IsChanged = false;
		bool open = false;
		bool IsEdited = false;
		int curline = 2;
		int openpoint;
		int closepoint;

		output.push_back(storeline2[0]);

		if ((storeline1[1].find(storeline2[1], 0) == string::npos) || (storeline1[1].length() != storeline2[1].length()))
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			openpoint = curline - 1;
			IsChanged = true;
			IsEdited = true;
			open = true;
		}

		output.push_back(storeline2[1]);

		for (unsigned int i = 2; i < storeline2.size(); i++)
		{
			if (!newtransition) // existing data
			{
				if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
				{
					if (open)
					{
						if (IsChanged)
						{
							closepoint = curline;

							if (closepoint != openpoint)
							{
								if ((storeline2[i].find("<hkparam name=\"localTime\">", 0) != string::npos) && (output[output.size() - 2].find("OPEN", 0) == string::npos))
								{
									output.push_back("				<hkobject>");
								}

								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);
								}
							}
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}
				else
				{
					if (!open)
					{
						if (storeline2[i].find("<hkparam name=\"localTime\">", 0) != string::npos)
						{
							output.push_back("				<hkobject>");
						}

						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}
				}

				output.push_back(storeline2[i]);

				if (curline != storeline1.size() - 1)
				{
					curline++;
				}
				else
				{
					newtransition = true;
				}

				if (i == storeline2.size() - 1) // if close no new element
				{
					if (open)
					{
						if (IsChanged)
						{
							closepoint = curline + 1;

							if (closepoint != openpoint)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);
								}
							}
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}
			}
			else // new added data
			{
				if (i != storeline2.size() - 1)
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back("				<hkobject>");
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);

					if (open)
					{
						if (IsChanged)
						{
							closepoint = curline + 1;

							if (closepoint != openpoint)
							{
								if (storeline2[i].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
								{
									output.push_back("				</hkobject>");
								}

								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}
								}
							}
						}
						else
						{
							if (storeline2[i].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
							{
								output.push_back("				</hkobject>");
							}
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
				}
			}
		}

		NemesisReaderFormat(storeline2);

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
					if (output[i].find("<hkparam name=\"localTime\">", 0) != string::npos)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							if (output[i - 1].find("ORIGINAL", 0) == string::npos)
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							if (output[i - 1].find("OPEN", 0) == string::npos)
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else
						{
							if ((output[i - 1].find("ORIGINAL", 0) == string::npos) && (output[i - 1].find("OPEN", 0) == string::npos) && (output[i - 1].find("<hkobject>", 0) == string::npos))
							{
								fwrite << "				<hkobject>" << "\n";
							}

							fwrite << output[i] << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"event\">", 0) != string::npos)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							fwrite << "						<hkobject>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "						<hkobject>" << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"payload\">", 0) != string::npos)
					{
						if (output[i + 1].find("CLOSE", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- CLOSE -->" << "\n";
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
							i++;
						}
						else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
						{
							fwrite << output[i] << "\n";
							fwrite << "<!-- ORIGINAL -->" << "\n";
							i++;
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
						}
					}
					else if (output[i].find("<hkparam name=\"isAnnotation\">", 0) != string::npos)
					{
						if (i != output.size() - 1)
						{
							if (output[i + 1].find("CLOSE", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
								fwrite << "<!-- CLOSE -->" << "\n";
								fwrite << "				</hkobject>" << "\n";
								i++;
							}
							else if (output[i + 1].find("ORIGINAL", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
								fwrite << "<!-- ORIGINAL -->" << "\n";
								i++;
							}
							else if (output[i + 1].find("</hkobject>", 0) != string::npos)
							{
								fwrite << output[i] << "\n";
							}
							else if (output[i + 1].find("<hkobject>", 0) != string::npos)
							{
								if (output[i + 2].find("ORIGINAL", 0) != string::npos)
								{
									fwrite << output[i] << "\n";
									fwrite << "<!-- ORIGINAL -->" << "\n";
									i += 2;
								}
								else
								{
									fwrite << output[i] << "\n";
									fwrite << "				</hkobject>" << "\n";
								}
							}
							else
							{
								fwrite << output[i] << "\n";
								fwrite << "				</hkobject>" << "\n";
							}
						}
						else
						{
							fwrite << output[i] << "\n";
							fwrite << "				</hkobject>" << "\n";
						}
					}
					else
					{
						fwrite << output[i] << "\n";
					}
				}

				fwrite << "			</hkparam>" << "\n";
				fwrite << "		</hkobject>" << "\n";
				outputfile.close();
			}
			else
			{
				cout << "ERROR: Edit hkbClipTriggerArray Output Not Found (File: " << filename << ")" << endl;
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
};
