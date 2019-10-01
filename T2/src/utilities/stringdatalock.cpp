#include "stringdatalock.h"

using namespace std;

atomic<unsigned short> num_stringData;
mutex mutex_stringData;
condition_variable cont_stringData;
