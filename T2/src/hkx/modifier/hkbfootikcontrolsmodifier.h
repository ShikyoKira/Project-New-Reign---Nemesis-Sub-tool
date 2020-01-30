#ifndef HKBFOOTIKCONTROLSMODIFIER_H_
#define HKBFOOTIKCONTROLSMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"
#include "src\hkx\modifier\base\hkbfootikgains.h"

struct hkbfootikcontrolsmodifier : public hkbmodifier, std::enable_shared_from_this<hkbfootikcontrolsmodifier>
{
public:
	hkbfootikcontrolsmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	struct hkleg
	{
		bool proxy;

		coordinate groundPosition;
		eventproperty ungroundedEvent;
		double verticalError;
		bool hitSomething;
		bool isPlantedMS;

		hkleg() : proxy(true) {}
		hkleg(std::string line) : proxy(false) { groundPosition.update(line); }
	};

	struct controldata
	{
		gain gains;
	};

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	controldata controlData;
	std::vector<hkleg> legs;
	coordinate errorOutTranslation;
	coordinate alignWithGroundRotation;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<hkleg>& ori, std::vector<hkleg>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbfootikcontrolsmodifier>> hkbfootikcontrolsmodifierList;
extern safeStringUMap<std::shared_ptr<hkbfootikcontrolsmodifier>> hkbfootikcontrolsmodifierList_E;

#endif