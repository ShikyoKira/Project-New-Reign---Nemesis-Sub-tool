#ifndef HKBSTATEMACHINEEVENTPROPERTYARRAY_H_
#define HKBSTATEMACHINEEVENTPROPERTYARRAY_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"

struct hkbstatemachineeventpropertyarray : public hkbobject, std::enable_shared_from_this<hkbstatemachineeventpropertyarray>
{
public:
	hkbstatemachineeventpropertyarray() {}

	std::string GetAddress();

	std::string tempaddress;

	std::vector<eventproperty> events;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<eventproperty>& ori, std::vector<eventproperty>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbstatemachineeventpropertyarray>> hkbstatemachineeventpropertyarrayList;
extern safeStringUMap<std::shared_ptr<hkbstatemachineeventpropertyarray>> hkbstatemachineeventpropertyarrayList_E;

#endif