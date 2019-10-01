#ifndef NEMESISFORMAT_H_
#define NEMESISFORMAT_H_

#include "Global.h"

namespace nemesis
{
	void try_open(bool& open, bool& isEdited, vecstr& output);
	void try_close(bool& open, vecstr& output, vecstr& storeline);
}

#endif
