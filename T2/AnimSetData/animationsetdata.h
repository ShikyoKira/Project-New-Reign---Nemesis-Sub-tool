#ifndef ANIMATIONSETDATA_H_
#define ANIMATIONSETDATA_H_

#include <iostream>
#include <boost\algorithm\string.hpp>
#include <unordered_map>
#include <vector>
#include <map>
#include <set>
#include "alphanum.hpp"
#include "Global.h"

typedef std::vector<std::string> vecstr;

struct attackdata
{
	bool proxy = true;

	std::string data;
};

struct equip
{
	bool proxy = true;

	std::string name;
};

struct typepack
{
	bool proxy = true;

	std::string name;
	std::string equiptype1;
	std::string equiptype2;
};

struct animpack
{
	bool proxy = true;

	std::string name;
	std::string unknown;
	std::vector<attackdata> attack;
};

struct crc32
{
	bool proxy = true;

	std::string filepath;
	std::string filename;
	std::string fileformat;
};

struct datapack
{
	bool proxy = true;

	std::vector<equip> equiplist;		// anim data set, equip list
	std::vector<typepack> typelist;		// anim data set, type list
	std::vector<animpack> animlist;		// anim data set, list of animpack
	std::vector<crc32> crc32list;		// anim data set, crc32 list
};

struct AnimSetDataProject
{
	bool proxy = true;					// is this new set of animdata data

	std::string project;
	std::map<std::string, datapack, alphanum_less> datalist;		// anim data set

	AnimSetDataProject() {}
	AnimSetDataProject(int& startline, vecstr& animdatafile, std::string filename, std::string projectname);
};

void animSetDataInitialize(std::string filepath, vecstr& catalyst, std::vector<std::shared_ptr<AnimSetDataProject>>& ADProject);

#endif
