#ifndef VARIABLEINFOPACK_H_
#define VARIABLEINFOPACK_H_

#include <unordered_map>

#include "Global.h"

#include "src\hkx\hkbvariableinfo.h"
#include "src\hkx\hkbvariablevalue.h"

struct hkbbehaviorgraph;

struct VariableId
{
	std::string nodeID;
	int id;
	hkbbehaviorgraph* graphroot;

	VariableId() : id(-1) {}
	VariableId(int n_id);
	VariableId(std::string n_id);

	void connectVariableInfo(std::string n_nodeID, hkbbehaviorgraph* n_graphroot);

	bool operator==(VariableId& n_evnt);
	bool operator!=(VariableId& n_evnt);

	std::string operator+(std::string line);
	std::string operator=(std::string line);

	std::string getName();
	std::string getID();

private:
	void dataBake(int n_id);
	bool errorCheck();
};

struct v_datainfo : public hkbvariableinfo, public hkbvariablevalue
{
	std::string name;

	v_datainfo() : hkbvariablevalue() {}
	v_datainfo(std::string n_name) : name(n_name), hkbvariablevalue(0) {}
};

struct variableinfopack : public std::vector<v_datainfo>
{
	bool scan_done = false;

	size_t d_size();
	void regis_size(size_t num);
	bool hasVariableName(std::string name);
	int getID(std::string name);

	void push_back(v_datainfo datainfo);
	
private:
	size_t v_size = 0;
	std::unordered_map<std::string, size_t> locator;
};

namespace variablefunc
{
	void matchScoring(variableinfopack& ori, variableinfopack& edit, std::string id, std::string classname);
	void fillScore(variableinfopack& ori, variableinfopack& edit, usize start, usize cap, std::map<int, std::map<int, double>>& scorelist, bool dup, std::set<std::string> dupname);
}

extern safeStringUMap<std::shared_ptr<variableinfopack>> variablePackNode;
extern safeStringUMap<std::shared_ptr<variableinfopack>> variablePackNode_E;

#endif