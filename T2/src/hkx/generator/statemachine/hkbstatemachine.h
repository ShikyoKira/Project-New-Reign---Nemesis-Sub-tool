#ifndef HKBSTATEMACHINE_H_
#define HKBSTATEMACHINE_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\generator\statemachine\hkbstatechooser.h"
#include "src\hkx\generator\statemachine\hkbstatemachinestateinfo.h"
#include "src\hkx\generator\statemachine\hkbstatemachinetransitioninfoarray.h"

struct hkbstatemachine : public hkbgenerator, std::enable_shared_from_this<hkbstatemachine>
{
public:
	hkbstatemachine() {}

	std::string GetAddress();

	std::string tempaddress;

	enum startstatemode
	{
		START_STATE_MODE_DEFAULT,
		START_STATE_MODE_SYNC,
		START_STATE_MODE_RANDOM,
		START_STATE_MODE_CHOOSER
	};

	enum statemachineselftransitionmode
	{
		SELF_TRANSITION_MODE_NO_TRANSITION,
		SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE,
		SELF_TRANSITION_MODE_FORCE_TRANSITION_TO_START_STATE
	};

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	eventproperty eventToSendWhenStateOrTransitionChanges;
	std::shared_ptr<hkbstatechooser> startStateChooser;
	int startStateId;
	int returnToPreviousStateEventId;
	int randomTransitionEventId;
	int transitionToNextHigherStateEventId;
	int transitionToNextLowerStateEventId;
	int syncVariableIndex;
	bool wrapAroundStateId;
	int maxSimultaneousTransitions;
	startstatemode startStateMode;
	statemachineselftransitionmode selfTransitionMode;
	std::vector<std::shared_ptr<hkbstatemachinestateinfo>> states;
	std::shared_ptr<hkbstatemachinetransitioninfoarray> wildcardTransitions;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getStartStateMode();
	std::string getSelfTransitionMode();
	void matchScoring(std::vector<std::shared_ptr<hkbstatemachinestateinfo>>& ori, std::vector<std::shared_ptr<hkbstatemachinestateinfo>>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbstatemachine>> hkbstatemachineList;
extern safeStringUMap<std::shared_ptr<hkbstatemachine>> hkbstatemachineList_E;

#endif