#ifndef HKBVARIABLEVALUESET_H_
#define HKBVARIABLEVALUESET_H_

#include <iostream>
#include <fstream>
#include <cstdio>
#include "src\hkx\hkbobject.h"
#include "src\utilities\coordinate.h"
#include "src\utilities\variableinfopack.h"

struct hkreferencedobject : public hkbobject, std::enable_shared_from_this<hkreferencedobject>
{
	void regis(std::string id, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot) {}
	std::string getClassCode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
};

extern safeStringUMap<std::shared_ptr<hkreferencedobject>> hkreferencedobjectList;
extern safeStringUMap<std::shared_ptr<hkreferencedobject>> hkreferencedobjectList_E;

struct hkbvariablevalueset : public hkbobject, std::enable_shared_from_this<hkbvariablevalueset>
{
public:
	hkbvariablevalueset() {}
	hkbvariablevalueset(std::string filepath, std::string id, std::string preaddress, int functionLayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	int GetBoneCount();
	std::string GetBoneArray(int order);

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr boneArray;
	int boneCount = 0;
	bool IsNegated = false;


	std::shared_ptr<variableinfopack> wordVariableValues;
	std::vector<coordinate> quadVariableValues;
	std::vector<std::shared_ptr<hkbobject>> variantVariableValues;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbVariableValueSetExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbvariablevalueset>> hkbvariablevaluesetList;
extern safeStringUMap<std::shared_ptr<hkbvariablevalueset>> hkbvariablevaluesetList_E;

#endif