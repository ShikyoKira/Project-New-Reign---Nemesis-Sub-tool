#ifndef HKBSENSEHANDLEMODIFIER_H_
#define HKBSENSEHANDLEMODIFIER_H_

#include <iostream>
#include <fstream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbhandle.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbsensehandlemodifier : public hkbmodifier, std::enable_shared_from_this<hkbsensehandlemodifier>
{
public:
	hkbsensehandlemodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct range
	{
		bool proxy;

		eventproperty event;
		double minDistance;
		double maxDistance;
		bool ignoreHandle;

		range() : proxy(true) {}
		range(int id) : event(id), proxy(false) {}
	};

	enum sensingmode
	{
		SENSE_IN_NEARBY_RIGID_BODIES,
		SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER,
		SENSE_IN_OTHER_CHARACTER_RIGID_BODIES,
		SENSE_IN_THIS_CHARACTER_RIGID_BODIES,
		SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES,
		SENSE_IN_GIVEN_RIGID_BODY,
		SENSE_IN_OTHER_CHARACTER_SKELETON,
		SENSE_IN_THIS_CHARACTER_SKELETON,
		SENSE_IN_GIVEN_CHARACTER_SKELETON,
		SENSE_IN_GIVEN_LOCAL_FRAME_GROUP
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	coordinate sensorLocalOffset;
	std::vector<range> ranges;
	std::shared_ptr<hkbhandle> handleOut;
	std::shared_ptr<hkbhandle> handleIn;
	std::string localFrameName;
	std::string sensorLocalFrameName;
	double minDistance;
	double maxDistance;
	double distanceOut;
	int collisionFilterInfo;
	int sensorRagdollBoneIndex;
	int sensorAnimationBoneIndex;
	sensingmode sensingMode;
	bool extrapolateSensorPosition;
	bool keepFirstSensedHandle;
	bool foundHandleOut;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<range>& ori, std::vector<range>& edit, std::string id);
	std::string getSensingMode();
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbsensehandlemodifier>> hkbsensehandlemodifierList;
extern safeStringUMap<std::shared_ptr<hkbsensehandlemodifier>> hkbsensehandlemodifierList_E;

#endif