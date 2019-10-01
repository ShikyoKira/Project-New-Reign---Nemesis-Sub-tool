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
	hkbstatemachineeventpropertyarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	int GetPayloadCount();
	std::string GetPayload(int child);
	bool IsPayloadNull(int hchild);
	
	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr payload;
	bool IsNegated = false;

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

void hkbStateMachineEventPropertyArrayExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbstatemachineeventpropertyarray>> hkbstatemachineeventpropertyarrayList;
extern safeStringUMap<std::shared_ptr<hkbstatemachineeventpropertyarray>> hkbstatemachineeventpropertyarrayList_E;

#endif