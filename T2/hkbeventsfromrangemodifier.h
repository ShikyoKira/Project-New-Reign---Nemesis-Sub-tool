#ifndef HKBEVENTSFROMRANGEMODIFIER_H_
#define HKBEVENTSFROMRANGEMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbeventsfromrangemodifier
{
public:
	hkbeventsfromrangemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetEventRanges();
	bool IsEventRangesNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string eventranges;
	bool IsNegated = false;
};

extern void hkbEventsFromRangeModifierExport(std::string id);

#endif