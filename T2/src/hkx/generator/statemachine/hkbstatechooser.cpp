#include "hkbstatechooser.h"

using namespace std;

namespace statechooser
{
	const string key = "cl";
}

safeStringUMap<shared_ptr<hkbstatechooser>> hkbstatechooserList;
safeStringUMap<shared_ptr<hkbstatechooser>> hkbstatechooserList_E;

void hkbstatechooser::regis(string id, bool isEdited)
{
	isEdited ? hkbstatechooserList_E[id] = shared_from_this() : hkbstatechooserList[id] = shared_from_this();
	ID = id;
}

string hkbstatechooser::getClassCode()
{
	return statechooser::key;
}
