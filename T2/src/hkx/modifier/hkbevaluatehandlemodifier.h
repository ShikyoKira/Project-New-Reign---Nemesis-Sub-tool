#ifndef HKBEVALUATEHANDLEMODIFIER_H_
#define HKBEVALUATEHANDLEMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbhandle.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbevaluatehandlemodifier : public hkbmodifier, std::enable_shared_from_this<hkbevaluatehandlemodifier>
{
public:
	hkbevaluatehandlemodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	enum handlechangemode
	{
		HANDLE_CHANGE_MODE_ABRUPT,
		HANDLE_CHANGE_MODE_CONSTANT_VELOCITY
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::shared_ptr<hkbhandle> handle;
	coordinate handlePositionOut;
	coordinate handleRotationOut;
	bool isValidOut;
	double extrapolationTimeStep;
	double handleChangeSpeed;
	handlechangemode handleChangeMode;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getHandleChangeMode();
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbevaluatehandlemodifier>> hkbevaluatehandlemodifierList;
extern safeStringUMap<std::shared_ptr<hkbevaluatehandlemodifier>> hkbevaluatehandlemodifierList_E;

#endif