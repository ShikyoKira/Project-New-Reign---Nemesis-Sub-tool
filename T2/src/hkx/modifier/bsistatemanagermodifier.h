#ifndef BSISTATEMANAGERMODIFIER_H_
#define BSISTATEMANAGERMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"
#include "src\hkx\generator\hkbgenerator.h"

struct bsistatemanagermodifier : public hkbmodifier, std::enable_shared_from_this<bsistatemanagermodifier>
{
public:
	bsistatemanagermodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct statedata
	{
		bool proxy;

		std::shared_ptr<hkbgenerator> pStateMachine;
		int StateID;
		int iStateToSetAs;

		statedata() : proxy(true) {}
		statedata(std::shared_ptr<hkbgenerator> generator) : pStateMachine(generator), proxy(false) {}
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	int iStateVar;
	std::vector<statedata> stateData;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<statedata>& ori, std::vector<statedata>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<bsistatemanagermodifier>> bsistatemanagermodifierList;
extern safeStringUMap<std::shared_ptr<bsistatemanagermodifier>> bsistatemanagermodifierList_E;

#endif