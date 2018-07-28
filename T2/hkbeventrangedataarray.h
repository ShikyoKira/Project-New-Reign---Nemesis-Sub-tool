#ifndef HKBEVENTRANGEDATAARRAY_H_
#define HKBEVENTRANGEDATAARRAY_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbeventrangedataarray
{
public:
	hkbeventrangedataarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	int GetPayloadCount();
	std::string GetPayload(int child);
	bool IsPayloadNull(int child);

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::vector<std::string> payload;
	bool IsNegated = false;
};

extern void hkbEventRangeDataArrayExport(std::string id);

#endif