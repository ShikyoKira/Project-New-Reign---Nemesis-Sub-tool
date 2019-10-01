#ifndef BSBONESWITCHGENERATOR_H_
#define BSBONESWITCHGENERATOR_H_

#include <iostream>
#include <fstream>
#include "bsboneswitchgeneratorbonedata.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"

struct bsboneswitchgenerator : public hkbgenerator, std::enable_shared_from_this<bsboneswitchgenerator>
{
public:
	bsboneswitchgenerator() {}
	bsboneswitchgenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetBoneData(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr bonedata;
	std::string variablebindingset;
	std::string generator;
	int children;
	bool IsNegated = false;


	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::shared_ptr<hkbgenerator> pDefaultGenerator;
	std::vector<std::shared_ptr<bsboneswitchgeneratorbonedata>> ChildrenA;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<std::shared_ptr<bsboneswitchgeneratorbonedata>>& ori, std::vector<std::shared_ptr<bsboneswitchgeneratorbonedata>>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void BSBoneSwitchGeneratorExport(std::string id);

extern safeStringUMap<std::shared_ptr<bsboneswitchgenerator>> bsboneswitchgeneratorList;
extern safeStringUMap<std::shared_ptr<bsboneswitchgenerator>> bsboneswitchgeneratorList_E;

#endif