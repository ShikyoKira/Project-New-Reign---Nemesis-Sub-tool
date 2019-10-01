#ifndef BSISACTIVEMODIFIER_H_
#define BSISACTIVEMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bsisactivemodifier : public hkbmodifier, std::enable_shared_from_this<bsisactivemodifier>
{
public:
	bsisactivemodifier() {}
	bsisactivemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	bool IsNegated = false;


	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	bool bIsActive0;
	bool bInvertActive0;
	bool bIsActive1;
	bool bInvertActive1;
	bool bIsActive2;
	bool bInvertActive2;
	bool bIsActive3;
	bool bInvertActive3;
	bool bIsActive4;
	bool bInvertActive4;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
};

void BSIsActiveModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<bsisactivemodifier>> bsisactivemodifierList;
extern safeStringUMap<std::shared_ptr<bsisactivemodifier>> bsisactivemodifierList_E;

#endif