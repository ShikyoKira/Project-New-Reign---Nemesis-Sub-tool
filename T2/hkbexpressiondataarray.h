#ifndef HKBEXPRESSIONDATAARRAY_H_
#define HKBEXPRESSIONDATAARRAY_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbexpressiondataarray
{
public:
	hkbexpressiondataarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

extern void hkbExpressionDataArrayExport(std::string id);

#endif