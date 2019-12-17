#ifndef HKBBLENDINGTRANSITIONEFFECT_H_
#define HKBBLENDINGTRANSITIONEFFECT_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\blendcurve.h"
#include "src\hkx\hkbvariablebindingset.h"
#include "src\hkx\transition\hkbtransitioneffect.h"

struct hkbblendingtransitioneffect : public hkbtransitioneffect, std::enable_shared_from_this<hkbblendingtransitioneffect>
{
public:
	hkbblendingtransitioneffect() {}

	std::string GetAddress();

	std::string tempaddress;

	struct flagbits
	{
		bool FLAG_NONE = false;
		bool FLAG_IGNORE_FROM_WORLD_FROM_MODEL = false;
		bool FLAG_SYNC = false;
		bool FLAG_IGNORE_TO_WORLD_FROM_MODEL = false;

		std::string getflags();
		void update(std::string flag);
	};

	enum endmode
	{
		END_MODE_NONE,
		END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR,
		END_MODE_CAP_DURATION_AT_END_OF_FROM_GENERATOR,
	};
	
	// variableBindingSet from hkbgenerator
	// userData from hkbgenerator
	// name from hkbgenerator
	// selfTransitionMode from hkbtransitioneffect;
	// eventMode from hkbtransitioneffect;
	double duration;
	double toGeneratorStartTimeFraction;
	flagbits flags;
	endmode endMode;
	blendcurve blendCurve;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getEndMode();
};

extern safeStringUMap<std::shared_ptr<hkbblendingtransitioneffect>> hkbblendingtransitioneffectList;
extern safeStringUMap<std::shared_ptr<hkbblendingtransitioneffect>> hkbblendingtransitioneffectList_E;

#endif