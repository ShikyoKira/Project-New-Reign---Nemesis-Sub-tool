#ifndef HKBBEHAVIORGRAPH_H_
#define HKBBEHAVIORGRAPH_H_

#include <iostream>
#include <string>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbbehaviorgraphdata.h"
#include "src\hkx\generator\hkbgenerator.h"

struct hkbbehaviorgraph : public hkbgenerator, std::enable_shared_from_this<hkbbehaviorgraph>
{
public:
	hkbbehaviorgraph() {}

	std::string GetAddress();

	std::string tempaddress;

	enum variablemode
	{
		VARIABLE_MODE_DISCARD_WHEN_INACTIVE,
		VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE
	};

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	variablemode variableMode;
	std::shared_ptr<hkbgenerator> rootGenerator;
	std::shared_ptr<hkbbehaviorgraphdata> data;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getVariableMode();
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbbehaviorgraph>> hkbbehaviorgraphList;
extern safeStringUMap<std::shared_ptr<hkbbehaviorgraph>> hkbbehaviorgraphList_E;

#endif