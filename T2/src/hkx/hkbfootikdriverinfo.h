#ifndef HKBFOOTIKDRIVERINFO_H_
#define HKBFOOTIKDRIVERINFO_H_

#include <iostream>
#include <fstream>
#include "src\utilities\coordinate.h"
#include "src\hkx\hkbobject.h"

struct hkbfootikdriverinfo : public hkbobject, std::enable_shared_from_this<hkbfootikdriverinfo>
{
public:
	hkbfootikdriverinfo() {}
	hkbfootikdriverinfo(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();
	
	std::string tempaddress;
	bool IsNegated = false;


	struct leg
	{
		bool proxy;

		coordinate kneeAxisLS;
		coordinate footEndLS;
		double footPlantedAnkleHeightMS;
		double footRaisedAnkleHeightMS;
		double maxAnkleHeightMS;
		double minAnkleHeightMS;
		double maxKneeAngleDegrees;
		double minKneeAngleDegrees;
		double maxAnkleAngleDegrees;
		int hipIndex;
		int kneeIndex;
		int ankleIndex;

		leg() : proxy(true) {}
		leg(coordinate n_kneeAxisLS) : kneeAxisLS(n_kneeAxisLS), proxy(false) {}
		leg(double X, double Y, double Z, double W) : kneeAxisLS(coordinate(X, Y, Z, W)), proxy(false) {}
	};

	std::vector<leg> legs;
	double raycastDistanceUp;
	double raycastDistanceDown;
	double originalGroundHeightMS;
	double verticalOffset;
	double collisionFilterInfo;
	double forwardAlignFraction;
	double sidewaysAlignFraction;
	double sidewaysSampleWidth;
	bool lockFeetWhenPlanted;
	bool useCharacterUpVector;
	bool isQuadrupedNarrow;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	void matchScoring(std::vector<leg>& ori, std::vector<leg>& edit, std::string id);
};

void hkbFootIkDriverInfoExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbfootikdriverinfo>> hkbfootikdriverinfoList;
extern safeStringUMap<std::shared_ptr<hkbfootikdriverinfo>> hkbfootikdriverinfoList_E;

#endif