#ifndef HKBEXPRESSIONCONDITION_H_
#define HKBEXPRESSIONCONDITION_H_

#include <iostream>
#include <fstream>
#include "src\hkx\condition\hkbcondition.h"

struct hkbexpressioncondition : public hkbcondition, std::enable_shared_from_this<hkbexpressioncondition>
{
public:
	hkbexpressioncondition() {}

	std::string GetAddress();

	std::string tempaddress;

	std::string expression;

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

extern safeStringUMap<std::shared_ptr<hkbexpressioncondition>> hkbexpressionconditionList;
extern safeStringUMap<std::shared_ptr<hkbexpressioncondition>> hkbexpressionconditionList_E;

#endif