#ifndef HKBCLIPGENERATOR_H_
#define HKBCLIPGENERATOR_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbclipgenerator
{
public:
	hkbclipgenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetTriggers();
	bool IsTriggersNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string triggers;
	bool IsNegated = false;
};

extern void hkbClipGeneratorExport(std::string id);

#endif