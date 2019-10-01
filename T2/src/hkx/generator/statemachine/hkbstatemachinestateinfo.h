#ifndef HKBSTATEMACHINESTATEINFO_H_
#define HKBSTATEMACHINESTATEINFO_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\generator\statemachine\hkbstatelistener.h"
#include "src\hkx\generator\statemachine\hkbstatemachinetransitioninfoarray.h"
#include "src\hkx\generator\statemachine\hkbstatemachineeventpropertyarray.h"

struct hkbstatemachinestateinfo : public hkbobject, std::enable_shared_from_this<hkbstatemachinestateinfo>
{
public:
	hkbstatemachinestateinfo() {}
	hkbstatemachinestateinfo(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();

	std::string GetVariableBindingSet();
	bool IsBindingNull();
	std::string GetTransitions();
	bool IsTransitionsNull();
	std::string GetEnterNotifyEvent();
	bool IsEnterNotifyEventNull();
	std::string GetExitNotifyEvent();
	bool IsExitNotifyEventNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string s_generator;
	std::string variablebindingset;
	std::string s_transitions;
	std::string enternotifyevent;
	std::string exitnotifyevent;
	bool IsNegated = false;


	std::shared_ptr<hkbvariablebindingset> variableBindingSet;
	std::vector<std::shared_ptr<hkbstatelistener>> listeners;
	std::shared_ptr<hkbstatemachineeventpropertyarray> enterNotifyEvents;
	std::shared_ptr<hkbstatemachineeventpropertyarray> exitNotifyEvents;
	std::shared_ptr<hkbstatemachinetransitioninfoarray> transitions;
	std::shared_ptr<hkbgenerator> generator;
	std::string name;
	int stateId;
	double probability;
	bool enable;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<std::shared_ptr<hkbstatelistener>>& ori, std::vector<std::shared_ptr<hkbstatelistener>>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbStateMachineStateInfoExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbstatemachinestateinfo>> hkbstatemachinestateinfoList;
extern safeStringUMap<std::shared_ptr<hkbstatemachinestateinfo>> hkbstatemachinestateinfoList_E;

#endif