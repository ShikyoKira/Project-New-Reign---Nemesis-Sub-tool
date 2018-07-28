#ifndef GLOBALADDON_H_
#define GLOBALADDON_H_

#include <string>
#include <vector>
#include <atomic>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <boost\date_time\posix_time\posix_time.hpp>

extern void ReferenceReplacementExt(std::string wrongReference, std::string rightReference); // replacement function for foreign principle
extern void ReferenceReplacement(std::string wrongReference, std::string rightReference, bool reserve = false);
extern void NemesisReaderFormat(std::vector<std::string>& output, bool hasID = false);
extern void FolderCreate(std::string curBehaviorPath);
extern void GetFunctionLines(std::string filename, std::vector<std::string>& storeline);
extern std::vector<std::string> GetElements(std::string number, std::unordered_map<std::string, std::vector<std::string>>& functionlines, bool isTransition = false, std::string key = "");
extern bool isOnlyNumber(std::string line);
extern bool hasAlpha(std::string line);

#endif