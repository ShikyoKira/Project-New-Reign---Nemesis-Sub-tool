#ifndef HKBGENERATORTRANSITIONEFFECT_H_
#define HKBGENERATORTRANSITIONEFFECT_H_

#include "src\hkx\transition\hkbtransitioneffect.h"

struct hkbgeneratortransitioneffect : public hkbtransitioneffect, std::enable_shared_from_this<hkbgeneratortransitioneffect>
{
public:
	hkbgeneratortransitioneffect() {}

	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	// selfTransitionMode from hkbtransitioneffect;
	// eventMode from hkbtransitioneffect;
	std::shared_ptr<hkbgenerator> transitionGenerator;
	double blendInDuration;
	double blendOutDuration;
	bool syncToGeneratorStartTime;
	// hkRefVariant fromGenerator;
	// hkRefVariant toGenerator;
	double timeInTransition;
	double duration;
	double effectiveBlendInDuration;
	double effectiveBlendOutDuration;
	int toGeneratorState;
	bool echoTransitionGenerator;
	bool echoToGenerator;
	bool justActivated;
	bool updateActiveNodes;
	int stage;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbgeneratortransitioneffect>> hkbgeneratortransitioneffectList_E;
extern safeStringUMap<std::shared_ptr<hkbgeneratortransitioneffect>> hkbgeneratortransitioneffectList;

#endif
