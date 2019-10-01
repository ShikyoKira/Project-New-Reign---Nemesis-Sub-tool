#ifndef HKXOBJECT_H_
#define HKXOBJECT_H_

#include <string>
#include <memory>
#include <boost\atomic.hpp>
#include <unordered_map>
#include <boost\thread\mutex.hpp>
#include <boost\thread\lock_guard.hpp>
#include "src\utilities\xmlformat.h"

extern std::atomic<unsigned int> num_thread;
extern safeStringMap<std::string> newID;
extern safeStringMap<std::string> region;		// ID = region
extern std::unordered_map<std::string, int> regioncount;		// region same name count
extern boost::atomic_flag regioncountlock;

struct hkbbehaviorgraph;

struct hkbobject
{
public:
	std::string ID;
	std::string address;
	vecstr poolAddress;
	bool isModifier = false;
	bool isGenerator = false;
	bool isCondition = false;
	bool isTransition = false;

	virtual void regis(std::string id, bool isEdited) {}
	virtual void dataBake(std::string filepath, vecstr& nodelines, bool isEdited) {}
	virtual void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot) {}
	virtual void match(std::shared_ptr<hkbobject> counterpart) {}
	virtual void newNode() {}
	virtual std::string getClassCode() { return ""; }

protected:
	boost::mutex nodeMutex;
};

#endif
