#ifndef HKBSTATEMACHINETRANSITIONINTOARRAY_H_
#define HKBSTATEMACHINETRANSITIONINTOARRAY_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbstatemachinetransitioninfoarray
{
public:
	hkbstatemachinetransitioninfoarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetTransition(int number);
	int GetTransitionCount();

	std::string GetCondition(int number);
	int GetConditionCount();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::vector<std::string> transition;
	std::vector<std::string> condition;
	bool IsNegated = false;
};

extern void hkbStateMachineTransitionInfoArrayExport(std::string originalfile, std::string editedfile, std::string id);

#endif