#ifndef ANIMSETDATAMATCH_H_
#define ANIMSETDATAMATCH_H_

#include "Global.h"
#include "alphanum.hpp"

bool matchProjectScoring(std::vector<std::shared_ptr<AnimSetDataProject>>& ori, std::vector<std::shared_ptr<AnimSetDataProject>>& edit, std::string filename);
bool matchDataScoring(std::map<std::string, datapack, alphanum_less>& ori, std::map<std::string, datapack, alphanum_less>& edit, std::string filename);

bool matchNameScoring(std::vector<equip>& ori, std::vector<equip>& edit, std::string filename);
bool matchNameScoring(std::vector<typepack>& ori, std::vector<typepack>& edit, std::string filename);
bool matchNameScoring(std::vector<animpack>& ori, std::vector<animpack>& edit, std::string filename);

bool matchAtkScoring(std::vector<attackdata>& ori, std::vector<attackdata>& edit, std::string filename);
bool matchCRC32Scoring(std::vector<crc32>& ori, std::vector<crc32>& edit, std::string filename);

#endif