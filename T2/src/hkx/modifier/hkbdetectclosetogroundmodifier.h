#ifndef HKBDETECTCLOSETOGROUNDMODIFIER_H_
#define HKBDETECTCLOSETOGROUNDMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbdetectclosetogroundmodifier : public hkbmodifier, std::enable_shared_from_this<hkbdetectclosetogroundmodifier>
{
public:
	hkbdetectclosetogroundmodifier() {}
	hkbdetectclosetogroundmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetPayload();
	bool IsPayloadNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string payload;
	std::string variablebindingset;
	bool IsNegated = false;

	
	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	eventproperty closeToGroundEvent;
	double closeToGroundHeight;
	double raycastDistanceDown;
	int collisionFilterInfo;
	int boneIndex;
	int animBoneIndex;

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

void hkbDetectCloseToGroundModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbdetectclosetogroundmodifier>> hkbdetectclosetogroundmodifierList;
extern safeStringUMap<std::shared_ptr<hkbdetectclosetogroundmodifier>> hkbdetectclosetogroundmodifierList_E;

#endif