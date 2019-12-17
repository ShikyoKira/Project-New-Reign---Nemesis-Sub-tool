#include "hkbhandle.h"

using namespace std;

namespace handle_h
{
	const string key = "cs";
	const string classname = "hkbHandle";
}

safeStringUMap<shared_ptr<hkbhandle>> hkbhandleList;
safeStringUMap<shared_ptr<hkbhandle>> hkbhandleList_E;

safeStringUMap<shared_ptr<hklocalframe>> hklocalframeList;
safeStringUMap<shared_ptr<hklocalframe>> hklocalframeList_E;

safeStringUMap<shared_ptr<hkprigidbody>> hkprigidbodyList;
safeStringUMap<shared_ptr<hkprigidbody>> hkprigidbodyList_E;

safeStringUMap<shared_ptr<hkbcharacter>> hkbcharacterList;
safeStringUMap<shared_ptr<hkbcharacter>> hkbcharacterList_E;

void hkbhandle::regis(string id, bool isEdited)
{
	isEdited ? hkbhandleList_E[id] = shared_from_this() : hkbhandleList[id] = shared_from_this();
	ID = id;
}

void hkbhandle::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	for (auto& line : nodelines)
	{
		for (auto& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("<hkparam name=\"(.+?)\">(.*?)<\\/hkparam>"));
			itr != boost::sregex_iterator(); ++itr)
		{
			string header = itr->str(1);

			if (header == "frame") frame = isEdited ? hklocalframeList_E[itr->str(2)] : hklocalframeList[itr->str(2)];
			else if (header == "rigidBody") rigidBody = isEdited ? hkprigidbodyList_E[itr->str(2)] : hkprigidbodyList[itr->str(2)];
			else if (header == "character") character = isEdited ? hkbcharacterList_E[itr->str(2)] : hkbcharacterList[itr->str(2)];
			else if (header == "animationBoneIndex") animationBoneIndex = stoi(itr->str(2));

			break;
		}
	}

	if ((Debug) && (!Error))
	{
		cout << handle_h::classname + " (ID: " << ID << ") is complete!" << endl;
	}

}

string hkbhandle::getClassCode()
{
	return handle_h::key;
}
