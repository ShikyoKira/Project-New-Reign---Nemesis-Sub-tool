#ifndef HKBATTRIBUTEMODIFIER_H_
#define HKBATTRIBUTEMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbattributemodifier : public hkbmodifier, std::enable_shared_from_this<hkbattributemodifier>
{
public:
	hkbattributemodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct assignmentclass
	{
		bool proxy;

		int attributeIndex;
		double attributeValue;

		assignmentclass() : proxy(true) {}
		assignmentclass(int n_attributeIndex) : attributeIndex(n_attributeIndex), proxy(false) {}
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::vector<assignmentclass> assignments;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<assignmentclass>& ori, std::vector<assignmentclass>& edit, std::string id);
};

extern safeStringUMap<std::shared_ptr<hkbattributemodifier>> hkbattributemodifierList;
extern safeStringUMap<std::shared_ptr<hkbattributemodifier>> hkbattributemodifierList_E;

#endif