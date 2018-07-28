#ifndef HKBPOWEREDRAGDOLLCONTROLMODIFIER_H_
#define HKBPOWEREDRAGDOLLCONTROLMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbpoweredragdollcontrolmodifier
{
public:
	hkbpoweredragdollcontrolmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetBone();
	bool IsBoneNull();

	std::string GetBoneWeight();
	bool IsBoneWeightNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string bone;
	std::string boneweight;
	bool IsNegated = false;
};

extern void hkbPoweredRagdollControlsModifierExport(std::string id);

#endif