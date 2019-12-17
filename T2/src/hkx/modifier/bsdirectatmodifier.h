#ifndef BSDIRECTATMODIFIER_H_
#define BSDIRECTATMODIFIER_H_

#include <iostream>
#include <fstream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bsdirectatmodifier : public hkbmodifier, std::enable_shared_from_this<bsdirectatmodifier>
{
public:
	bsdirectatmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	bool directAtTarget;
	int sourceBoneIndex;
	int startBoneIndex;
	int endBoneIndex;
	double limitHeadingDegrees;
	double limitPitchDegrees;
	double offsetHeadingDegrees;
	double offsetPitchDegrees;
	double onGain;
	double offGain;
	coordinate targetLocation;
	int userInfo;
	bool directAtCamera;
	double directAtCameraX;
	double directAtCameraY;
	double directAtCameraZ;
	bool active;
	double currentHeadingOffset;
	double currentPitchOffset;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<bsdirectatmodifier>> bsdirectatmodifierList;
extern safeStringUMap<std::shared_ptr<bsdirectatmodifier>> bsdirectatmodifierList_E;

#endif