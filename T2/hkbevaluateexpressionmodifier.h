#ifndef HKBEVALUATEEXPRESSIONMODIFIER_H_
#define HKBEVALUATEEXPRESSIONMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbevaluateexpressionmodifier
{
public:
	hkbevaluateexpressionmodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetExpression();
	bool IsExpressionNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string variablebindingset;
	std::string expression;
	bool IsNegated = false;
};

extern void hkbEvaluateExpressionModifierExport(std::string originalfile, std::string editedfile, std::string id);

#endif