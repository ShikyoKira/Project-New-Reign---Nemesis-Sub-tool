#include "stateid.h"
#include <iostream>

using namespace std;

unordered_map<string, shared_ptr<hkbstatemachine>> StateMachineID;		// node ID, pointer to statemachine
unordered_map<string, bool> isSMIDExist;					// node ID, pointer to statemachine exist or not

unordered_map<string, STATE> StateID;						// parent ID, state ID; containing base, and current state ID count
unordered_map<string, bool> isStateIDExist;					// parent ID, true/false; exist or not

bool stateChange = false;

void STATE::InstallBase(string id, unsigned int& stateID)
{
	if (base <= stateID)
	{
		base = stateID + 1;
	}

	if (baselist[stateID])
	{
		cout << "WARNING: Duplicated state ID detected (ID: " << id << ")" << endl;
	}

	baselist[stateID] = true;
}

bool STATE::IsNewState(unsigned int stateid)
{
	return !baselist[stateid];
}

string STATE::GetBaseStr()
{
	return to_string(base);
}

string STATE::GetStateID(string stateID)
{
	if (currentID[stateID].length() > 0)
	{
		return currentID[stateID];
	}
	else
	{
		currentID[stateID] = to_string(current);
		return to_string(current++);
	}
}
