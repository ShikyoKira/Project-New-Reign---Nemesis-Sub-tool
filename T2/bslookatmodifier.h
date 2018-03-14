#ifndef BSLOOKATMODIFIER_H_
#define BSLOOKATMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct bslookatmodifier
{
public:
	bslookatmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetPayload();
	bool IsPayloadNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string payload;
	std::string variablebindingset;
	bool IsNegated = false;
};

extern void BSLookAtModifierExport(std::string originalfile, std::string editedfile, std::string id);

#endif