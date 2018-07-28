#ifndef HKBGETUPMODIFIER_H_
#define HKBGETUPMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbgetupmodifier
{
public:
	hkbgetupmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

extern void hkbGetUpModifierExport(std::string id);

#endif