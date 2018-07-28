#ifndef HKROOTLEVELCONTAINER_H_
#define HKROOTLEVELCONTAINER_H_

#include <iostream>
#include <string>
#include <fstream>

struct hkrootlevelcontainer
{
public:
	hkrootlevelcontainer(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();
	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string generator;
	bool IsNegated = false;
};

extern void hkRootLevelContainerExport(std::string id);

#endif