#ifndef BOOLSTRING_H_
#define BOOLSTRING_H_

#include <string>
#include <sstream>
#include <boost\algorithm\string.hpp>

std::string from_bool(bool boolean);
bool to_bool(std::string line);

#endif
