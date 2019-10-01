#pragma once

#include "src\hkx\hkxclass.h"
#include "src\hkx\bone\bones.h"
#include "src\hkx\modifier\modifiers.h"
#include "src\hkx\condition\condition.h"
#include "src\hkx\generator\generators.h"
#include "src\hkx\transition\transition.h"

void AddBehavior(std::string filename, vecstr& storeline, bool edited);
void AddAnimData(std::string filename, vecstr& storeline, bool edited);
void AddAnimSetData(std::string filename, vecstr& storeline, bool edited);
