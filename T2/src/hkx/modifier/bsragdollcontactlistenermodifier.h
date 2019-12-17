#ifndef BSRAGDOLLCONTACTLISTENERMODIFIER_H_
#define BSRAGDOLLCONTACTLISTENERMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"
#include "src\hkx\bone\hkbboneindexarray.h"

struct bsragdollcontactlistenermodifier : public hkbmodifier, std::enable_shared_from_this<bsragdollcontactlistenermodifier>
{
public:
	bsragdollcontactlistenermodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	eventproperty event;
	std::shared_ptr<hkbboneindexarray> bones;

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

extern safeStringUMap<std::shared_ptr<bsragdollcontactlistenermodifier>> bsragdollcontactlistenermodifierList;
extern safeStringUMap<std::shared_ptr<bsragdollcontactlistenermodifier>> bsragdollcontactlistenermodifierList_E;

#endif