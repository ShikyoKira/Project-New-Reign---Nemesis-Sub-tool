#ifndef HKBBLENDERGENERATORCHILD_H_
#define HKBBLENDERGENERATORCHILD_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\bone\hkbboneweightarray.h"


struct hkbblendergeneratorchild : public hkbobject, std::enable_shared_from_this<hkbblendergeneratorchild>
{
public:
	hkbblendergeneratorchild() {}
	hkbblendergeneratorchild(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetBoneWeights();
	bool IsBoneWeightsNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string s_generator;
	std::string variablebindingset;
	std::string boneweights;
	bool IsNegated = false;


	std::shared_ptr<hkbvariablebindingset> variableBindingSet;
	std::shared_ptr<hkbgenerator> generator;
	std::shared_ptr<hkbboneweightarray> boneWeights;
	double weight;
	double worldFromModelWeight;

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

void hkbBlenderGeneratorChildExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbblendergeneratorchild>> hkbblendergeneratorchildList;
extern safeStringUMap<std::shared_ptr<hkbblendergeneratorchild>> hkbblendergeneratorchildList_E;

#endif