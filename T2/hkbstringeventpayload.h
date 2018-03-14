#ifndef HKBSTRINGEVENTPAYLOAD_H_
#define HKBSTRINGEVENTPAYLOAD_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbstringeventpayload
{
public:
	hkbstringeventpayload(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

extern void hkbStringEventPayloadExport(std::string originalfile, std::string editedfile, std::string id);

#endif