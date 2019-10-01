#ifndef HKBCLIPTRIGGERARRAY_H_
#define HKBCLIPTRIGGERARRAY_H_

#include <iostream>
#include <fstream>
#include <map>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"

struct hkbcliptriggerarray : public hkbobject, std::enable_shared_from_this<hkbcliptriggerarray>
{
public:
	hkbcliptriggerarray() {}
	hkbcliptriggerarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	int GetPayloadCount();
	std::string GetPayload(int child);
	bool IsPayloadNull(int child);

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	vecstr payload;
	bool IsNegated = false;


	struct hkbcliptrigger
	{
		bool proxy;

		double localTime;
		eventproperty event;
		bool relativeToEndOfClip;
		bool acyclic;
		bool isAnnotation;

		hkbcliptrigger() : proxy(true) {}
		hkbcliptrigger(double n_localTime) : localTime(n_localTime), proxy(false) {}
	};

	std::vector<hkbcliptrigger> triggers;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<hkbcliptrigger>& ori, std::vector<hkbcliptrigger>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbClipTriggerArrayExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbcliptriggerarray>> hkbcliptriggerarrayList;
extern safeStringUMap<std::shared_ptr<hkbcliptriggerarray>> hkbcliptriggerarrayList_E;

#endif