#ifndef BSEVENTONFALSETOTRUEMODIFIER_H_
#define BSEVENTONFALSETOTRUEMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bseventonfalsetotruemodifier : public hkbmodifier, std::enable_shared_from_this<bseventonfalsetotruemodifier>
{
public:
	bseventonfalsetotruemodifier() {}

	std::string GetAddress();

	std::string tempaddress;
		
	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier

	bool bEnableEvent1;
	bool bVariableToTest1;
	eventproperty EventToSend1;

	bool bEnableEvent2;
	bool bVariableToTest2;
	eventproperty EventToSend2;

	bool bEnableEvent3;
	bool bVariableToTest3;
	eventproperty EventToSend3;

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

extern safeStringUMap<std::shared_ptr<bseventonfalsetotruemodifier>> bseventonfalsetotruemodifierList;
extern safeStringUMap<std::shared_ptr<bseventonfalsetotruemodifier>> bseventonfalsetotruemodifierList_E;

#endif