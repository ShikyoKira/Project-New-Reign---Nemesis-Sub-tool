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
	bseventonfalsetotruemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	int GetPayloadCount();
	std::string GetPayload(int child);
	bool IsPayloadNull(int child);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr payload;
	std::string variablebindingset;
	bool IsNegated = false;

		
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

void BSEventOnFalseToTrueModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<bseventonfalsetotruemodifier>> bseventonfalsetotruemodifierList;
extern safeStringUMap<std::shared_ptr<bseventonfalsetotruemodifier>> bseventonfalsetotruemodifierList_E;

#endif