#ifndef HKBEVENTSFROMRANGEMODIFIER_H_
#define HKBEVENTSFROMRANGEMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventrangedataarray.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbeventsfromrangemodifier : public hkbmodifier, std::enable_shared_from_this<hkbeventsfromrangemodifier>
{
public:
	hkbeventsfromrangemodifier() {}
	hkbeventsfromrangemodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetEventRanges();
	bool IsEventRangesNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variablebindingset;
	std::string eventranges;
	bool IsNegated = false;


	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	double inputValue;
	double lowerBound;
	std::shared_ptr<hkbeventrangedataarray> eventRanges;

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

void hkbEventsFromRangeModifierExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbeventsfromrangemodifier>> hkbeventsfromrangemodifierList;
extern safeStringUMap<std::shared_ptr<hkbeventsfromrangemodifier>> hkbeventsfromrangemodifierList_E;

#endif