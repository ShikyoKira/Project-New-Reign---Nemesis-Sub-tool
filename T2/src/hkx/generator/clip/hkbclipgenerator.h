#ifndef HKBCLIPGENERATOR_H_
#define HKBCLIPGENERATOR_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\generator\clip\hkbcliptriggerarray.h"

struct hkbclipgenerator : public hkbgenerator, std::enable_shared_from_this<hkbclipgenerator>
{
public:
	hkbclipgenerator() {}

	std::string GetAddress();
	
	std::string tempaddress;

	enum playbackmode
	{
		MODE_SINGLE_PLAY,
		MODE_LOOPING,
		MODE_USER_CONTROLLED,
		MODE_PING_PONG,
		MODE_COUNT
	};

	enum clipflags
	{
		FLAG_CONTINUE_MOTION_AT_END = 1,
		FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE = 2,
		FLAG_MIRROR = 4,
		FLAG_FORCE_DENSE_POSE = 8,
		FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS = 16,
		FLAG_IGNORE_MOTION = 32
	};

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	std::string animationName;
	std::shared_ptr<hkbcliptriggerarray> triggers;
	double cropStartAmountLocalTime;
	double cropEndAmountLocalTime;
	double startTime;
	double playbackSpeed;
	double enforcedDuration;
	double userControlledTimeFraction;
	int animationBindingIndex;
	playbackmode mode;
	clipflags flags;

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
	std::string getMode();
};

extern safeStringUMap<std::shared_ptr<hkbclipgenerator>> hkbclipgeneratorList;
extern safeStringUMap<std::shared_ptr<hkbclipgenerator>> hkbclipgeneratorList_E;

#endif