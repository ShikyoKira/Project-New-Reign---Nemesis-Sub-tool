#ifndef HKBMIRROREDSKELETONINFO_H_
#define HKBMIRROREDSKELETONINFO_H_

#include <iostream>
#include <fstream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"

struct hkbmirroredskeletoninfo : public hkbobject, std::enable_shared_from_this<hkbmirroredskeletoninfo>
{
public:
	hkbmirroredskeletoninfo() {}

	std::string GetAddress();

	std::string tempaddress;

	coordinate mirrorAxis;
	std::vector<int> bonePairMap;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
};

extern safeStringUMap<std::shared_ptr<hkbmirroredskeletoninfo>> hkbmirroredskeletoninfoList;
extern safeStringUMap<std::shared_ptr<hkbmirroredskeletoninfo>> hkbmirroredskeletoninfoList_E;

#endif