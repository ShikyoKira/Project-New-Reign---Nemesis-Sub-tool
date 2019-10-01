#ifndef HKBTWISTMODIFIER_H_
#define HKBTWISTMODIFIER_H_

#include <iostream>
#include <fstream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbtwistmodifier : public hkbmodifier, std::enable_shared_from_this<hkbtwistmodifier>
{
public:
	hkbtwistmodifier() {}
	hkbtwistmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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


	enum setanglemethod
	{
		LINEAR,
		RAMPED
	};

	enum rotationaxiscoordinates
	{
		ROTATION_AXIS_IN_MODEL_COORDINATES,
		ROTATION_AXIS_IN_LOCAL_COORDINATES
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	coordinate axisOfRotation;
	double twistAngle;
	int startBoneIndex;
	int endBoneIndex;
	setanglemethod setAngleMethod;
	rotationaxiscoordinates rotationAxisCoordinates;
	bool isAdditive;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getAngleMethod();
	std::string getRotationAxis();
};

void hkbTwistModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbtwistmodifier>> hkbtwistmodifierList;
extern safeStringUMap<std::shared_ptr<hkbtwistmodifier>> hkbtwistmodifierList_E;

#endif