#include "hkbobject.h"
#include "highestscore.h"

using namespace std;

atomic<unsigned int> num_thread = 0;
safeStringMap<string> newID;
safeStringMap<string> region;
unordered_map<string, int> regioncount;
boost::atomic_flag regioncountlock = BOOST_ATOMIC_FLAG_INIT;
