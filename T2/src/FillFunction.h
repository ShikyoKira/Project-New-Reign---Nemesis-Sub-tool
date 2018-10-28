#pragma once

#include "Global.h"

typedef std::vector<std::string> vecstr;

void AddBehavior(std::string filename, vecstr& storeline, bool edited);
void AddAnimData(std::string filename, vecstr& storeline, bool edited);
void AddAnimSetData(std::string filename, vecstr& storeline, bool edited);
