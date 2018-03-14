#ifndef HKBMANUALSELECTORGENERATOR_H_
#define HKBMANUALSELECTORGENERATOR_H_

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

struct hkbmanualselectorgenerator
{
public:
	hkbmanualselectorgenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetClass(std::string id, std::string inputfile);

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

extern void hkbManualSelectorGeneratorExport(std::string originalfile, std::string editedfile, std::string id);

#endif