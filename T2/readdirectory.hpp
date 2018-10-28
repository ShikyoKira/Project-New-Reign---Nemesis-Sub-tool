#ifndef READDIRECTORY_H_
#define READDIRECTORY_H_

#include <boost\filesystem.hpp>
#include <vector>

struct path_leaf_string
{
	std::string operator()(const boost::filesystem::directory_entry& entry) const
	{
		return entry.path().leaf().string();
	}
};

void read_directory(const std::string& name, std::vector<std::string>& fv);

#endif