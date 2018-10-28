#ifndef HKBSTATEMACHINE_H_
#define HKBSTATEMACHINE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <memory>

struct hkbstatemachine
{
public:
	std::shared_ptr<hkbstatemachine> previousSM;
	bool previousSMExist = false;

	hkbstatemachine(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator(int child);
	int GetChildren();

	std::string GetPayload();
	bool IsPayloadNull();

	std::string GetWildcard();
	bool IsWildcardNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string GetRealID();

private:
	std::string realID;
	std::string address;
	std::string tempaddress;
	std::string name;
	std::string payload;
	std::vector<std::string> generator;
	std::string variablebindingset;
	std::string wildcard;
	int children;
	bool IsNegated = false;
};

extern void hkbStateMachineExport(std::string id);

#endif