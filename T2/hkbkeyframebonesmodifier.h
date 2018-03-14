#ifndef HKBKEYFRAMEBONESMODIFIER_H_
#define HKBKEYFRAMEBONESMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbkeyframebonesmodifier
{
public:
	hkbkeyframebonesmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetKeyframedBonesList();
	bool IsKeyframedBonesListNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string keyframedbonelist;
	bool IsNegated = false;
};

extern void hkbKeyframeBonesModifierExport(std::string originalfile, std::string editedfile, std::string id);

#endif