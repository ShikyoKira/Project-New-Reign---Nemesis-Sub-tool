#ifndef BSMODIFYONCEMODIFIER_H_
#define BSMODIFYONCEMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bsmodifyoncemodifier : public hkbmodifier, std::enable_shared_from_this<bsmodifyoncemodifier>
{
public:
	bsmodifyoncemodifier() {}
	bsmodifyoncemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetModifier(int number);
	bool IsModifierNull(int number);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string modifier1;
	std::string modifier2;
	std::string variablebindingset;
	bool IsNegated = false;


	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::shared_ptr<hkbmodifier> pOnActivateModifier;
	std::shared_ptr<hkbmodifier> pOnDeactivateModifier;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void BSModifyOnceModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<bsmodifyoncemodifier>> bsmodifyoncemodifierList;
extern safeStringUMap<std::shared_ptr<bsmodifyoncemodifier>> bsmodifyoncemodifierList_E;

#endif