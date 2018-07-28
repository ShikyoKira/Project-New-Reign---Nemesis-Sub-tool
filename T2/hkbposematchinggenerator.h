#ifndef HKBPOSEMATCHINGGENERATOR_H_
#define HKBPOSEMATCHINGGENERATOR_H_

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

struct hkbposematchinggenerator
{
public:
	hkbposematchinggenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

extern void hkbPoseMatchingGeneratorExport(std::string id);

#endif