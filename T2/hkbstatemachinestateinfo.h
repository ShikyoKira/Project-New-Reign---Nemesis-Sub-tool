#ifndef HKBSTATEMACHINESTATEINFO_H_
#define HKBSTATEMACHINESTATEINFO_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbstatemachinestateinfo
{
public:
	hkbstatemachinestateinfo(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetVariableBindingSet();
	bool IsBindingNull();
	std::string GetTransitions();
	bool IsTransitionsNull();
	std::string GetEnterNotifyEvent();
	bool IsEnterNotifyEventNull();
	std::string GetExitNotifyEvent();
	bool IsExitNotifyEventNull();

	std::string GetAddress();
	bool IsNegate();
private:
	std::string address;
	std::string tempaddress;
	std::string generator;
	std::string variablebindingset;
	std::string transitions;
	std::string enternotifyevent;
	std::string exitnotifyevent;
	bool IsNegated = false;
};

extern void hkbStateMachineStateInfoExport(std::string originalfile, std::string editedfile, std::string id);

#endif