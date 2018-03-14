#ifndef HKBDELAYEDMODIFIER_H_
#define HKBDELAYEDMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbdelayedmodifier
{
public:
	hkbdelayedmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetModifier();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string modifier;
	bool IsNegated = false;
};

extern void hkbDelayedModifierExport(std::string originalfile, std::string editedfile, std::string id);

#endif