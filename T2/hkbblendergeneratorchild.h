#ifndef HKBBLENDERGENERATORCHILD_H_
#define HKBBLENDERGENERATORCHILD_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbblendergeneratorchild
{
public:
	hkbblendergeneratorchild(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetBoneWeights();
	bool IsBoneWeightsNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string generator;
	std::string variablebindingset;
	std::string boneweights;
	bool IsNegated = false;
};

extern void hkbBlenderGeneratorChildExport(std::string id);

#endif