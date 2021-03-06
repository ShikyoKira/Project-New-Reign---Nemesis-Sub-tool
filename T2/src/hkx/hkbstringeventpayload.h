#ifndef HKBSTRINGEVENTPAYLOAD_H_
#define HKBSTRINGEVENTPAYLOAD_H_

#include <iostream>
#include <fstream>
#include "src\hkx\hkbobject.h"

struct hkbstringeventpayload : public hkbobject, std::enable_shared_from_this<hkbstringeventpayload>
{
public:
	hkbstringeventpayload() {}

	std::string GetAddress();

	std::string tempaddress;

	std::string data;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
};

extern safeStringUMap<std::shared_ptr<hkbstringeventpayload>> hkbstringeventpayloadList;
extern safeStringUMap<std::shared_ptr<hkbstringeventpayload>> hkbstringeventpayloadList_E;

#endif