#ifndef HKBCHARACTERDATA_H_
#define HKBCHARACTERDATA_H_

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

struct hkbcharacterdata
{
public:
	hkbcharacterdata(std::string filepath, std::string id, std::string preaddress, int functionLayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetStringData();
	std::string GetSkeletonInfo();

	std::string GetCharPropertyValues();
	bool HasCharProperty();

	std::string GetFootIK();
	bool HasFootIK();

	std::string GetHandIK();
	bool HasHandIK();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string charPropertyID;
	std::string footIKID;
	std::string handIKID;
	std::string stringData;
	std::string skeleton;
	bool IsNegated = false;
};

extern void hkbCharacterDataExport(std::string id);

#endif
