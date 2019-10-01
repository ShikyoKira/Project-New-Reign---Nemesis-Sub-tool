#ifndef ATTRIBUTEINFOPACK_H_
#define ATTRIBUTEINFOPACK_H_

#include "Global.h"

struct a_datainfo
{
	bool proxy;

	double value;
	std::string name;

	a_datainfo() : proxy(true) {}
	a_datainfo(std::string n_name) : name(n_name), proxy(false) {}
};

struct attributeinfopack : public std::vector<a_datainfo>
{
	bool scan_done = false;

	usize d_size();
	void regis_size(usize num);

private:
	usize v_size = 0;
};

namespace attributefunc
{
	void matchScoring(attributeinfopack& ori, attributeinfopack& edit, std::string id, std::string classname);
	void fillScore(attributeinfopack& ori, attributeinfopack& edit, usize start, usize cap, std::map<int, std::map<int, double>>& scorelist, bool dup, std::set<std::string> dupname);
}

extern safeStringUMap<std::shared_ptr<attributeinfopack>> attributePackNode;
extern safeStringUMap<std::shared_ptr<attributeinfopack>> attributePackNode_E;

#endif
