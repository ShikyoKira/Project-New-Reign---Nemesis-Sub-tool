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
#include "Global-Type.h"

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
	std::unordered_map<std::string, vecstr> node;

public:
	vecstr& operator[] (std::string id)
	{
		nodelock locker(&locker);
		return node[id];
	}

	unsigned int size()
	{
		return node.size();
	}

	void erase(std::unordered_map<std::string, vecstr>::iterator iter)
	{
		nodelock locker(&locker);
		node.erase(iter);
	}

	std::unordered_map<std::string, vecstr>::iterator find(std::string key)
	{
		nodelock locker(&locker);
		return node.find(key);
	}
};

template<typename subclass>
class safeStringMap : public std::map<std::string, subclass>
{
private:
	std::atomic_flag locker = ATOMIC_FLAG_INIT;

public:
	subclass& operator[] (std::string key)
	{
		nodelock locker(&locker);
		return try_emplace(_STD move(key)).first->second;
	}

	iterator find(const std::string& _Keyval)
	{
		nodelock locker(&locker);
		return lower_bound(_Keyval);
	}
};

template<typename subclass>
class safeStringUMap : public std::unordered_map<std::string, subclass>
{
private:
	std::atomic_flag locker = ATOMIC_FLAG_INIT;

public:
	subclass& operator[] (std::string key)
	{
		safeStringMap<string> test;
		test["hehe"];

		nodelock locker(&locker);
		return try_emplace(_STD move(key)).first->second;
	}

	subclass& at(std::string key)
	{
		nodelock locker(&locker);
		iterator _Where = _Mybase::lower_bound(key);

		if (_Where == _Mybase::end()) std::cout << "invalid unordered_map<K, T> key" << std::endl;

		return _Where->second;
	}

	iterator find(const std::string& _Keyval)
	{
		nodelock locker(&locker);
		return lower_bound(_Keyval);
	}

	iterator end() _NOEXCEPT
	{
		nodelock locker(&locker);
		return _List.end();
	}
};

typedef safeStringMap<std::shared_ptr<hkbobject>> hkRefPtr;

void NemesisReaderFormat(int id, vecstr& output);
void FolderCreate(std::string curBehaviorPath);
void GetFunctionLines(std::string filename, vecstr& storeline);
std::string NodeIDCheck(std::string ID);
vecstr GetElements(std::string number, std::unordered_map<std::string, vecstr>& functionlines, bool isTransition = false, std::string key = "");
bool isOnlyNumber(std::string line);
bool hasAlpha(std::string line);

#endif