#ifndef HKBBEHAVIORGRAPHSTRINGDATA_H_
#define HKBBEHAVIORGRAPHSTRINGDATA_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\utilities\eventinfopack.h"
#include "src\utilities\variableinfopack.h"
#include "src\utilities\attributeinfopack.h"

struct hkbbehaviorgraphstringdata : public hkbobject, std::enable_shared_from_this<hkbbehaviorgraphstringdata>
{
public:
	hkbbehaviorgraphstringdata() {}

	std::string GetAddress();

	std::string tempaddress;

	std::shared_ptr<eventinfopack> eventNames;
	std::shared_ptr<attributeinfopack> attributeNames;
	std::shared_ptr<variableinfopack> variableNames;
	std::shared_ptr<variableinfopack> characterPropertyNames;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	void matchScoring(std::vector<e_datainfo>& ori, std::vector<e_datainfo>& edit, std::string id);
	void fillScore(std::vector<e_datainfo>& ori, std::vector<e_datainfo>& edit, usize start, usize cap, std::map<int, std::map<int, double>>& scorelist);
};

extern safeStringUMap<std::shared_ptr<hkbbehaviorgraphstringdata>> hkbbehaviorgraphstringdataList;
extern safeStringUMap<std::shared_ptr<hkbbehaviorgraphstringdata>> hkbbehaviorgraphstringdataList_E;

#endif