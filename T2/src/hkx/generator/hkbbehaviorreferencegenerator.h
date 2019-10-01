#ifndef HKBBEHAVIORREFERENCEGENERATOR_H_
#define HKBBEHAVIORREFERENCEGENERATOR_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"

struct hkbbehaviorreferencegenerator : public hkbgenerator, std::enable_shared_from_this<hkbbehaviorreferencegenerator>
{
public:
	hkbbehaviorreferencegenerator() {}
	hkbbehaviorreferencegenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	bool IsNegated = false;

	
	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::string behaviorName;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
};

void hkbBehaviorReferenceGeneratorExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbbehaviorreferencegenerator>> hkbbehaviorreferencegeneratorList;
extern safeStringUMap<std::shared_ptr<hkbbehaviorreferencegenerator>> hkbbehaviorreferencegeneratorList_E;

#endif