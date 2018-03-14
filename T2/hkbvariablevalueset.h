#ifndef HKBVARIABLEVALUESET_H_
#define HKBVARIABLEVALUESET_H_

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

struct hkbvariablevalueset
{
public:
	hkbvariablevalueset(std::string filepath, std::string id, std::string preaddress, int functionLayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	bool IsNegated = false;
};

extern void hkbVariableValueSetExport(std::string originalfile, std::string editedfile, std::string id);

#endif