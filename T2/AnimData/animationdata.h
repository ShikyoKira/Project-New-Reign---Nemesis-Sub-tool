#ifndef ANIMATIONDATA_H_
#define ANIMATIONDATA_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "Global.h"

typedef std::vector<std::string> vecstr;

struct AnimDataFunct
{
	bool isNew = false;
	bool isCondition = false;
	bool isConditionOri = false;
	bool nextCondition = false;
	bool skip = false;

	int conditionOpen = false;
};

namespace AnimDataFormat
{
	enum position
	{
		totallinecount,
		behaviorfilecount,
		behaviorfilelist,
		animationname,
		uniquecode,
		eventnamecount,
		eventnamelist,
		totalcliplength,
		motiondatacount,
		motiondatalist,
		rotationdatacount,
		rotationdatalist,
		unknown2,
		unknown3,
		unknown4,
		unknown5,
		space,
		xerror,
		null
	};
}

struct AnimDataPack
{
	// anim data
	std::string name;
	std::string uniquecode;
	std::string unknown1;
	std::string unknown2;
	std::string unknown3;
	vecstr eventname;
};

struct InfoDataPack
{
	std::string uniquecode;
	std::string duration;
	vecstr motiondata;
	vecstr rotationdata;
};

struct AnimDataProject
{
	std::string unknown1;
	vecstr behaviorlist;
	std::string unknown2;

	std::vector<AnimDataPack> animdatalist;
	std::vector<InfoDataPack> infodatalist;

	AnimDataProject() {}
	AnimDataProject(vecstr animdatafile, std::string filename, std::string filepath, std::string modcode = "nemesis");
	int GetAnimTotalLine();
	int GetInfoTotalLine();
};

void BehaviorListProcess(AnimDataProject& storeline, int& startline, vecstr& animdatafile, std::string project, std::string modcode);
void AnimDataProcess(std::vector<AnimDataPack>& storeline, int& startline, vecstr& animdatafile, std::string project, std::string modcode);
void InfoDataProcess(std::vector<InfoDataPack>& storeline, int& startline, vecstr& animdatafile, std::string project, std::string modcode);

#endif