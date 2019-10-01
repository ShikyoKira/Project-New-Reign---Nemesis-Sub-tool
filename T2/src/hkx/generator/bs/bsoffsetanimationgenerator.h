#ifndef BSOFFSETANIMATIONGENERATOR_H_
#define BSOFFSETANIMATIONGENERATOR_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"

struct bsoffsetanimationgenerator : public hkbgenerator, std::enable_shared_from_this<bsoffsetanimationgenerator>
{
public:
	bsoffsetanimationgenerator() {}
	bsoffsetanimationgenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetClipGenerator();
	bool IsClipNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string generator;
	std::string variablebindingset;
	std::string clipgenerator;
	bool IsNegated = false;


	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::shared_ptr<hkbgenerator> pDefaultGenerator;
	std::shared_ptr<hkbgenerator> pOffsetClipGenerator;
	double fOffsetVariable;
	double fOffsetRangeStart;
	double fOffsetRangeEnd;

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

void BSOffsetAnimationGeneratorExport(std::string id);

extern safeStringUMap<std::shared_ptr<bsoffsetanimationgenerator>> bsoffsetanimationgeneratorList;
extern safeStringUMap<std::shared_ptr<bsoffsetanimationgenerator>> bsoffsetanimationgeneratorList_E;

#endif