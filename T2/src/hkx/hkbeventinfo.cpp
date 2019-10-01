#include "hkbeventinfo.h"

using namespace std;

string hkbeventinfo::flags::getflags()
{
	string flags;

	if (FLAG_SYNC_POINT) flags.append("FLAG_SYNC_POINT|");
	if (FLAG_SILENT) flags.append("FLAG_SILENT|");

	if (flags.length() == 0) return "0";

	flags.pop_back();
	return flags;
}
