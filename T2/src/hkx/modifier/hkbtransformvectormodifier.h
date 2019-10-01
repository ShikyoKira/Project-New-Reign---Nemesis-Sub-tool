#ifndef HKBTRANSFORMVECTORMODIFIER_H_
#define HKBTRANSFORMVECTORMODIFIER_H_

#include <iostream>
#include <fstream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbtransformvectormodifier : public hkbmodifier, std::enable_shared_from_this<hkbtransformvectormodifier>
{
public:
	hkbtransformvectormodifier() {}
	hkbtransformvectormodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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


	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	coordinate rotation;
	coordinate translation;
	coordinate vectorIn;
	coordinate vectorOut;
	bool rotateOnly;
	bool inverse;
	bool computeOnActivate;
	bool computeOnModify;

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

void hkbTransformVectorModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbtransformvectormodifier>> hkbtransformvectormodifierList;
extern safeStringUMap<std::shared_ptr<hkbtransformvectormodifier>> hkbtransformvectormodifierList_E;

#endif