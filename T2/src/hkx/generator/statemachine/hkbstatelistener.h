#ifndef HKBSTATELISTENER_H_
#define HKBSTATELISTENER_H_

#include "src\hkx\hkbobject.h"

struct hkbstatelistener : public hkbobject, std::enable_shared_from_this<hkbstatelistener>
{
	hkbstatelistener() {}

	void regis(std::string id, bool isEdited);
	std::string getClassCode();
};

extern safeStringUMap<std::shared_ptr<hkbstatelistener>> hkbstatelistenerList;
extern safeStringUMap<std::shared_ptr<hkbstatelistener>> hkbstatelistenerList_E;

#endif
