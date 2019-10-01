#ifndef HKROOTLEVELCONTAINER_H_
#define HKROOTLEVELCONTAINER_H_

#include <iostream>
#include <fstream>
#include <unordered_map>
#include "src\hkx\hkbobject.h"
#include "src\hkx\generator\hkbbehaviorgraph.h"

struct hkrootlevelcontainer : public hkbobject, std::enable_shared_from_this<hkrootlevelcontainer>
{
public:
	hkrootlevelcontainer() {}
	hkrootlevelcontainer(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string NextGenerator();
	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	std::string generator;
	bool IsNegated = false;


	struct namedvariant
	{
		bool proxy;

		std::string name;
		std::string className;
		std::shared_ptr<hkbobject> variant;

		namedvariant() : proxy(true) {}
		namedvariant(std::string n_name) : name(n_name), proxy(false) {}
	};

	std::vector<namedvariant> namedVariants;
	
	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void matchScoring(std::vector<namedvariant>& ori, std::vector<namedvariant>& edit, std::string id);
};

void hkRootLevelContainerExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkrootlevelcontainer>> hkrootlevelcontainerList;
extern safeStringUMap<std::shared_ptr<hkrootlevelcontainer>> hkrootlevelcontainerList_E;

#endif