#ifndef ANIMATIONDATA_H_
#define ANIMATIONDATA_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "Global.h"

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
	bool proxy = true;

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
	bool proxy = true;

	std::string uniquecode;
	std::string duration;
	vecstr motiondata;
	vecstr rotationdata;
};

struct AnimDataProject
{
	bool proxy = true;

	std::string name;
	std::string filename;
	std::string unknown1;
	vecstr behaviorlist;
	std::string unknown2;

	std::vector<AnimDataPack> animdatalist;
	std::vector<InfoDataPack> infodatalist;

	AnimDataProject() {}
	AnimDataProject(vecstr animdatafile, std::string projectname, std::string projectfile);
	int GetAnimTotalLine();
	int GetInfoTotalLine();
};

void animDataInitialize(std::string filepath, vecstr& storeline, std::vector<std::shared_ptr<AnimDataProject>>& ADProject);

#endif