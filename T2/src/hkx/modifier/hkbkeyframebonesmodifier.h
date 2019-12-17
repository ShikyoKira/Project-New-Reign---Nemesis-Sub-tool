#ifndef HKBKEYFRAMEBONESMODIFIER_H_
#define HKBKEYFRAMEBONESMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\bone\hkbboneindexarray.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbkeyframebonesmodifier : public hkbmodifier, std::enable_shared_from_this<hkbkeyframebonesmodifier>
{
public:
	hkbkeyframebonesmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct keyframeinfo
	{
		bool proxy;

		coordinate keyframedPosition;
		coordinate keyframedRotation;
		int boneIndex;
		bool isValid;

		keyframeinfo() : proxy(true) {}
		keyframeinfo(std::string line) : proxy(false) { keyframedPosition.update(line); }
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::vector<keyframeinfo> keyframeInfo;
	std::shared_ptr<hkbboneindexarray> keyframedBonesList;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<keyframeinfo>& ori, std::vector<keyframeinfo>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbkeyframebonesmodifier>> hkbkeyframebonesmodifierList;
extern safeStringUMap<std::shared_ptr<hkbkeyframebonesmodifier>> hkbkeyframebonesmodifierList_E;

#endif