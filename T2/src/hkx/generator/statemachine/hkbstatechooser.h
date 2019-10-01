#ifndef HKBSTATECHOOSER_H_
#define HKBSTATECHOOSER_H_

#include "src\hkx\hkbobject.h"

struct hkbstatechooser : public hkbobject, std::enable_shared_from_this<hkbstatechooser>
{
	hkbstatechooser() {}

	void regis(std::string id, bool isEdited);
	std::string getClassCode();
};

extern safeStringUMap<std::shared_ptr<hkbstatechooser>> hkbstatechooserList;
extern safeStringUMap<std::shared_ptr<hkbstatechooser>> hkbstatechooserList_E;

#endif
