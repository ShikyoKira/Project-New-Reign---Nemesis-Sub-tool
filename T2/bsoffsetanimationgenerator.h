#ifndef BSOFFSETANIMATIONGENERATOR_H_
#define BSOFFSETANIMATIONGENERATOR_H_

#include <string>
#include <iostream>
#include <fstream>

struct bsoffsetanimationgenerator
{
public:
	bsoffsetanimationgenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetClipGenerator();
	bool IsClipNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string generator;
	std::string variablebindingset;
	std::string clipgenerator;
	bool IsNegated = false;
};

extern void BSOffsetAnimationGeneratorExport(std::string id);

#endif