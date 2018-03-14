#ifndef HKBVARIABLEBINDINGSET_H_
#define HKBVARIABLEBINDINGSET_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbvariablebindingset
{
public:
	hkbvariablebindingset(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	bool IsNegated = false;
};

extern void hkbVariableBindingSetExport(std::string originalfile, std::string editedfile, std::string id);

#endif