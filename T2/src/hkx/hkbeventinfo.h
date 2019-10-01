#ifndef HKBEVENTINFO_H_
#define HKBEVENTINFO_H_

#include <string>

struct hkbeventinfo
{
	struct flags
	{
		bool FLAG_SILENT = false;
		bool FLAG_SYNC_POINT = false;

		std::string getflags();

		void update(std::string flag)
		{
			if (flag == "FLAG_SILENT") FLAG_SILENT = true;
			else if (flag == "FLAG_SYNC_POINT") FLAG_SYNC_POINT = true;
		}
	};

	flags flags;
};

#endif
