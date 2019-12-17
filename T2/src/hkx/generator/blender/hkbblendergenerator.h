#ifndef HKBBLENDERGENERATOR_H_
#define HKBBLENDERGENERATOR_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\generator\blender\hkbblendergeneratorchild.h"

struct blenderBase : public hkbgenerator
{
	enum blenderflags
	{
		FLAG_SYNC = 1,
		FLAG_SMOOTH_GENERATOR_WEIGHTS = 4,
		FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS = 8,
		FLAG_PARAMETRIC_BLEND = 16,
		FLAG_IS_PARAMETRIC_BLEND_CYCLIC = 32,
		FLAG_FORCE_DENSE_POSE = 64
	};

	double referencePoseWeightThreshold;
	double blendParameter;
	double minCyclicBlendParameter;
	double maxCyclicBlendParameter;
	int indexOfSyncMasterChild;
	blenderflags flags;
	bool subtractLastChild;
	std::vector<std::shared_ptr<hkbblendergeneratorchild>> children;

protected:
	void matchScoring(std::vector<std::shared_ptr<hkbblendergeneratorchild>>& ori, std::vector<std::shared_ptr<hkbblendergeneratorchild>>& edit, std::string id,
		std::string classname);
};

struct hkbblendergenerator : public blenderBase, std::enable_shared_from_this<hkbblendergenerator>
{
public:
	hkbblendergenerator() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	// double referencePoseWeightThreshold from blenderBase
	// double blendParameter from blenderBase
	// double minCyclicBlendParameter from blenderBase
	// double maxCyclicBlendParameter from blenderBase
	// int indexOfSyncMasterChild from blenderBase
	// blenderflags flags from blenderBase
	// bool subtractLastChild from blenderBase
	// std::vector<std::shared_ptr<hkbblendergeneratorchild>> children from blenderBase

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

extern safeStringUMap<std::shared_ptr<hkbblendergenerator>> hkbblendergeneratorList;
extern safeStringUMap<std::shared_ptr<hkbblendergenerator>> hkbblendergeneratorList_E;

#endif