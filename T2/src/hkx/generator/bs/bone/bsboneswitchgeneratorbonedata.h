#ifndef BSBONESWITCHGENERATORBONEDATA_H_
#define BSBONESWITCHGENERATORBONEDATA_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"
#include "src\hkx\hkbvariablebindingset.h"
#include "src\hkx\generator\hkbgenerator.h"
#include "src\hkx\bone\hkbboneweightarray.h"

struct bsboneswitchgeneratorbonedata : public hkbobject, std::enable_shared_from_this<bsboneswitchgeneratorbonedata>
{
public:
	bsboneswitchgeneratorbonedata() {}

	std::string GetAddress();

	std::string address;
	std::string tempaddress;
	
	std::shared_ptr<hkbvariablebindingset> variableBindingSet;
	std::shared_ptr<hkbgenerator> pGenerator;
	std::shared_ptr<hkbboneweightarray> spBoneWeight;

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

void BSBoneSwitchGeneratorBoneDataExport(std::string id);

extern safeStringUMap<std::shared_ptr<bsboneswitchgeneratorbonedata>> bsboneswitchgeneratorbonedataList;
extern safeStringUMap<std::shared_ptr<bsboneswitchgeneratorbonedata>> bsboneswitchgeneratorbonedataList_E;

#endif