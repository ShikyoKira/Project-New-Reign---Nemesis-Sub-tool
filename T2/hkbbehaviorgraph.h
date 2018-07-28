#ifndef HKBBEHAVIORGRAPH_H_
#define HKBBEHAVIORGRAPH_H_

#include <iostream>
#include <string>
#include <fstream>

struct hkbbehaviorgraph
{
public:
	hkbbehaviorgraph(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetData();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string generator;
	std::string variablebindingset;
	std::string data;
	bool IsNegated = false;
};

extern void hkbBehaviorGraphExport(std::string id);

#endif