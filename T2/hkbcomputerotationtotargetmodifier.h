#ifndef HKBCOMPUTEROTATIONTOTARGETMODIFIER_H_
#define HKBCOMPUTEROTATIONTOTARGETMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbcomputerotationtotargetmodifier
{
public:
	hkbcomputerotationtotargetmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	bool IsNegated = false;
};

extern void hkbComputeRotationToTargetModifierExport(std::string originalfile, std::string editedfile, std::string id);

#endif