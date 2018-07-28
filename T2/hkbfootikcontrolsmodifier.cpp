#include "hkbfootikcontrolsmodifier.h"
#include "Global.h"
#include "boolstring.h"
#include "highestscore.h"

using namespace std;

struct leg
{
	bool proxy = true;

	string groundPosition;
	int id;
	string payload;
	double verticalError;
	bool hitSomething;
	bool isPlantedMS;
};

struct footik
{
	string variableBindingSet;
	int userData;
	string name;
	bool enable;

	// controlData, gains
	double onOffGain;
	double groundAscendingGain;
	double groundDescendingGain;
	double footPlantedGain;
	double footRaisedGain;
	double footUnlockGain;
	double worldFromModelFeedbackGain;
	double errorUpDownBias;
	double alignWorldFromModelGain;
	double hipOrientationGain;
	double maxKneeAngleDifference;
	double ankleOrientationGain;

	vector<leg> legs;

	string errorOutTranslation;
	string alignWithGroundRotation;
};

void inputFootIK(vector<string>& input, leg& leg);
void inputDoubleFootIK(vector<string>& output, vector<string>& storeline, double oriD, double newD, string key, bool& IsEdited, bool& open, bool first = false);
void footIKInfoProcess(string line, footik& footIK);
bool matchLegsScoring(vector<leg>& ori, vector<leg>& edit, string id);

hkbfootikcontrolsmodifier::hkbfootikcontrolsmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "af" + to_string(functionlayer) + ">";

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

void hkbfootikcontrolsmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
		cout << "ERROR: hkbFootIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbFootIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikcontrolsmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbFootIkControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbFootIkControlsModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
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
			addressChange.erase(addressChange.find(address));
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbFootIkControlsModifier(newID: " << id << ") with hkbFootIkControlsModifier(oldID: " << tempid << ")" << endl;
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
			cout << "Comparing hkbFootIkControlsModifier(newID: " << id << ") with hkbFootIkControlsModifier(oldID: " << tempid << ") is complete!" << endl;
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
		cout << "hkbFootIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbfootikcontrolsmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbFootIkControlsModifier(ID: " << id << ") has been initialized!" << endl;
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
	}
	else
	{
		cout << "ERROR: Dummy hkbFootIkControlsModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbFootIkControlsModifier(ID: " << id << ") is complete!" << endl;
	}
}

int hkbfootikcontrolsmodifier::GetPayloadCount()
{
	return int(payload.size());
}

string hkbfootikcontrolsmodifier::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbfootikcontrolsmodifier::IsPayloadNull(int child)
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

string hkbfootikcontrolsmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbfootikcontrolsmodifier::IsBindingNull()
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

string hkbfootikcontrolsmodifier::GetAddress()
{
	return address;
}

bool hkbfootikcontrolsmodifier::IsNegate()
{
	return IsNegated;
}

void hkbFootIkControlsModifierExport(string id)
{
	//stage 1 reading
	string line;
	footik oriFootIK;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			footIKInfoProcess(FunctionLineTemp[id][i], oriFootIK);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkControlsModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	footik newFootIK;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			footIKInfoProcess(FunctionLineNew[id][i], newFootIK);
		}
	}
	else
	{
		cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 identifying edits
	vector<string> output;
	vector<string> storeline;
	bool IsEdited = false;
	bool open = false;

	output.push_back(FunctionLineTemp[id][0]);

	if (oriFootIK.variableBindingSet != newFootIK.variableBindingSet)
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		output.push_back("			<hkparam name=\"variableBindingSet\">" + newFootIK.variableBindingSet + "</hkparam>");
		storeline.push_back("			<hkparam name=\"variableBindingSet\">" + oriFootIK.variableBindingSet + "</hkparam>");
		IsEdited = true;
		open = true;
	}
	else
	{
		output.push_back("			<hkparam name=\"variableBindingSet\">" + oriFootIK.variableBindingSet + "</hkparam>");
	}

	if (oriFootIK.variableBindingSet != newFootIK.variableBindingSet)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			open = true;
		}

		output.push_back("			<hkparam name=\"userData\">" + to_string(newFootIK.userData) + "</hkparam>");
		storeline.push_back("			<hkparam name=\"userData\">" + to_string(oriFootIK.userData) + "</hkparam>");
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

		output.push_back("			<hkparam name=\"userData\">" + to_string(oriFootIK.userData) + "</hkparam>");
	}

	if (oriFootIK.name != newFootIK.name)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			open = true;
		}

		output.push_back("			<hkparam name=\"name\">" + newFootIK.name + "</hkparam>");
		storeline.push_back("			<hkparam name=\"name\">" + oriFootIK.name + "</hkparam>");
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

		output.push_back("			<hkparam name=\"name\">" + oriFootIK.name + "</hkparam>");
	}

	if (oriFootIK.enable != newFootIK.enable)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
		}

		output.push_back("			<hkparam name=\"enable\">" + from_bool(newFootIK.enable) + "</hkparam>");
		storeline.push_back("			<hkparam name=\"enable\">" + from_bool(oriFootIK.enable) + "</hkparam>");
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
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

		output.push_back("			<hkparam name=\"enable\">" + from_bool(oriFootIK.enable) + "</hkparam>");
	}

	output.push_back("			<hkparam name=\"controlData\">");
	output.push_back("				<hkobject>");
	output.push_back("					<hkparam name=\"gains\">");
	output.push_back("						<hkobject>");

	inputDoubleFootIK(output, storeline, oriFootIK.onOffGain, newFootIK.onOffGain, "onOffGain", IsEdited, open, true);
	inputDoubleFootIK(output, storeline, oriFootIK.groundAscendingGain, newFootIK.groundAscendingGain, "groundAscendingGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.groundDescendingGain, newFootIK.groundDescendingGain, "groundDescendingGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.footPlantedGain, newFootIK.footPlantedGain, "footPlantedGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.footRaisedGain, newFootIK.footRaisedGain, "footRaisedGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.footUnlockGain, newFootIK.footUnlockGain, "footUnlockGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.worldFromModelFeedbackGain, newFootIK.worldFromModelFeedbackGain, "worldFromModelFeedbackGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.errorUpDownBias, newFootIK.errorUpDownBias, "errorUpDownBias", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.alignWorldFromModelGain, newFootIK.alignWorldFromModelGain, "alignWorldFromModelGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.hipOrientationGain, newFootIK.hipOrientationGain, "hipOrientationGain", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.maxKneeAngleDifference, newFootIK.maxKneeAngleDifference, "maxKneeAngleDifference", IsEdited, open);
	inputDoubleFootIK(output, storeline, oriFootIK.ankleOrientationGain, newFootIK.ankleOrientationGain, "ankleOrientationGain", IsEdited, open);

	if (open)
	{
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}

	output.push_back("						</hkobject>");
	output.push_back("					</hkparam>");
	output.push_back("				</hkobject>");
	output.push_back("			</hkparam>");

	bool newZero = false;
	bool oriZero = false;
	bool notSameCount = false;

	if (oriFootIK.legs.size() != newFootIK.legs.size())
	{
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

		if (newFootIK.legs.size() != 0)
		{
			output.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(newFootIK.legs.size()) + "\">");
		}
		else
		{
			output.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
			newZero = true;
		}

		if (oriFootIK.legs.size() != 0)
		{
			if (newFootIK.legs.size() != 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(oriFootIK.legs.size()) + "\">");
				output.push_back("<!-- CLOSE -->");
			}
		}
		else
		{
			oriZero = true;
		}

		IsEdited = true;
		notSameCount = true;
	}
	else if (oriFootIK.legs.size() > 0)
	{
		output.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(oriFootIK.legs.size()) + "\">");
	}
	else
	{
		output.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
	}

	if (!matchLegsScoring(oriFootIK.legs, newFootIK.legs, id))
	{
		return;
	}

	for (unsigned int i = 0; i < oriFootIK.legs.size(); ++i)
	{
		storeline.clear();
		bool open1 = false;

		if (newFootIK.legs[i].proxy)
		{
			vector<string> instore;

			if (!newZero)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			}
			else
			{
				instore.push_back("			<hkparam name=\"legs\" numelements=\"" + to_string(oriFootIK.legs.size()) + "\">");
			}

			IsEdited = true;
			bool nobreak = true;

			while (i < oriFootIK.legs.size())
			{
				if (!newFootIK.legs[i].proxy)
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

				inputFootIK(instore, oriFootIK.legs[i]);
				++i;
			}

			if (nobreak)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), instore.begin(), instore.end());

				if (!newZero)
				{
					output.push_back("<!-- CLOSE -->");
				}
			}
		}
		else if (!oriFootIK.legs[i].proxy)
		{
			output.push_back("				<hkobject>");

			if (oriFootIK.legs[i].groundPosition != newFootIK.legs[i].groundPosition)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"groundPosition\">" + newFootIK.legs[i].groundPosition + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.push_back("					<hkparam name=\"groundPosition\">" + oriFootIK.legs[i].groundPosition + "</hkparam>");
				output.push_back("<!-- CLOSE -->");
				IsEdited = true;
			}
			else
			{
				output.push_back("					<hkparam name=\"groundPosition\">" + oriFootIK.legs[i].groundPosition + "</hkparam>");
			}

			output.push_back("					<hkparam name=\"ungroundedEvent\">");
			output.push_back("						<hkobject>");

			if (oriFootIK.legs[i].id != newFootIK.legs[i].id)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("							<hkparam name=\"id\">" + to_string(newFootIK.legs[i].id) + "</hkparam>");
				open1 = true;
				IsEdited = true;
			}
			else
			{
				output.push_back("							<hkparam name=\"id\">" + to_string(oriFootIK.legs[i].id) + "</hkparam>");
			}

			if (oriFootIK.legs[i].payload != newFootIK.legs[i].payload)
			{
				if (!open1)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					output.push_back("							<hkparam name=\"payload\">" + newFootIK.legs[i].payload + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"payload\">" + oriFootIK.legs[i].payload + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					IsEdited = true;
				}
				else
				{
					output.push_back("							<hkparam name=\"payload\">" + newFootIK.legs[i].payload + "</hkparam>");
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + to_string(oriFootIK.legs[i].id) + "</hkparam>");
					output.push_back("							<hkparam name=\"payload\">" + oriFootIK.legs[i].payload + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open1 = false;
				}
			}
			else
			{
				if (open1)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.push_back("							<hkparam name=\"id\">" + to_string(oriFootIK.legs[i].id) + "</hkparam>");
					output.push_back("<!-- CLOSE -->");
					open1 = false;
				}

				output.push_back("							<hkparam name=\"payload\">" + oriFootIK.legs[i].payload + "</hkparam>");
			}

			output.push_back("						</hkobject>");
			output.push_back("					</hkparam>");

			if (oriFootIK.legs[i].verticalError != newFootIK.legs[i].verticalError)
			{
				stringstream ssNewD;
				ssNewD << setprecision(6) << fixed << newFootIK.legs[i].verticalError;

				stringstream ssOriD;
				ssOriD << setprecision(6) << fixed << oriFootIK.legs[i].verticalError;

				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				output.push_back("					<hkparam name=\"verticalError\">" + ssNewD.str() + "</hkparam>");
				storeline.push_back("					<hkparam name=\"verticalError\">" + ssOriD.str() + "</hkparam>");
				IsEdited = true;
				open1 = true;
			}
			else
			{
				stringstream ssD;
				ssD << setprecision(6) << fixed << newFootIK.legs[i].verticalError;
				output.push_back("					<hkparam name=\"verticalError\">" + ssD.str() + "</hkparam>");
			}

			if (oriFootIK.legs[i].hitSomething != newFootIK.legs[i].hitSomething)
			{
				if (!open1)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					open1 = true;
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"hitSomething\">" + from_bool(newFootIK.legs[i].hitSomething) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"hitSomething\">" + from_bool(oriFootIK.legs[i].hitSomething) + "</hkparam>");
			}
			else
			{
				if (open1)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					storeline.clear();
					open1 = false;
				}

				output.push_back("					<hkparam name=\"hitSomething\">" + from_bool(oriFootIK.legs[i].hitSomething) + "</hkparam>");
			}

			if (oriFootIK.legs[i].isPlantedMS != newFootIK.legs[i].isPlantedMS)
			{
				if (!open1)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
				}

				output.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(newFootIK.legs[i].isPlantedMS) + "</hkparam>");
				storeline.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(oriFootIK.legs[i].isPlantedMS) + "</hkparam>");
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
				output.push_back("<!-- CLOSE -->");
				storeline.clear();
			}
			else
			{
				if (open1)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					output.push_back("<!-- CLOSE -->");
					storeline.clear();
				}

				output.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(oriFootIK.legs[i].isPlantedMS) + "</hkparam>");
			}

			output.push_back("				</hkobject>");
		}
		else
		{
			if (!oriZero)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
			}

			while (i < oriFootIK.legs.size())
			{
				inputFootIK(output, newFootIK.legs[i]);
				++i;
			}

			if (!oriZero)
			{
				output.push_back("<!-- CLOSE -->");
			}
		}
	}

	if (notSameCount)
	{
		if (newZero)
		{
			output.push_back("			</hkparam>");
			output.push_back("<!-- CLOSE -->");
		}
		else
		{
			output.push_back("			</hkparam>");
		}
	}
	else if (!oriZero)
	{
		output.push_back("			</hkparam>");
	}

	if (oriFootIK.errorOutTranslation != newFootIK.errorOutTranslation)
	{
		if (notSameCount && oriZero)
		{
			storeline.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
		}
		else
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		}

		output.push_back("			<hkparam name=\"errorOutTranslation\">" + newFootIK.errorOutTranslation + "</hkparam>");
		storeline.push_back("			<hkparam name=\"errorOutTranslation\">" + oriFootIK.errorOutTranslation + "</hkparam>");
		IsEdited = true;
		open = true;
	}
	else
	{
		if (notSameCount && oriZero)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.push_back("			<hkparam name=\"legs\" numelements=\"0\"></hkparam>");
			output.push_back("<!-- CLOSE -->");
		}

		output.push_back("			<hkparam name=\"errorOutTranslation\">" + oriFootIK.errorOutTranslation + "</hkparam>");
	}

	if (oriFootIK.alignWithGroundRotation != newFootIK.alignWithGroundRotation)
	{
		if (!open)
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
		}

		output.push_back("			<hkparam name=\"alignWithGroundRotation\">" + newFootIK.alignWithGroundRotation + "</hkparam>");
		storeline.push_back("			<hkparam name=\"alignWithGroundRotation\">" + oriFootIK.alignWithGroundRotation + "</hkparam>");
		output.push_back("<!-- ORIGINAL -->");
		output.insert(output.end(), storeline.begin(), storeline.end());
		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
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

		output.push_back("			<hkparam name=\"alignWithGroundRotation\">" + oriFootIK.alignWithGroundRotation + "</hkparam>");
	}
	
	output.push_back("		</hkobject>");
	NemesisReaderFormat(output, true);

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
			cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (File: " << filename << ")" << endl;
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

void footIKInfoProcess(string line, footik& footIK)
{
	if (line.find("<hkparam name=\"variableBindingSet\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"variableBindingSet\">") + 35;
		footIK.variableBindingSet = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"userData\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"userData\">") + 25;
		footIK.userData = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"name\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"name\">") + 21;
		footIK.name = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"enable\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"enable\">") + 23;
		footIK.enable = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"onOffGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"onOffGain\">") + 26;
		footIK.onOffGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"groundAscendingGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"groundAscendingGain\">") + 36;
		footIK.groundAscendingGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"groundDescendingGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"groundDescendingGain\">") + 37;
		footIK.groundDescendingGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"footPlantedGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"footPlantedGain\">") + 32;
		footIK.footPlantedGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"footRaisedGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"footRaisedGain\">") + 31;
		footIK.footRaisedGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"footUnlockGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"footUnlockGain\">") + 31;
		footIK.footUnlockGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"worldFromModelFeedbackGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"worldFromModelFeedbackGain\">") + 43;
		footIK.worldFromModelFeedbackGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"errorUpDownBias\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"errorUpDownBias\">") + 32;
		footIK.errorUpDownBias = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"alignWorldFromModelGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"alignWorldFromModelGain\">") + 40;
		footIK.alignWorldFromModelGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"hipOrientationGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"hipOrientationGain\">") + 35;
		footIK.hipOrientationGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"maxKneeAngleDifference\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"maxKneeAngleDifference\">") + 39;
		footIK.maxKneeAngleDifference = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"ankleOrientationGain\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"ankleOrientationGain\">") + 37;
		footIK.ankleOrientationGain = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"groundPosition\">") != string::npos)
	{
		leg dummy;
		dummy.proxy = false;
		size_t pos = line.find("<hkparam name=\"groundPosition\">") + 31;
		dummy.groundPosition = line.substr(pos, line.find("</hkparam>", pos) - pos);
		footIK.legs.push_back(dummy);
	}
	else if (line.find("<hkparam name=\"id\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"id\">") + 19;
		footIK.legs.back().id = stoi(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"payload\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"payload\">") + 24;
		footIK.legs.back().payload = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"verticalError\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"verticalError\">") + 30;
		footIK.legs.back().verticalError = stod(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"hitSomething\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"hitSomething\">") + 29;
		footIK.legs.back().hitSomething = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"isPlantedMS\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"isPlantedMS\">") + 28;
		footIK.legs.back().isPlantedMS = to_bool(line.substr(pos, line.find("</hkparam>", pos) - pos));
	}
	else if (line.find("<hkparam name=\"errorOutTranslation\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"errorOutTranslation\">") + 36;
		footIK.errorOutTranslation = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
	else if (line.find("<hkparam name=\"alignWithGroundRotation\">") != string::npos)
	{
		size_t pos = line.find("<hkparam name=\"alignWithGroundRotation\">") + 40;
		footIK.alignWithGroundRotation = line.substr(pos, line.find("</hkparam>", pos) - pos);
	}
}

bool matchLegsScoring(vector<leg>& ori, vector<leg>& edit, string id)
{
	int counter = 0;
	map<int, map<int, int>> scorelist;
	map<int, bool> taken;
	vector<leg> newOri;
	vector<leg> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].groundPosition == edit[j].groundPosition)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].id == edit[j].id)
			{
				scorelist[i][j] += 10;
			}

			if (ori[i].payload == edit[j].payload)
			{
				scorelist[i][j] += 5;
			}

			if (ori[i].verticalError == edit[j].verticalError)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].hitSomething == edit[j].hitSomething)
			{
				scorelist[i][j] += 2;
			}

			if (ori[i].isPlantedMS == edit[j].isPlantedMS)
			{
				scorelist[i][j] += 2;
			}

			if (i == j)
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
			leg dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			leg dummy;
			newEdit.push_back(dummy);
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

void inputDoubleFootIK(vector<string>& output, vector<string>& storeline, double oriD, double newD, string key, bool& IsEdited, bool& open, bool first)
{
	if (first)
	{
		if (oriD != newD)
		{
			stringstream ssNewD;
			ssNewD << setprecision(6) << fixed << newD;

			stringstream ssOriD;
			ssOriD << setprecision(6) << fixed << oriD;

			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			output.push_back("							<hkparam name=\"" + key + "\">" + ssNewD.str() + "</hkparam>");
			storeline.push_back("							<hkparam name=\"" + key + "\">" + ssOriD.str() + "</hkparam>");
			IsEdited = true;
			open = true;
		}
		else
		{
			stringstream ssD;
			ssD << setprecision(6) << fixed << oriD;
			output.push_back("							<hkparam name=\"" + key + "\">" + ssD.str() + "</hkparam>");
		}
	}
	else
	{
		if (oriD != newD)
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			stringstream ssNewD;
			ssNewD << setprecision(6) << fixed << newD;

			stringstream ssOriD;
			ssOriD << setprecision(6) << fixed << oriD;

			output.push_back("							<hkparam name=\"" + key + "\">" + ssNewD.str() + "</hkparam>");
			storeline.push_back("							<hkparam name=\"" + key + "\">" + ssOriD.str() + "</hkparam>");
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

			stringstream ssD;
			ssD << setprecision(6) << fixed << oriD;
			output.push_back("							<hkparam name=\"" + key + "\">" + ssD.str() + "</hkparam>");
		}
	}
}

void inputFootIK(vector<string>& input, leg& leg)
{
	stringstream verticalError;
	verticalError << setprecision(6) << fixed << leg.verticalError;

	input.push_back("				<hkobject>");
	input.push_back("					<hkparam name=\"groundPosition\">" + leg.groundPosition + "</hkparam>");
	input.push_back("					<hkparam name=\"ungroundedEvent\">");
	input.push_back("						<hkobject>");
	input.push_back("							<hkparam name=\"id\">" + to_string(leg.id) + "</hkparam>");
	input.push_back("							<hkparam name=\"payload\">" + leg.payload + "</hkparam>");
	input.push_back("						</hkobject>");
	input.push_back("					</hkparam>");
	input.push_back("					<hkparam name=\"verticalError\">" + verticalError.str() + "</hkparam>");
	input.push_back("					<hkparam name=\"hitSomething\">" + from_bool(leg.hitSomething) + "</hkparam>");
	input.push_back("					<hkparam name=\"isPlantedMS\">" + from_bool(leg.isPlantedMS) + "</hkparam>");
	input.push_back("				</hkobject>");
}

namespace keepsake
{
	void hkbFootIkControlsModifierExport(string id)
	{
		//stage 1 reading
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
			cout << "ERROR: Edit hkbFootIkControlsModifier Input Not Found (ID: " << id << ")" << endl;
			Error = true;
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
			cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (ID: " << id << ")" << endl;
			Error = true;
		}

		//stage 2 identifying edits
		vector<string> output;
		bool IsEdited = false;
		bool IsChanged = false;
		bool open = false;
		int curline = 0;
		int part = 0;
		int openpoint;
		int closepoint;

		for (unsigned int i = 0; i < storeline2.size(); i++)
		{
			if ((storeline1[curline].find("<hkparam name=\"legs\" numelements=", 0) == string::npos) && (part == 0)) // pre legs info
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;

				if (storeline1[curline].find("<hkparam name=\"legs\" numelements=", 0) != string::npos)
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}
				}
			}
			else if ((storeline1[curline].find("<hkparam name=\"errorOutTranslation\">", 0) == string::npos) && (part == 1))
			{
				if (((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
					}

					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						output.push_back(storeline2[i]);

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}
				}
				else if (((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length())) && ((storeline1[curline].find("</hkparam>", 0) == string::npos) || storeline1[curline].length() > 15))
				{
					if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
					{
						output.push_back("				<hkobject>");
					}

					if (open)
					{
						closepoint = curline;
						output.push_back("<!-- ORIGINAL -->");

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					output.push_back(storeline2[i]);
				}

				curline++;

				if ((open) && (storeline2[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos))
				{
					closepoint = curline;
					output.push_back("<!-- ORIGINAL -->");

					for (int j = openpoint; j < closepoint; j++)
					{
						output.push_back(storeline1[j]);
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				if (storeline2[i].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
				{
					output.push_back("				</hkobject>");
				}
			}
			else if (part == 2) // existing leftover settings
			{
				if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsChanged = true;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
				}
				else
				{
					if (open)
					{
						output.push_back("<!-- ORIGINAL -->");
						closepoint = curline;

						for (int j = openpoint; j < closepoint; j++)
						{
							output.push_back(storeline1[j]);
						}

						output.push_back("<!-- CLOSE -->");
						IsChanged = false;
						open = false;
					}

					output.push_back(storeline2[i]);
				}

				curline++;
			}
			else // added variable value
			{
				if (storeline2[i].find("<hkparam name=\"legs\" numelements=", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");

						if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
						{
							output.push_back("				<hkobject>");
							output.push_back(storeline2[i]);
						}
						else if (storeline2[i].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
						{
							output.push_back(storeline2[i]);
							output.push_back("				</hkobject>");
						}
						else
						{
							output.push_back(storeline2[i]);
						}

						if ((storeline1[curline].find("</hkparam>", 0) != string::npos) && (storeline2[i + 1].find("errorOutTranslation", 0) == string::npos))
						{
							IsChanged = true;
							openpoint = curline;
							open = true;
						}
						else
						{
							output.push_back("<!-- ORIGINAL -->");
							output.push_back(storeline1[curline]);
							output.push_back("<!-- CLOSE -->");
							IsChanged = false;
							open = false;
						}

						IsEdited = true;
					}
					else
					{
						output.push_back(storeline2[i]);
					}

					part = 1;
					curline++;
				}
				else if (storeline2[i].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos)
				{
					if ((storeline1[curline].find(storeline2[i], 0) == string::npos) || (storeline1[curline].length() != storeline2[i].length()))
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						openpoint = curline;
						IsEdited = true;
						open = true;
					}

					output.push_back(storeline2[i]);
					part = 2;
					curline++;
				}
				else
				{
					if (part == 1)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						if (storeline2[i].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos)
						{
							output.push_back("				<hkobject>");
							output.push_back(storeline2[i]);
						}
						else if (storeline2[i].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
						{
							output.push_back(storeline2[i]);
							output.push_back("				</hkobject>");
						}
						else
						{
							output.push_back(storeline2[i]);
						}

						if (storeline2[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos)
						{
							if (IsChanged)
							{
								output.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < curline; j++)
								{
									if ((storeline1[j].find("<hkparam name=\"fwdAxisLS\">", 0) != string::npos) && (output.back().find("ORIGINAL", 0) == string::npos))
									{
										output.push_back("				<hkobject>");
									}

									output.push_back(storeline1[j]);

									if (storeline1[j].find("<hkparam name=\"isPlantedMS\">", 0) != string::npos)
									{
										output.push_back("				</hkobject>");
									}

								}

								IsChanged = false;
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
				}
			}
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

		NemesisReaderFormat(output, true);

		// stage 3 output if it is edited
		string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";
		bool closeOri = false;
		bool closeEdit = false;

		if (IsEdited)
		{
			ofstream outputfile(filename);

			if (outputfile.is_open())
			{
				FunctionWriter fwrite(&outputfile);
				part = 0;

				for (unsigned int i = 0; i < output.size(); i++)
				{
					if (i < output.size() - 1)
					{
						if ((output[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos) && (output[i].find("OPEN", 0) != string::npos)) // close outside if both aren't closed
						{
							if ((!closeOri) && (!closeEdit))
							{
								fwrite << "			</hkparam>" << "\n";
								closeOri = true;
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"legs\" numelements=", 0) != string::npos) && (output[i + 1].find("</hkparam>", 0) != string::npos) && (output[i].find("ORIGINAL", 0) != string::npos)) // close edit if original has closed
						{
							if (!closeEdit)
							{
								fwrite << "			</hkparam>" << "\n";
								closeEdit = true;
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"legs\" numelements=", 0) != string::npos) && (output[i].find("<hkparam name=\"ankleOrientationGain\">", 0) == string::npos) && (output[i].find("ORIGINAL", 0) == string::npos) && (output[i].find("CLOSE", 0) == string::npos))
						{
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
							fwrite << "				</hkobject>" << "\n";
							fwrite << "			</hkparam>" << "\n";
						}
					}

					if (part == 0)
					{
						if (output[i].find("<hkparam name=\"legs\" numelements=", 0) != string::npos)
						{
							if (output[i - 1].find("OPEN", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
								}
								else
								{
									closeEdit = false;
								}
							}
							else if (output[i - 1].find("ORIGINAL", 0) != string::npos)
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeOri = true;
								}
								else
								{
									closeOri = false;
								}
							}
							else
							{
								if (output[i].find("</hkparam>", 0) != string::npos)
								{
									closeEdit = true;
									closeOri = true;
								}
								else
								{
									closeEdit = false;
									closeOri = false;
								}
							}
						}

						fwrite << output[i] << "\n";

						if (output[i + 1].find("<hkparam name=\"errorOutTranslation\">", 0) != string::npos)
						{
							part = 1;
						}
					}
					else
					{
						fwrite << output[i] << "\n";
					}

					if (output[i].find("<hkparam name=\"controlData\">", 0) != string::npos)
					{
						fwrite << "				<hkobject>" << "\n";
					}
					else if (output[i].find("<hkparam name=\"gains\">", 0) != string::npos)
					{
						fwrite << "						<hkobject>" << "\n";
					}

					if (i < output.size() - 1)
					{
						if (output[i + 1].find("errorOutTranslation", 0) != string::npos) // close outside if both aren't closed
						{
							if ((output[i].find("CLOSE", 0) != string::npos) || (output[i].find("</hkobject>", 0) != string::npos))
							{
								if ((!closeOri) && (!closeEdit))
								{
									fwrite << "			</hkparam>" << "\n";
									closeOri = true;
									closeEdit = true;
								}
							}
						}
						else if ((output[i + 1].find("<hkparam name=\"legs\" numelements=", 0) != string::npos) && ((output[i].find("<hkparam name=\"ankleOrientationGain\">", 0) != string::npos) || (output[i].find("CLOSE", 0) != string::npos)))
						{
							fwrite << "						</hkobject>" << "\n";
							fwrite << "					</hkparam>" << "\n";
							fwrite << "				</hkobject>" << "\n";
							fwrite << "			</hkparam>" << "\n";
						}
					}
				}

				fwrite << "		</hkobject>" << "\n";
				outputfile.close();
			}
			else
			{
				cout << "ERROR: Edit hkbFootIkControlsModifier Output Not Found (File: " << filename << ")" << endl;
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
}