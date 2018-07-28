#ifndef BSBONESWITCHGENERATOR_H_
#define BSBONESWITCHGENERATOR_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct bsboneswitchgenerator
{
public:
	bsboneswitchgenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetBoneData(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string name;
	std::vector<std::string> bonedata;
	std::string variablebindingset;
	std::string generator;
	int children;
	bool IsNegated = false;
};

extern void BSBoneSwitchGeneratorExport(std::string id);

#endif