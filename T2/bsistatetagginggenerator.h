#ifndef BSISTATETAGGINGGENERATOR_H_
#define BSISTATETAGGINGGENERAOTR_H_

#include <string>
#include <iostream>
#include <fstream>

struct bsistatetagginggenerator
{
public:
	bsistatetagginggenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetVariableBindingSet();
	bool IsBindingNull();
	
	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string generator;
	bool IsNegated = false;
};

extern void BSiStateTaggingGeneratorExport(std::string id);

#endif