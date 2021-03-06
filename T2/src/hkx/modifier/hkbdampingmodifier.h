#ifndef HKBDAMPINGMODIFIER_H_
#define HKBDAMPINGMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbdampingmodifier : public hkbmodifier, std::enable_shared_from_this<hkbdampingmodifier>
{
public:
	hkbdampingmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	double kP;
	double kI;
	double kD;
	bool enableScalarDamping;
	bool enableVectorDamping;
	double rawValue;
	double dampedValue;
	coordinate rawVector;
	coordinate dampedVector;
	coordinate vecErrorSum;
	coordinate vecPreviousError;
	double errorSum;
	double previousError;

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

extern safeStringUMap<std::shared_ptr<hkbdampingmodifier>> hkbdampingmodifierList;
extern safeStringUMap<std::shared_ptr<hkbdampingmodifier>> hkbdampingmodifierList_E;

#endif