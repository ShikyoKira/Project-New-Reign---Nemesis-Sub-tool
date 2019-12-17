#ifndef HKBMODIFIERGENERATOR_H_
#define HKBMODIFIERGENERATOR_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"
#include "src\hkx\generator\hkbgenerator.h"

struct hkbmodifier;

struct hkbmodifiergenerator : public hkbgenerator, std::enable_shared_from_this<hkbmodifiergenerator>
{
public:
	hkbmodifiergenerator() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::shared_ptr<hkbmodifier> modifier;
	std::shared_ptr<hkbgenerator> generator;

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

extern safeStringUMap<std::shared_ptr<hkbmodifiergenerator>> hkbmodifiergeneratorList;
extern safeStringUMap<std::shared_ptr<hkbmodifiergenerator>> hkbmodifiergeneratorList_E;

#endif