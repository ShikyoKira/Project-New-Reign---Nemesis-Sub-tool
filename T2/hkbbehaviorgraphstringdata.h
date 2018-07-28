#ifndef HKBBEHAVIORGRAPHSTRINGDATA_H_
#define HKBBEHAVIORGRAPHSTRINGDATA_H_

#include <string>
#include <iostream>
#include <fstream>

struct hkbbehaviorgraphstringdata
{
public:
	hkbbehaviorgraphstringdata(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

extern void hkbBehaviorGraphStringDataExport(std::string id);

#endif