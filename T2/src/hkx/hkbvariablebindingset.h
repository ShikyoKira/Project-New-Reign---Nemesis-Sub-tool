#ifndef HKBVARIABLEBINDINGSET_H_
#define HKBVARIABLEBINDINGSET_H_

#include <iostream>
#include <fstream>

#include "src\hkx\hkbobject.h"

#include "src\utilities\variableinfopack.h"

struct hkbvariablebindingset : public hkbobject, std::enable_shared_from_this<hkbvariablebindingset>
{
public:
	hkbvariablebindingset() {}
	hkbvariablebindingset(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	bool IsNegated = false;


	struct binding
	{
		enum bindingtype
		{
			BINDING_TYPE_VARIABLE,
			BINDING_TYPE_CHARACTER_PROPERTY
		};
		
		bool proxy;

		std::string memberPath;
		VariableId variableIndex;
		int bitIndex;
		bindingtype bindingType;

		std::string getBindingType();

		binding() : proxy(true) {}
		binding(std::string n_memberPath) : memberPath(n_memberPath), proxy(false) {}
	};

	std::vector<binding> bindings;
	int indexOfBindingToEnable;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	void matchScoring(std::vector<binding>& ori, std::vector<binding>& edit, std::string id);
};

void hkbVariableBindingSetExport(std::string id);

extern safeStringUMap<std::shared_ptr<hkbvariablebindingset>> hkbvariablebindingsetList;
extern safeStringUMap<std::shared_ptr<hkbvariablebindingset>> hkbvariablebindingsetList_E;

#endif