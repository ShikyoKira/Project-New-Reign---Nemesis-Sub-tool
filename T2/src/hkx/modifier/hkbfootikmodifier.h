#ifndef HKBFOOTIKMODIFIER_H_
#define HKBFOOTIKMODIFIER_H_

#include "src\utilities\qstransform.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"
#include "src\hkx\modifier\base\hkbfootikgains.h"

struct hkbfootikmodifier : public hkbmodifier, std::enable_shared_from_this<hkbfootikmodifier>
{
public:
	hkbfootikmodifier() {}
	hkbfootikmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	bool IsNegated = false;
	
	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier

	struct leg
	{
		bool proxy;

		qstransform originalAnkleTransformMS;
		coordinate kneeAxisLS;
		coordinate footEndLS;
		eventproperty ungroundedEvent;
		double footPlantedAnkleHeightMS;
		double footRaisedAnkleHeightMS;
		double maxAnkleHeightMS;
		double minAnkleHeightMS;
		double maxKneeAngleDegrees;
		double minKneeAngleDegrees;
		double verticalError;
		double maxAnkleAngleDegrees;
		int hipIndex;
		int kneeIndex;
		int ankleIndex;
		bool hitSomething;
		bool isPlantedMS;
		bool isOriginalAnkleTransformMSSet;

		leg() : proxy(true) {}
		leg(qstransform n_originalAnkleTransformMS) : originalAnkleTransformMS(n_originalAnkleTransformMS), proxy(false) {}
	};

	enum mode
	{
		ALIGN_MODE_FORWARD_RIGHT = 0,
		ALIGN_MODE_FORWARD = 1
	};

	gain gains;
	std::vector<leg> legs;
	double raycastDistanceUp;
	double raycastDistanceDown;
	double originalGroundHeightMS;
	double errorOut;
	coordinate errorOutTranslation;
	coordinate alignWithGroundRotation;
	double verticalOffset;
	int collisionFilterInfo;
	double forwardAlignFraction;
	double sidewaysAlignFraction;
	double sidewaysSampleWidth;
	bool useTrackData;
	bool lockFeetWhenPlanted;
	bool useCharacterUpVector;
	mode alignMode;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<hkbfootikmodifier::leg>& ori, std::vector<hkbfootikmodifier::leg>& edit, std::string id);
	std::string getMode();
};

void hkbFootIkModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbfootikmodifier>> hkbfootikmodifierList;
extern safeStringUMap<std::shared_ptr<hkbfootikmodifier>> hkbfootikmodifierList_E;



#endif
