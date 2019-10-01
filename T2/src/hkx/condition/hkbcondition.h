#ifndef HKBCONDITION_H_
#define HKBCONDITION_H_

#include "src\hkx\hkbobject.h"

struct hkbcondition : public hkbobject
{
	std::string previousID;
	hkbcondition() { isCondition = true; }
};

extern safeStringUMap<std::shared_ptr<hkbcondition>> hkbconditionList_E;
extern safeStringUMap<std::shared_ptr<hkbcondition>> hkbconditionList;

#endif
