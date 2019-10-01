#ifndef BSCYCLICBLENDERTRANSITIONGENERATOR_H_
#define BSCYCLICBLENDERTRANSITIONGENERATOR_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\blendcurve.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\hkbstringeventpayload.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\generator\blender\hkbblendergenerator.h"

struct bscyclicblendtransitiongenerator : public hkbgenerator, std::enable_shared_from_this<bscyclicblendtransitiongenerator>
{
public:
	bscyclicblendtransitiongenerator() {}
	bscyclicblendtransitiongenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	int GetPayloadCount();
	std::string GetPayload(int child);
	bool IsPayloadNull(int child);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string generator;
	vecstr payload;
	std::string variablebindingset;
	bool IsNegated = false;

	
	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::shared_ptr<hkbgenerator> pBlenderGenerator;
	eventproperty EventToFreezeBlendValue;
	eventproperty EventToCrossBlend;
	double fBlendParameter;
	double fTransitionDuration;
	blendcurve eBlendCurve;

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

void BSCyclicBlendTransitionGeneratorExport(std::string id);

extern safeStringUMap<std::shared_ptr<bscyclicblendtransitiongenerator>> bscyclicblendtransitiongeneratorList;
extern safeStringUMap<std::shared_ptr<bscyclicblendtransitiongenerator>> bscyclicblendtransitiongeneratorList_E;

#endif