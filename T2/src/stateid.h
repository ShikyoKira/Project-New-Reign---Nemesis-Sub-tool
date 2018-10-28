#ifndef STATEID_H_
#define STATEID_H_

#include <unordered_map>
#include <string>
#include <memory>

class STATE;
struct hkbstatemachine;

extern std::unordered_map<std::string, std::shared_ptr<hkbstatemachine>> StateMachineID;	// parent ID, state machine ID
extern std::unordered_map<std::string, bool> isSMIDExist;									// parent ID, state machine ID; exist or not

extern std::unordered_map<std::string, STATE> StateID;				// parent ID, state ID; containing base, and current state ID count
extern std::unordered_map<std::string, bool> isStateIDExist;		// parent ID, state ID; exist or not

extern bool stateChange;

class STATE
{
private:
	unsigned int base = 0;
	unsigned int current = 0;
	std::unordered_map<unsigned int, bool> baselist;
	std::unordered_map<std::string, std::string> currentID;

public:
	void InstallBase(std::string id, unsigned int& stateID);
	bool IsNewState(unsigned int stateid);
	std::string GetBaseStr();
	std::string GetStateID(std::string stateID);
};

#endif