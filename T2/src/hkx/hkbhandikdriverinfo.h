#ifndef HKBHANDIKDRIVERINFO_H_
#define HKBHANDIKDRIVERINFO_H_

#include "src\utilities\coordinate.h"
#include "src\hkx\blendcurve.h"
#include "src\hkx\hkbobject.h"

struct hkbhandikdriverinfo : public hkbobject, std::enable_shared_from_this<hkbhandikdriverinfo>
{
public:
	hkbhandikdriverinfo() {}

	struct hkbhand
	{
		bool proxy;

		coordinate elbowAxisLS;
		coordinate backHandNormalLS;
		coordinate handOffsetLS;
		coordinate handOrienationOffsetLS;
		double maxElbowAngleDegrees;
		double minElbowAngleDegrees;
		int shoulderIndex;
		int shoulderSiblingIndex;
		int elbowIndex;
		int elbowSiblingIndex;
		int wristIndex;
		bool enforceEndPosition;
		bool enforceEndRotation;
		std::string localFrameName;

		hkbhand() : proxy(true) {}
		hkbhand(coordinate n_elbowAxisLS) : elbowAxisLS(n_elbowAxisLS), proxy(false) {}
		hkbhand(double X, double Y, double Z, double W) : elbowAxisLS(coordinate(X, Y, Z, W)), proxy(false) {}
	};

	std::vector<hkbhand> hands;
	blendcurve fadeInOutCurve;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	void matchScoring(std::vector<hkbhand>& ori, std::vector<hkbhand>& edit, std::string id);
};

extern safeStringUMap<std::shared_ptr<hkbhandikdriverinfo>> hkbhandikdriverinfoList;
extern safeStringUMap<std::shared_ptr<hkbhandikdriverinfo>> hkbhandikdriverinfoList_E;

#endif
