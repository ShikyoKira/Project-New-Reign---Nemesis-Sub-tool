#ifndef HKBSTATEMACHINETRANSITIONINTOARRAY_H_
#define HKBSTATEMACHINETRANSITIONINTOARRAY_H_

#include <iostream>
#include <fstream>
#include <map>
#include "src\hkx\hkbobject.h"
#include "src\hkx\condition\hkbcondition.h"
#include "src\hkx\transition\hkbtransitioneffect.h"

struct hkbstatemachinetransitioninfoarray : public hkbobject, std::enable_shared_from_this<hkbstatemachinetransitioninfoarray>
{
public:
	hkbstatemachinetransitioninfoarray() {}
	hkbstatemachinetransitioninfoarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetTransition(int number);
	int GetTransitionCount();

	std::string GetCondition(int number);
	int GetConditionCount();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr transition;
	vecstr condition;
	bool IsNegated = false;

	struct transitioninfo
	{
		struct transitionflags
		{
			bool FLAG_USE_TRIGGER_INTERVAL = false;
			bool FLAG_USE_INITIATE_INTERVAL = false;
			bool FLAG_UNINTERRUPTIBLE_WHILE_PLAYING = false;
			bool FLAG_UNINTERRUPTIBLE_WHILE_DELAYED = false;
			bool FLAG_DELAY_STATE_CHANGE = false;
			bool FLAG_DISABLED = false;
			bool FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE = false;
			bool FLAG_DISALLOW_RANDOM_TRANSITION = false;
			bool FLAG_DISABLE_CONDITION = false;
			bool FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE = false;
			bool FLAG_IS_GLOBAL_WILDCARD = false;
			bool FLAG_IS_LOCAL_WILDCARD = false;
			bool FLAG_FROM_NESTED_STATE_ID_IS_VALID = false;
			bool FLAG_TO_NESTED_STATE_ID_IS_VALID = false;
			bool FLAG_ABUT_AT_END_OF_FROM_GENERATOR = false;

			bool operator==(transitionflags& ctrpart);
			bool operator!=(transitionflags& ctrpart);
			std::string getString();
			void update(std::string flag);
		};

		struct interval
		{
			EventId enterEventId;
			EventId exitEventId;
			double enterTime;
			double exitTime;

			interval() {}
			interval(int n_enterEventId) : enterEventId(n_enterEventId) {}
		};

		bool proxy;

		interval triggerInterval;
		interval initiateInterval;
		std::shared_ptr<hkbtransitioneffect> transition;
		std::shared_ptr<hkbcondition> condition;
		EventId eventId;
		int toStateId;
		int fromNestedStateId;
		int toNestedStateId;
		int priority;
		transitionflags flags;

		transitioninfo() : proxy(true) {}
		transitioninfo(int enterEventId) : triggerInterval(enterEventId), proxy(false) {}
	};

	std::vector<transitioninfo> transitions;
	std::vector<transitioninfo> de_transitions;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<transitioninfo>& ori, std::vector<transitioninfo>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbStateMachineTransitionInfoArrayExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbstatemachinetransitioninfoarray>> hkbstatemachinetransitioninfoarrayList;
extern safeStringUMap<std::shared_ptr<hkbstatemachinetransitioninfoarray>> hkbstatemachinetransitioninfoarrayList_E;

#endif