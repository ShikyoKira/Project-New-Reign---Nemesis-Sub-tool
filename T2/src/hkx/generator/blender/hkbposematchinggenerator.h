#ifndef HKBPOSEMATCHINGGENERATOR_H_
#define HKBPOSEMATCHINGGENERATOR_H_

#include <fstream>
#include <iostream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\blender\hkbblendergenerator.h"

struct hkbposematchinggenerator : public blenderBase, std::enable_shared_from_this<hkbposematchinggenerator>
{
public:
	hkbposematchinggenerator() {}
	hkbposematchinggenerator(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator(int child);
	int GetChildren();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetClass(std::string id, std::string inputfile);

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string s_name;
	vecstr generator;
	std::string variablebindingset;
	int i_children;
	bool IsNegated = false;


	enum mode
	{
		MODE_MATCH,
		MODE_PLAY
	};
	
	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	// referencePoseWeightThreshold from blenderBase
	// blendParameter from blenderBase
	// minCyclicBlendParameter from blenderBase
	// maxCyclicBlendParameter from blenderBase
	// indexOfSyncMasterChild from blenderBase
	// flags from blenderBase
	// subtractLastChild from blenderBase
	// children from blenderBase
	coordinate worldFromModelRotation;
	double blendSpeed;
	double minSpeedToSwitch;
	double minSwitchTimeNoError;
	double minSwitchTimeFullError;
	int startPlayingEventId;
	int startMatchingEventId;
	int rootBoneIndex;
	int otherBoneIndex;
	int anotherBoneIndex;
	int pelvisIndex;
	mode mode;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getMode();
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbPoseMatchingGeneratorExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbposematchinggenerator>> hkbposematchinggeneratorList;
extern safeStringUMap<std::shared_ptr<hkbposematchinggenerator>> hkbposematchinggeneratorList_E;

#endif