#ifndef HKBCHARACTERSTRINGDATA_H_
#define HKBCHARACTERSTRGINDATA_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct hkbcharacterstringdata
{
public:
	hkbcharacterstringdata(std::string filepath, std::string id, std::string preaddress, int functionLayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	bool IsNegated = false;
};

void hkbCharacterStringDataExport(std::string id);
inline void process(std::vector<std::string> storeline1, std::vector<std::string> storeline2, int curline, int i, bool& IsChanged, int& openpoint, bool& open, bool& IsEdited, std::vector<std::string>& output);
inline void postProcess(std::string elementName, std::vector<std::string> storeline1, std::vector<std::string> storeline2, int curline, int i, bool& IsChanged, int openpoint, bool& open, bool& IsEdited, std::vector<std::string>& output);

#endif