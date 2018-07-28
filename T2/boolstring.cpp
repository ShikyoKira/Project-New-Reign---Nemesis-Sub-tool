#include "boolstring.h"

using namespace std;

string from_bool(bool boolean)
{
	if (boolean)
	{
		return "true";
	}

	return "false";
}

bool to_bool(string line)
{
	bool boolean;
	boost::to_lower(line);
	istringstream stream(line);
	stream >> boolalpha >> boolean;
	return boolean;
}
