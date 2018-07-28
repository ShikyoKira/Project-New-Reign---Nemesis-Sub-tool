#ifndef HKBBEHAVIORGRAPHDATA_H_
#define HKBBEHAVIORGRAPHDATA_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbbehaviorgraphdata
{
public:
	hkbbehaviorgraphdata(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableInitialValues();
	std::string GetStringData();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variableinitialvalues;
	std::string stringdata;
	bool IsNegated = false;
};

extern void hkbBehaviorGraphDataExport(std::string id);

#endif