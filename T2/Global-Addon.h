#ifndef GLOBALADDON_H_
#define GLOBALADDON_H_

#include <string>
#include <vector>
#include <atomic>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <boost\date_time\posix_time\posix_time.hpp>
#include <boost\thread\mutex.hpp>
#include <boost\thread\lock_guard.hpp>

class nodelock
{
private:
	std::atomic_flag* locker;

public:
	nodelock(std::atomic_flag* n_locker)
	{
		while (n_locker->test_and_set(std::memory_order_acquire));
		locker = n_locker;
	}

	~nodelock()
	{
		locker->clear(std::memory_order_release);
	}
};

class nodelist
{
private:
	std::atomic_flag locker = ATOMIC_FLAG_INIT;
	std::unordered_map<std::string, std::vector<std::string>> node;

public:
	std::vector<std::string>& operator[] (std::string id)
	{
		nodelock locker(&locker);
		return node[id];
	}

	unsigned int size()
	{
		return node.size();
	}

	void erase(std::unordered_map<std::string, std::vector<std::string>>::iterator iter)
	{
		nodelock locker(&locker);
		node.erase(iter);
	}

	std::unordered_map<std::string, std::vector<std::string>>::iterator find(std::string key)
	{
		nodelock locker(&locker);
		return node.find(key);
	}
};

extern void ReferenceReplacementExt(std::string wrongReference, std::string rightReference); // replacement function for foreign principle
extern void ReferenceReplacement(std::string wrongReference, std::string rightReference, bool reserve = false);
extern void NemesisReaderFormat(std::vector<std::string>& output, bool hasID = false);
extern void FolderCreate(std::string curBehaviorPath);
extern void GetFunctionLines(std::string filename, std::vector<std::string>& storeline);
extern std::vector<std::string> GetElements(std::string number, std::unordered_map<std::string, std::vector<std::string>>& functionlines, bool isTransition = false, std::string key = "");
extern bool isOnlyNumber(std::string line);
extern bool hasAlpha(std::string line);

#endif