#ifndef HKBMODIFIERLIST_H_
#define HKBMODIFIERLIST_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbmodifierlist
{
public:
	hkbmodifierlist(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetModifier(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string name;
	std::vector<std::string> modifier;
	std::string variablebindingset;
	int children;
	bool IsNegated = false;
};

extern void hkbModifierListExport(std::string id);

#endif