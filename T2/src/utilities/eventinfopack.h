#ifndef EVENTINFOPACK_H_
#define EVENTINFOPACK_H_

#include "Global.h"
#include "src\hkx\hkbeventinfo.h"
#include "src\hkx\hkbvariablevalue.h"

struct hkbbehaviorgraph;

struct EventId
{
	std::string nodeID;
	int id;
	hkbbehaviorgraph* graphroot = nullptr;

	EventId() : id(-1) {}
	EventId(int n_id);
	EventId(std::string n_id);

	void connectEventInfo(std::string n_nodeID, hkbbehaviorgraph* n_graphroot);

	bool operator==(EventId& n_evnt);
	bool operator!=(EventId& n_evnt);

	std::string operator+(std::string line);
	std::string operator=(std::string line);

	std::string getName();
	std::string getID();

private:
	void dataBake(int n_id);
	bool errorCheck();
};

struct e_datainfo : public hkbeventinfo
{
	bool proxy;

	std::string name;

	e_datainfo() : proxy(true) {}
	e_datainfo(std::string n_name) : name(n_name), proxy(false) {}
};

struct eventinfopack : public std::vector<e_datainfo>
{
	bool scan_done = false;

	size_t d_size();
	void regis_size(size_t num);
	bool hasEventName(std::string name);
	size_t getID(std::string name);

	void push_back(e_datainfo datainfo);

private:
	size_t v_size = 0;
	std::unordered_map<std::string, size_t> locator;
};

namespace eventfunc
{
	void matchScoring(eventinfopack& ori, eventinfopack& edit, std::string id, std::string classname);
	void fillScore(eventinfopack& ori, eventinfopack& edit, usize start, usize cap, std::map<int, std::map<int, double>>& scorelist, bool dup, std::set<std::string> dupname);
}


extern safeStringUMap<std::shared_ptr<eventinfopack>> eventPackNode;
extern safeStringUMap<std::shared_ptr<eventinfopack>> eventPackNode_E;

#endif
