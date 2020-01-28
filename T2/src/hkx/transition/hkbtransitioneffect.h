#ifndef HKBTRANSITIONEFFECT_H_
#define HKBTRANSITIONEFFECT_H_

#include "src/hkx/generator/hkbgenerator.h"
#include "src/utilities/hkMap.h"

struct hkbtransitioneffect : public hkbgenerator
{
	hkbtransitioneffect() { isTransition = false; }

	enum selftransitionmode
	{
		SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC,
		SELF_TRANSITION_MODE_CONTINUE,
		SELF_TRANSITION_MODE_RESET,
		SELF_TRANSITION_MODE_BLEND
	};

	enum eventmode
	{
		EVENT_MODE_DEFAULT,
		EVENT_MODE_PROCESS_ALL,
		EVENT_MODE_IGNORE_FROM_GENERATOR,
		EVENT_MODE_IGNORE_TO_GENERATOR
	};

	selftransitionmode selfTransitionMode;
	eventmode eventMode;

protected:
	std::string getSelfTransitionMode();
	std::string getEventMode();
};

extern safeStringUMap<std::shared_ptr<hkbtransitioneffect>> hkbtransitioneffectList_E;
extern safeStringUMap<std::shared_ptr<hkbtransitioneffect>> hkbtransitioneffectList;

extern hkMap<std::string, hkbtransitioneffect::selftransitionmode> selfTransitionMap;
extern hkMap<std::string, hkbtransitioneffect::eventmode> eventMap;

#endif
