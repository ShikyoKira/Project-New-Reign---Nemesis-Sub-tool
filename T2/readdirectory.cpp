#include "readdirectory.hpp"

void read_directory(const std::string & name, std::vector<std::string>& fv)
{
	boost::filesystem::path p(name);
	boost::filesystem::directory_iterator start(p);
	boost::filesystem::directory_iterator end;
	std::transform(start, end, back_inserter(fv), path_leaf_string());
}
