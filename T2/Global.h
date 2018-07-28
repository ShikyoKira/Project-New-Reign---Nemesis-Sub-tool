#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <set>
#include <condition_variable>
#include <boost\regex.hpp>
#include <sys/stat.h>
#include "Global-Addon.h"
#include "Global-Type.h"
#include "functionwriter.h"

extern bool Error;
extern bool Debug;
extern std::set<int> idcount;
extern std::vector<std::string> originalfileline;
extern std::vector<std::string> editedfileline;
extern std::unordered_map<std::string, int> regioncount;		// region same name count
extern std::unordered_map<std::string, int> elements;			// elements in the function
extern std::unordered_map<std::string, bool> IsForeign;
extern std::unordered_map<std::string, bool> IsExist;			// check if function exist
extern std::unordered_map<std::string, bool> IsFileInUse;
extern std::unordered_map<std::string, bool> IsBranchOrigin;	// check if branch has original node
extern std::unordered_map<std::string, std::string> parent;
extern std::unordered_map<std::string, std::string> region;		// ID = region
extern std::unordered_map<std::string, std::string> newID;
extern std::unordered_map<std::string, std::string> eventID;
extern std::unordered_map<std::string, std::string> variableID;
extern std::unordered_map<std::string, std::string> attributeID;
extern std::unordered_map<std::string, std::string> characterID;
extern std::unordered_map<std::string, std::string> addressID;
extern std::unordered_map<std::string, std::string> exchangeID;
extern std::unordered_map<std::string, std::string> addressChange;
extern std::unordered_map<std::string, std::vector<std::string>> FunctionLineNew;
extern std::unordered_map<std::string, std::vector<std::string>> FunctionLineTemp;
extern std::unordered_map<std::string, std::vector<std::string>> FunctionLineOriginal;
extern std::unordered_map<std::string, std::vector<std::string>> FunctionLineEdited;
extern std::unordered_map<std::string, std::vector<std::string>> referencingIDs;
extern std::string modcode;
extern std::string targetfilename;
extern std::string targetfilenameedited;
extern std::string shortFileName;
extern std::string shortFileNameEdited;
extern int functioncount;

extern inline int sameWordCount(std::string line, std::string word);
extern inline bool IsFileExist(const std::string& name); // check if file exist
extern bool IsOldFunction(std::string filename, std::string id, std::string address); // initiate check if it is old or new node
extern inline std::string ExePath(); // get exe path
extern std::string CrossReferencing(std::string id, std::string address, int functionlayer, bool compare, bool special = false); // whether it is foreign principle or usual cross referencing
extern std::string conditionOldFunction(std::string id, std::string address, int functionlayer, std::string addline, bool isString); // expressCondition specific check whether it is new or old node
extern int FileLineCount(std::string filename); // get the total line count in a file
extern void RecordID(std::string id, std::string address, bool compare = false); // record id for IsExist node

#endif