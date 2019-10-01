#ifndef HKBEVENTRANGEDATAARRAY_H_
#define HKBEVENTRANGEDATAARRAY_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventproperty.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct hkbeventrangedataarray : public hkbobject, std::enable_shared_from_this<hkbeventrangedataarray>
{
public:
	hkbeventrangedataarray() {}
	hkbeventrangedataarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

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

	
	struct eventdata
	{
		enum eventmode
		{
			EVENT_MODE_SEND_ON_ENTER_RANGE,
			EVENT_MODE_SEND_WHEN_IN_RANGE
		};

		bool proxy;

		double upperBound;
		eventproperty event;
		eventmode eventMode;

		std::string getEventMode();

		eventdata() : proxy(true) {}
		eventdata(double n_upperBound) : upperBound(n_upperBound), proxy(false) {}
	};

	std::vector<eventdata> eventData;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<eventdata>& ori, std::vector<eventdata>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbEventRangeDataArrayExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbeventrangedataarray>> hkbeventrangedataarrayList;
extern safeStringUMap<std::shared_ptr<hkbeventrangedataarray>> hkbeventrangedataarrayList_E;

#endif