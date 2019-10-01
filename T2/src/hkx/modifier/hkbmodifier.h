#ifndef HKBMODIFIER_H_
#define HKBMODIFIER_H_

#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbvariablebindingset.h"

struct hkbmodifier : public hkbobject
{
	hkbmodifier() { isModifier = true; }

	std::shared_ptr<hkbvariablebindingset> variableBindingSet;
	int userData;
	std::string name;
	bool enable;
};

extern safeStringUMap<std::shared_ptr<hkbmodifier>> hkbmodifierList;
extern safeStringUMap<std::shared_ptr<hkbmodifier>> hkbmodifierList_E;

#endif
