#ifndef ANIMDATANMATCH_H_
#define ANIMDATAMATCH_H_

#include "Global.h"

typedef std::vector<std::string> vecstr;

bool matchProjectScoring(std::vector<std::shared_ptr<AnimDataProject>>& ori, std::vector<std::shared_ptr<AnimDataProject>>& edit, std::string filename);
bool matchProjectScoring(std::vector<AnimDataPack>& ori, std::vector<AnimDataPack>& edit, std::string filename);
bool matchProjectScoring(std::vector<InfoDataPack>& ori, std::vector<InfoDataPack>& edit, std::string filename);
bool matchProjectScoring(vecstr& ori, vecstr& edit, std::string filename);
bool matchDetailedScoring(vecstr& ori, vecstr& edit, std::string filename);

#endif