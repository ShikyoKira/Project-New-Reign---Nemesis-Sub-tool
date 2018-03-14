#ifndef HKBBLENDERGENERATOR_H_
#define HKBBLENDERGENERATOR_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

struct hkbblendergenerator
{
public:
	hkbblendergenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string name;
	std::vector<std::string> generator;
	std::string variablebindingset;
	int children;
	bool IsNegated = false;
};

extern void hkbBlenderGeneratorExport(std::string originalfile, std::string editedfile, std::string id);

#endif