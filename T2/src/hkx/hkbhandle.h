#ifndef HKBHANDLE_H_
#define HKBHANDLE_H_

#include "src\hkx\hkbobject.h"

struct hklocalframe : public hkbobject {};
struct hkprigidbody : public hkbobject {};
struct hkbcharacter : public hkbobject {};

struct hkbhandle : public hkbobject, std::enable_shared_from_this<hkbhandle>
{
	std::shared_ptr<hklocalframe> frame;
	std::shared_ptr<hkprigidbody> rigidBody;
	std::shared_ptr<hkbcharacter> character;
	int animationBoneIndex;

	hkbhandle() {}

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot) {}
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart) {}
	void newNode() {}

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
};

extern safeStringUMap<std::shared_ptr<hkbhandle>> hkbhandleList;
extern safeStringUMap<std::shared_ptr<hkbhandle>> hkbhandleList_E;

extern safeStringUMap<std::shared_ptr<hklocalframe>> hklocalframeList;
extern safeStringUMap<std::shared_ptr<hklocalframe>> hklocalframeList_E;

extern safeStringUMap<std::shared_ptr<hkprigidbody>> hkprigidbodyList;
extern safeStringUMap<std::shared_ptr<hkprigidbody>> hkprigidbodyList_E;

extern safeStringUMap<std::shared_ptr<hkbcharacter>> hkbcharacterList;
extern safeStringUMap<std::shared_ptr<hkbcharacter>> hkbcharacterList_E;

#endif
