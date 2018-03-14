#ifndef BSBONESWITCHGENERATORBONEDATA_H_
#define BSBONESWITCHGENERATORBONEDATA_H_

#include <string>
#include <iostream>
#include <fstream>

struct bsboneswitchgeneratorbonedata
{
public:
	bsboneswitchgeneratorbonedata(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetBoneWeights();
	bool IsBoneWeightsNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

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

extern void BSBoneSwitchGeneratorBoneDataExport(std::string originalfile, std::string editedfile, std::string id);

#endif