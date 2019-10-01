#pragma once
#ifndef SAFE_PTR_H
#define SAFE_PTR_H

#include <condition_variable>
#include <mutex>
#include <atomic>

extern std::atomic<unsigned short> num_stringData;
extern std::mutex mutex_stringData;
extern std::condition_variable cont_stringData;

#endif // #ifndef SAFE_PTR_H