#ifndef HKBSTRINGCONDITION_H_
#define HKBSTRINGCONDITION_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbstringcondition
{
public:
	hkbstringcondition(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id, int functionlayer);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();
private:
	std::string address;
	std::string tempaddress;
	std::string expression;
	bool IsNegated = false;
};

extern void hkbStringConditionExport(std::string id);
#endif