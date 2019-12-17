#ifndef HKBSTRINGCONDITION_H_
#define HKBSTRINGCONDITION_H_

#include <iostream>
#include <fstream>
#include "src\hkx\condition\hkbcondition.h"

struct hkbstringcondition : public hkbcondition, std::enable_shared_from_this<hkbstringcondition>
{
public:
	hkbstringcondition() {}

	std::string GetAddress();

	std::string tempaddress;

	std::string conditionString;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
};

extern safeStringUMap<std::shared_ptr<hkbstringcondition>> hkbstringconditionList;
extern safeStringUMap<std::shared_ptr<hkbstringcondition>> hkbstringconditionList_E;

#endif