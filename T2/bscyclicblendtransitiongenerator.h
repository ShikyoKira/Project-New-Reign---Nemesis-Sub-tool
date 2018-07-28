#ifndef BSCYCLICBLENDERTRANSITIONGENERATOR_H_
#define BSCYCLICBLENDERTRANSITIONGENERATOR_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <Windows.h>

struct bscyclicblendtransitiongenerator
{
public:
	bscyclicblendtransitiongenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	int GetPayloadCount();
	std::string GetPayload(int child);
	bool IsPayloadNull(int child);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string generator;
	std::vector<std::string> payload;
	std::string variablebindingset;
	bool IsNegated = false;
};

extern void BSCyclicBlendTransitionGeneratorExport(std::string id);

#endif