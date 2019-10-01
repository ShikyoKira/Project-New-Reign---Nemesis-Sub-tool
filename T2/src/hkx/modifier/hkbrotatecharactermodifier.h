#ifndef HKBROTATECHARACTERMODIFIER_H_
#define HKBROTATECHARACTERMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbrotatecharactermodifier : public hkbmodifier, std::enable_shared_from_this<hkbrotatecharactermodifier>
{
public:
	hkbrotatecharactermodifier() {}
	hkbrotatecharactermodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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
	double degreesPerSecond;
	double speedMultiplier;
	coordinate axisOfRotation;

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

void hkbRotateCharacterModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbrotatecharactermodifier>> hkbrotatecharactermodifierList;
extern safeStringUMap<std::shared_ptr<hkbrotatecharactermodifier>> hkbrotatecharactermodifierList_E;

#endif