#ifndef BSMODIFYONCEMODIFIER_H_
#define BSMODIFYONCEMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct bsmodifyoncemodifier
{
public:
	bsmodifyoncemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetModifier(int number);
	bool IsModifierNull(int number);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string modifier1;
	std::string modifier2;
	std::string variablebindingset;
	bool IsNegated = false;
};

extern void BSModifyOnceModifierExport(std::string id);

#endif