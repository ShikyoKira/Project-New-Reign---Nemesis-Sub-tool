#ifndef HKBEVENTDRIVENMODIFIER_H_
#define HKBEVENTDRIVENMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbeventdrivenmodifier : public hkbmodifier, std::enable_shared_from_this<hkbeventdrivenmodifier>
{
public:
	hkbeventdrivenmodifier() {}
	hkbeventdrivenmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetModifier();
	
	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	std::string s_modifier;
	bool IsNegated = false;


	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::shared_ptr<hkbmodifier> modifier;
	EventId activateEventId;
	EventId deactivateEventId;
	bool activeByDefault;

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

void hkbEventDrivenModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbeventdrivenmodifier>> hkbeventdrivenmodifierList;
extern safeStringUMap<std::shared_ptr<hkbeventdrivenmodifier>> hkbeventdrivenmodifierList_E;

#endif