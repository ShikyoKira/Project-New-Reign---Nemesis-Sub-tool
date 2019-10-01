#ifndef STRINGMATCH_H_
#define STRINGMATCH_H_

#include "Global-Type.h"

void stringMatch(std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, bool caseSensitive = true);

#endif
