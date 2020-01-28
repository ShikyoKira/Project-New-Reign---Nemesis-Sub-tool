#include "hkbtransitioneffect.h"

using namespace std;

safeStringUMap<shared_ptr<hkbtransitioneffect>> hkbtransitioneffectList;
safeStringUMap<shared_ptr<hkbtransitioneffect>> hkbtransitioneffectList_E;

hkMap<string, hkbtransitioneffect::selftransitionmode> selfTransitionMap =
{
	{ "SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC", hkbtransitioneffect::SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC },
	{ "SELF_TRANSITION_MODE_CONTINUE", hkbtransitioneffect::SELF_TRANSITION_MODE_CONTINUE },
	{ "SELF_TRANSITION_MODE_RESET", hkbtransitioneffect::SELF_TRANSITION_MODE_RESET },
	{ "SELF_TRANSITION_MODE_BLEND", hkbtransitioneffect::SELF_TRANSITION_MODE_BLEND },
};

hkMap<string, hkbtransitioneffect::eventmode> eventMap =
{
	{ "EVENT_MODE_DEFAULT", hkbtransitioneffect::EVENT_MODE_DEFAULT },
	{ "EVENT_MODE_PROCESS_ALL", hkbtransitioneffect::EVENT_MODE_PROCESS_ALL },
	{ "EVENT_MODE_IGNORE_FROM_GENERATOR", hkbtransitioneffect::EVENT_MODE_IGNORE_FROM_GENERATOR },
	{ "EVENT_MODE_IGNORE_TO_GENERATOR", hkbtransitioneffect::EVENT_MODE_IGNORE_TO_GENERATOR },
};

string hkbtransitioneffect::getSelfTransitionMode()
{
	return selfTransitionMap[selfTransitionMode];
}

string hkbtransitioneffect::getEventMode()
{
	return eventMap[eventMode];
}
