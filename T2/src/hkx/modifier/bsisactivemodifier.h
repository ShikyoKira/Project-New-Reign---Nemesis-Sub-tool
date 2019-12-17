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

	std::string GetAddress();

	std::string tempaddress;

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

extern safeStringUMap<std::shared_ptr<bsisactivemodifier>> bsisactivemodifierList;
extern safeStringUMap<std::shared_ptr<bsisactivemodifier>> bsisactivemodifierList_E;

#endif