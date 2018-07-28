#ifndef HKBTIMERMODIFIER_H_
#define HKBTIMERMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbtimermodifier
{
public:
	hkbtimermodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

extern void hkbTimerModifierExport(std::string id);

#endif