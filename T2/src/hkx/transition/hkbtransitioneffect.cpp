#include "hkbtransitioneffect.h"

using namespace std;

safeStringUMap<shared_ptr<hkbtransitioneffect>> hkbtransitioneffectList;
safeStringUMap<shared_ptr<hkbtransitioneffect>> hkbtransitioneffectList_E;

string hkbtransitioneffect::getSelfTransitionMode()
{
	switch (selfTransitionMode)
	{
		case SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC: return "SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC";
		case SELF_TRANSITION_MODE_CONTINUE: return "SELF_TRANSITION_MODE_CONTINUE";
		case SELF_TRANSITION_MODE_RESET: return "SELF_TRANSITION_MODE_RESET";
		case SELF_TRANSITION_MODE_BLEND: return "SELF_TRANSITION_MODE_BLEND";
		default: return "SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC";
	}
}

string hkbtransitioneffect::getEventMode()
{
	switch (eventMode)
	{
		case EVENT_MODE_DEFAULT: return "EVENT_MODE_DEFAULT";
		case EVENT_MODE_PROCESS_ALL: return "EVENT_MODE_PROCESS_ALL";
		case EVENT_MODE_IGNORE_FROM_GENERATOR: return "EVENT_MODE_IGNORE_FROM_GENERATOR";
		case EVENT_MODE_IGNORE_TO_GENERATOR: return "EVENT_MODE_IGNORE_TO_GENERATOR";
		default: return "EVENT_MODE_DEFAULT";
	}
}
