#ifndef BSEVENTONFALSETOTRUEMODIFIER_H_
#define BSEVENTONFALSETOTRUEMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct bseventonfalsetotruemodifier
{
public:
	bseventonfalsetotruemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

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
	std::vector<std::string> payload;
	std::string variablebindingset;
	int payloadcount;
	bool IsNegated = false;
};

extern void BSEventOnFalseToTrueModifierExport(std::string originalfile, std::string editedfile, std::string id);

#endif