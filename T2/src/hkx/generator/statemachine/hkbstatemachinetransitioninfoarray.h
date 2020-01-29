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

	std::string GetAddress();

	std::string tempaddress;

	struct transitioninfo
	{
		struct transitionflags
		{
			enum flags
			{
				FLAG_NONE = 0,

				FLAG_USE_TRIGGER_INTERVAL = 1,
				FLAG_USE_INITIATE_INTERVAL = 2,
				FLAG_UNINTERRUPTIBLE_WHILE_PLAYING = 4,
				FLAG_UNINTERRUPTIBLE_WHILE_DELAYED = 8,
				FLAG_DELAY_STATE_CHANGE = 16,
				FLAG_DISABLED = 32,
				FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE = 64,
				FLAG_DISALLOW_RANDOM_TRANSITION = 128,
				FLAG_DISABLE_CONDITION = 256,
				FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE = 512,
				FLAG_IS_GLOBAL_WILDCARD = 1024,
				FLAG_IS_LOCAL_WILDCARD = 2048,
				FLAG_FROM_NESTED_STATE_ID_IS_VALID = 4096,
				FLAG_TO_NESTED_STATE_ID_IS_VALID = 8192,

				FLAG_ABUT_AT_END_OF_FROM_GENERATOR = 16384,
			};

			flags data = FLAG_NONE;

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

extern safeStringUMap<std::shared_ptr<hkbstatemachinetransitioninfoarray>> hkbstatemachinetransitioninfoarrayList;
extern safeStringUMap<std::shared_ptr<hkbstatemachinetransitioninfoarray>> hkbstatemachinetransitioninfoarrayList_E;

#endif