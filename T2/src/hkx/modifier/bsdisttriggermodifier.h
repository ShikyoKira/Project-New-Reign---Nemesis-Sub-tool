#ifndef BSDISTTRIGGERMODIFIER_H_
#define BSDISTTRIGGERMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bsdisttriggermodifier : public hkbmodifier, std::enable_shared_from_this<bsdisttriggermodifier>
{
public:
	bsdisttriggermodifier() {}

	std::string GetAddress();

	std::string tempaddress;
	
	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	coordinate targetPosition;
	double distance;
	double distanceTrigger;
	eventproperty triggerEvent;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<bsdisttriggermodifier>> bsdisttriggermodifierList;
extern safeStringUMap<std::shared_ptr<bsdisttriggermodifier>> bsdisttriggermodifierList_E;

#endif