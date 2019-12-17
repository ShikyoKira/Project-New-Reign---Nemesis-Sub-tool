#ifndef HKBHANDIKCONTROLSMODIFIER_H_
#define HKBHANDIKCONTROLSMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbhandle.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbhandle;

struct hkbhandikcontrolsmodifier : public hkbmodifier, std::enable_shared_from_this<hkbhandikcontrolsmodifier>
{
public:
	hkbhandikcontrolsmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct hkhand
	{
		struct controldata
		{
			enum handlechangemode
			{
				HANDLE_CHANGE_MODE_ABRUPT,
				HANDLE_CHANGE_MODE_CONSTANT_VELOCITY
			};

			coordinate targetPosition;
			coordinate targetRotation;
			coordinate targetNormal;
			std::shared_ptr<hkbhandle> targetHandle;
			double transformOnFraction;
			double normalOnFraction;
			double fadeInDuration;
			double fadeOutDuration;
			double extrapolationTimeStep;
			double handleChangeSpeed;
			handlechangemode handleChangeMode;
			bool fixUp;

			std::string getHandleChangeMode();
		};

		bool proxy;

		controldata controlData;
		int handIndex;
		bool enable;

		hkhand() : proxy(true) {}
		hkhand(std::string line) : proxy(false) { controlData.targetPosition.update(line); }
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	std::vector<hkhand> hands;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<hkhand>& ori, std::vector<hkhand>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbhandikcontrolsmodifier>> hkbhandikcontrolsmodifierList;
extern safeStringUMap<std::shared_ptr<hkbhandikcontrolsmodifier>> hkbhandikcontrolsmodifierList_E;

#endif