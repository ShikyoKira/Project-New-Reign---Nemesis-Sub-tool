#ifndef HKBPROXYMODIFIER_H_
#define HKBPROXYMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbproxymodifier : public hkbmodifier, std::enable_shared_from_this<hkbproxymodifier>
{
public:
	hkbproxymodifier() {}
	hkbproxymodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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


	enum phantomtype
	{
		PHANTOM_TYPE_SIMPLE,
		PHANTOM_TYPE_CACHING
	};

	enum linearvelocitymode
	{
		LINEAR_VELOCITY_MODE_WORLD,
		LINEAR_VELOCITY_MODE_MODEL
	};

	struct proxyinfo
	{
		double dynamicFriction;
		double staticFriction;
		int keepContactTolerance;
		coordinate up;
		double keepDistance;
		double contactAngleSensitivity;
		int userPlanes;
		double maxCharacterSpeedForSolver;
		double characterStrength;
		double characterMass;
		double maxSlope;
		double penetrationRecoverySpeed;
		int maxCastIterations;
		bool refreshManifoldInCheckSupport;
	};
	
	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	proxyinfo proxyInfo;
	coordinate linearVelocity;
	double horizontalGain;
	double verticalGain;
	double maxHorizontalSeparation;
	double limitHeadingDegrees;
	double maxVerticalSeparation;
	double verticalDisplacementError;
	double verticalDisplacementErrorGain;
	double maxVerticalDisplacement;
	double minVerticalDisplacement;
	double capsuleHeight;
	double capsuleRadius;
	double maxSlopeForRotation;
	int collisionFilterInfo;
	phantomtype phantomType;
	linearvelocitymode linearVelocityMode;
	bool ignoreIncomingRotation;
	bool ignoreCollisionDuringRotation;
	bool ignoreIncomingTranslation;
	bool includeDownwardMomentum;
	bool followWorldFromModel;
	bool isTouchingGround;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	std::string getPhantomType();
	std::string getLinearVelocityMode();
};

void hkbProxyModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbproxymodifier>> hkbproxymodifierList;
extern safeStringUMap<std::shared_ptr<hkbproxymodifier>> hkbproxymodifierList_E;

#endif