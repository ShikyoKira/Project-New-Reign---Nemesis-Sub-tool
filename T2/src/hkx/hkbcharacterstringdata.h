#ifndef HKBCHARACTERSTRINGDATA_H_
#define HKBCHARACTERSTRINGDATA_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\utilities\variableinfopack.h"

struct hkbcharacterstringdata : public hkbobject, std::enable_shared_from_this<hkbcharacterstringdata>
{
public:
	hkbcharacterstringdata() {}
	hkbcharacterstringdata(std::string filepath, std::string id, std::string preaddress, int functionLayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	bool IsNegated = false;


	vecstr deformableSkinNames;
	vecstr rigidSkinNames;
	vecstr animationNames;
	vecstr animationFilenames;
	std::shared_ptr<variableinfopack> characterPropertyNames;
	vecstr retargetingSkeletonMapperFilenames;
	vecstr lodNames;
	vecstr mirroredSyncPointSubstringsA;
	vecstr mirroredSyncPointSubstringsB;
	std::string name;
	std::string rigName;
	std::string ragdollName;
	std::string behaviorFilename;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	void matchScoring(vecstr& ori, vecstr& edit, std::string id);
	void fillScore(vecstr& ori, vecstr& edit, usize start, usize cap, std::map<int, std::map<int, double>>& scorelist, std::set<std::string> newname, bool dup,
		std::set<std::string> dupname);
	void stringEnter(vecstr& output, vecstr& namelist, vecstr& ctrpart_namelist, std::string listname, usize& base, bool& isEdited, bool caseSensitive = true);
	void stringEnterNew(vecstr& output, vecstr namelist, std::string listname, usize& base);
	bool dataNameListEnter(std::string line, vecstr& input);
};

void hkbCharacterStringDataExport(std::string id);
inline void process(vecstr storeline1, vecstr storeline2, int curline, int i, bool& IsChanged, int& openpoint, bool& open, bool& IsEdited, vecstr& output);
inline void postProcess(std::string elementName, vecstr storeline1, vecstr storeline2, int curline, int i, bool& IsChanged, int openpoint, bool& open, bool& IsEdited, vecstr& output);

extern safeStringUMap<std::shared_ptr<hkbcharacterstringdata>> hkbcharacterstringdataList;
extern safeStringUMap<std::shared_ptr<hkbcharacterstringdata>> hkbcharacterstringdataList_E;

#endif