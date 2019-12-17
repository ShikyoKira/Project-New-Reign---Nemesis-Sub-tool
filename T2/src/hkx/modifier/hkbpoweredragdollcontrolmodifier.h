#ifndef HKBPOWEREDRAGDOLLCONTROLMODIFIER_H_
#define HKBPOWEREDRAGDOLLCONTROLMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"
#include "src\hkx\bone\hkbboneindexarray.h"
#include "src\hkx\bone\hkbboneweightarray.h"

struct hkbpoweredragdollcontrolmodifier : public hkbmodifier, std::enable_shared_from_this<hkbpoweredragdollcontrolmodifier>
{
public:
	hkbpoweredragdollcontrolmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct controldata
	{
		double maxForce;
		double tau;
		double damping;
		double proportionalRecoveryVelocity;
		double constantRecoveryVelocity;
	};

	struct worldfrommodelmodedata
	{
		enum worldfrommodelmode
		{
			WORLD_FROM_MODEL_MODE_USE_OLD,
			WORLD_FROM_MODEL_MODE_USE_INPUT,
			WORLD_FROM_MODEL_MODE_COMPUTE,
			WORLD_FROM_MODEL_MODE_NONE,
			WORLD_FROM_MODEL_MODE_RAGDOLL
		};

		int poseMatchingBone0;
		int poseMatchingBone1;
		int poseMatchingBone2;
		worldfrommodelmode mode;

		std::string getMode();
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	controldata controlData;
	std::shared_ptr<hkbboneindexarray> bones;
	worldfrommodelmodedata worldFromModelModeData;
	std::shared_ptr<hkbboneweightarray> boneWeights;

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

extern safeStringUMap<std::shared_ptr<hkbpoweredragdollcontrolmodifier>> hkbpoweredragdollcontrolmodifierList;
extern safeStringUMap<std::shared_ptr<hkbpoweredragdollcontrolmodifier>> hkbpoweredragdollcontrolmodifierList_E;

#endif