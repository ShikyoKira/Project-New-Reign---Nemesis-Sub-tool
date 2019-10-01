#ifndef HKBRIGIDBODYRAGDOLLCONTROLSMODIFIER_H_
#define HKBRIGIDBODYRAGDOLLCONTROLSMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\bone\hkbboneindexarray.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbrigidbodyragdollcontrolsmodifier : public hkbmodifier, std::enable_shared_from_this<hkbrigidbodyragdollcontrolsmodifier>
{
public:
	hkbrigidbodyragdollcontrolsmodifier() {}
	hkbrigidbodyragdollcontrolsmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetBones();
	bool IsBonesNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	std::string s_bones;
	bool IsNegated = false;

	
	struct controldata
	{
		struct keyframehierarchycontroldata
		{
			double hierarchyGain;
			double velocityDamping;
			double accelerationGain;
			double velocityGain;
			double positionGain;
			double positionMaxLinearVelocity;
			double positionMaxAngularVelocity;
			double snapGain;
			double snapMaxLinearVelocity;
			double snapMaxAngularVelocity;
			double snapMaxLinearDistance;
			double snapMaxAngularDistance;
		};

		keyframehierarchycontroldata keyframehierarchycontroldata;
		double durationToBlend;
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	controldata controlData;
	std::shared_ptr<hkbboneindexarray> bones;

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

void hkbRigidBodyRagdollControlsModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbrigidbodyragdollcontrolsmodifier>> hkbrigidbodyragdollcontrolsmodifierList;
extern safeStringUMap<std::shared_ptr<hkbrigidbodyragdollcontrolsmodifier>> hkbrigidbodyragdollcontrolsmodifierList_E;

#endif