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
	hkbmodifiergenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();
	std::string GetModifier();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string s_generator;
	std::string variablebindingset;
	std::string s_modifier;
	bool IsNegated = false;


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

void hkbModifierGeneratorExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbmodifiergenerator>> hkbmodifiergeneratorList;
extern safeStringUMap<std::shared_ptr<hkbmodifiergenerator>> hkbmodifiergeneratorList_E;

#endif