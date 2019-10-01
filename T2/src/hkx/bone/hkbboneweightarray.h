#ifndef HKBBONEWEIGHTARRAY_H_
#define HKBBONEWEIGHTARRAY_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbvariablebindingset.h"

struct hkbboneweightarray : public hkbobject, std::enable_shared_from_this<hkbboneweightarray>
{
public:
	hkbboneweightarray() {}
	hkbboneweightarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	bool IsNegated = false;


	std::shared_ptr<hkbvariablebindingset> variableBindingSet;
	std::vector<double> boneWeights;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
};

void hkbBoneWeightArrayExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbboneweightarray>> hkbboneweightarrayList;
extern safeStringUMap<std::shared_ptr<hkbboneweightarray>> hkbboneweightarrayList_E;

#endif