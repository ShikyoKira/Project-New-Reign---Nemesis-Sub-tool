#ifndef HKBGENERATOR_H_
#define HKBGENERATOR_H_

#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbvariablebindingset.h"

struct hkbgenerator : public hkbobject
{
	hkbgenerator() { isGenerator = true; }

	std::shared_ptr<hkbvariablebindingset> variableBindingSet;
	int userData;
	std::string name;
};

extern safeStringUMap<std::shared_ptr<hkbgenerator>> hkbgeneratorList;
extern safeStringUMap<std::shared_ptr<hkbgenerator>> hkbgeneratorList_E;

#endif
