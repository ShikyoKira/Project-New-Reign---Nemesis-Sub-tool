#ifndef HKBCLIPTRIGGERARRAY_H_
#define HKBCLIPTRIGGERARRAY_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbcliptriggerarray
{
public:
	hkbcliptriggerarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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
	int payloadcount;
	bool IsNegated = false;
};

extern void hkbClipTriggerArrayExport(std::string originalfile, std::string editedfile, std::string id);

#endif