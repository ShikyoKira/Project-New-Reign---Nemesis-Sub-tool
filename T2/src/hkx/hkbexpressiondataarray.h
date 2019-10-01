#ifndef HKBEXPRESSIONDATAARRAY_H_
#define HKBEXPRESSIONDATAARRAY_H_

#include <iostream>
#include <fstream>
#include "Global-Type.h"
#include "src\hkx\hkbobject.h"

struct hkbexpressiondataarray : public hkbobject, std::enable_shared_from_this<hkbexpressiondataarray>
{
public:
	hkbexpressiondataarray() {}
	hkbexpressiondataarray(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();

	std::string tempaddress;
	bool IsNegated = false;


	struct expressiondata
	{
		enum eventmode
		{
			EVENT_MODE_SEND_ONCE,
			EVENT_MODE_SEND_ON_TRUE,
			EVENT_MODE_SEND_ON_FALSE_TO_TRUE,
			EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE
		};

		bool proxy;

		std::string expression;
		VariableId assignmentVariableIndex;
		EventId assignmentEventIndex;
		eventmode eventMode;

		std::string getEventMode();

		expressiondata() : proxy(true) {}
		expressiondata(std::string n_expression) : expression(n_expression), proxy(false) {}
	};

	std::vector<expressiondata> expressionsData;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	void matchScoring(std::vector<expressiondata>& ori, std::vector<expressiondata>& edit, std::string id);
};

extern safeStringUMap<std::shared_ptr<hkbexpressiondataarray>> hkbexpressiondataarrayList;
extern safeStringUMap<std::shared_ptr<hkbexpressiondataarray>> hkbexpressiondataarrayList_E;

void hkbExpressionDataArrayExport(std::string id);

#endif