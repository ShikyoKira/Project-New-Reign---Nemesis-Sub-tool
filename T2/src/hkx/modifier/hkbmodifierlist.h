#ifndef HKBMODIFIERLIST_H_
#define HKBMODIFIERLIST_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbmodifierlist : public hkbmodifier, std::enable_shared_from_this<hkbmodifierlist>
{
public:
	hkbmodifierlist() {}
	hkbmodifierlist(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetModifier(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr modifier;
	std::string variablebindingset;
	int children;
	bool IsNegated = false;


	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::vector<std::shared_ptr<hkbmodifier>> modifiers;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<std::shared_ptr<hkbmodifier>>& ori, std::vector<std::shared_ptr<hkbmodifier>>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbModifierListExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbmodifierlist>> hkbmodifierlistList;
extern safeStringUMap<std::shared_ptr<hkbmodifierlist>> hkbmodifierlistList_E;

#endif