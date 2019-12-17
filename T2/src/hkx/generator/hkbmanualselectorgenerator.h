#ifndef HKBMANUALSELECTORGENERATOR_H_
#define HKBMANUALSELECTORGENERATOR_H_

#include <fstream>
#include <iostream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"

struct hkbmanualselectorgenerator : public hkbgenerator, std::enable_shared_from_this<hkbmanualselectorgenerator>
{
public:
	hkbmanualselectorgenerator() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::vector<std::shared_ptr<hkbgenerator>> generators;
	int selectedGeneratorIndex;
	int currentGeneratorIndex;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<std::shared_ptr<hkbgenerator>>& ori, std::vector<std::shared_ptr<hkbgenerator>>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbmanualselectorgenerator>> hkbmanualselectorgeneratorList;
extern safeStringUMap<std::shared_ptr<hkbmanualselectorgenerator>> hkbmanualselectorgeneratorList_E;

#endif