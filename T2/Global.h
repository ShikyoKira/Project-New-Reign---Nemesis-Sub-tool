#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <set>
#include <windows.h>
#include <condition_variable>
#include <boost\regex.hpp>
#include <sys/stat.h>
#include "Global-Addon.h"
#include "functionwriter.h"

struct AnimDataProject;
struct AnimSetDataProject;
struct hkbobject;

typedef std::unordered_map<std::string, std::string> mapstring;

extern bool Error;
extern bool Debug;
extern nodelist FunctionLineNew;
extern nodelist FunctionLineTemp;
extern nodelist FunctionLineOriginal;
extern nodelist FunctionLineEdited;
extern hkRefPtr originalBehavior;
extern hkRefPtr editedBehavior;
extern std::set<int> idcount;
extern safeStringUMap<bool> IsExist;			// check if function exist
extern vecstr originalfileline;
extern vecstr editedfileline;
extern std::map<std::string, std::string> addressID;
extern std::unordered_map<std::string, int> elements;			// elements in the function
extern std::unordered_map<std::string, bool> IsForeign;
extern std::unordered_map<std::string, bool> IsFileInUse;
extern std::unordered_map<std::string, bool> IsBranchOrigin;	// check if branch has original node
extern mapstring parent;
extern mapstring exchangeID;
extern mapstring addressChange;
extern std::vector<mapstring> eventID;
extern std::vector<mapstring> variableID;
extern std::vector<mapstring> attributeID;
extern std::vector<mapstring> characterID;
extern std::unordered_map<std::string, vecstr> referencingIDs;
extern std::unordered_map<std::shared_ptr<hkbobject>, std::shared_ptr<hkbobject>> hkb_parent;
extern std::vector<std::shared_ptr<AnimDataProject>> AnimDataOriginal;			// AnimData data
extern std::vector<std::shared_ptr<AnimDataProject>> AnimDataEdited;			// Edited AnimData data
extern std::vector<std::shared_ptr<AnimSetDataProject>> AnimSetDataOriginal;		// AnimSetData data
extern std::vector<std::shared_ptr<AnimSetDataProject>> AnimSetDataEdited;			// Edited AnimSetData data
extern std::string modcode;
extern std::string targetfilename;
extern std::string targetfilenameedited;
extern std::string shortFileName;
extern std::string shortFileNameEdited;
extern int functioncount;
extern std::vector<unsigned int> eventCount;
extern std::vector<unsigned int> varCount;

extern inline int sameWordCount(std::string line, std::string word);
extern inline bool IsFileExist(const std::string& name); // check if file exist
bool IsOldFunction(std::string filename, std::shared_ptr<hkbobject> id, std::string address); // initiate check if it is old or new node
bool IsOldFunction(std::string filename, std::string id, std::string address); // initiate check if it is old or new node
extern inline std::string ExePath(); // get exe path
std::string CrossReferencing(std::string id, std::string address, int functionlayer, bool compare, bool special = false); // whether it is foreign principle or usual cross referencing
std::string CrossReferencing(std::shared_ptr<hkbobject> hkb_object, std::string address, int functionlayer, bool compare, bool special = false); // whether it is foreign principle or usual cross referencing
std::string conditionOldFunction(std::string id, std::string address, int functionlayer, std::string addline, bool isString); // expressCondition specific check whether it is new or old node
std::string conditionOldFunction(std::shared_ptr<hkbobject> hkb_object, std::string address, int functionlayer, std::string addline, bool isString); // expressCondition specific check whether it is new or old node
int FileLineCount(std::string filename); // get the total line count in a file
void RecordID(std::string id, std::string address, bool compare = false); // record id for IsExist node

#endif