#include "hkbstatelistener.h"

using namespace std;

namespace statelistener
{
	string key = "cm";
}

safeStringUMap<shared_ptr<hkbstatelistener>> hkbstatelistenerList;
safeStringUMap<shared_ptr<hkbstatelistener>> hkbstatelistenerList_E;

void hkbstatelistener::regis(string id, bool isEdited)
{
	isEdited ? hkbstatelistenerList_E[id] = shared_from_this() : hkbstatelistenerList[id] = shared_from_this();
	ID = id;
}

string hkbstatelistener::getClassCode()
{
	return statelistener::key;
}
