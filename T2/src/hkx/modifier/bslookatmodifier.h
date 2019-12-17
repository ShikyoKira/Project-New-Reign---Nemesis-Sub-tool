#ifndef BSLOOKATMODIFIER_H_
#define BSLOOKATMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bslookatmodifier : public hkbmodifier, std::enable_shared_from_this<bslookatmodifier>
{
public:
	bslookatmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct bone
	{
		bool proxy;

		int index;
		coordinate fwdAxisLS;
		double limitAngleDegrees;
		double onGain;
		double offGain;
		bool enable;

		bone() : proxy(true) {}
		bone(int n_index) : index(n_index), proxy(false) {}
	};
	
	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	bool lookAtTarget;
	std::vector<bone> bones;
	std::vector<bone> eyeBones;
	int limitAngleDegrees;
	int limitAngleThresholdDegrees;
	bool continueLookOutsideOfLimit;
	double onGain;
	double offGain;
	bool useBoneGains;
	coordinate targetLocation;
	bool targetOutsideLimits;
	eventproperty targetOutOfLimitEvent;
	bool lookAtCamera;
	double lookAtCameraX;
	double lookAtCameraY;
	double lookAtCameraZ;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<bone>& ori, std::vector<bone>& edit, std::string id);
	void elementCompare(std::vector<bone>& ori, std::vector<bone>& edit, vecstr& output, vecstr& storeline, std::string classname, usize& base, bool& open, bool& isEdited);
	void elementNew(vecstr& output, bone& bone, usize& base);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<bslookatmodifier>> bslookatmodifierList;
extern safeStringUMap<std::shared_ptr<bslookatmodifier>> bslookatmodifierList_E;

#endif