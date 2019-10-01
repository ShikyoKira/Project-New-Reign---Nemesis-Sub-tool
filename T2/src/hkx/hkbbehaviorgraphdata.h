#ifndef HKBBEHAVIORGRAPHDATA_H_
#define HKBBEHAVIORGRAPHDATA_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbeventinfo.h"
#include "src\hkx\hkbvariableinfo.h"
#include "src\hkx\hkbvariablevalue.h"
#include "src\hkx\hkbvariablevalueset.h"
#include "src\hkx\hkbbehaviorgraphstringdata.h"
#include "src\utilities\attributeinfopack.h"

extern std::vector<usize> datapacktracker;

struct hkbbehaviorgraphdata : public hkbobject, std::enable_shared_from_this<hkbbehaviorgraphdata>
{
public:
	hkbbehaviorgraphdata() {}
	hkbbehaviorgraphdata(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetVariableInitialValues();
	std::string GetStringData();

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string variableinitialvalues;
	std::string stringdata;
	bool IsNegated = false;
	

	std::shared_ptr<attributeinfopack> attributeDefaults;
	std::shared_ptr<variableinfopack> variableInfos;
	std::shared_ptr<variableinfopack> characterPropertyInfos;
	std::shared_ptr<eventinfopack> eventInfos;
	std::vector<hkbvariablevalue> wordMinVariableValues;
	std::vector<hkbvariablevalue> wordMaxVariableValues;
	std::shared_ptr<hkbvariablevalueset> variableInitialValues;
	std::shared_ptr<hkbbehaviorgraphstringdata> stringData;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<hkbvariablevalue>& ori, std::vector<hkbvariablevalue>& edit, std::string id);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

void hkbBehaviorGraphDataExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbbehaviorgraphdata>> hkbbehaviorgraphdataList;
extern safeStringUMap<std::shared_ptr<hkbbehaviorgraphdata>> hkbbehaviorgraphdataList_E;

#endif