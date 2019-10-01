#include <boost\regex.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\algorithm\string.hpp>
#include "coordinate.h"

using namespace std;

namespace nemesis
{
	string to_string(double num);
}

string space(usize base);

bool isOnlyNumber(string line);

bool coordinate::operator==(coordinate& ctrpart)
{
	if (X != ctrpart.X) return false;
	else if (Y != ctrpart.Y) return false;
	else if (Z != ctrpart.Z) return false;
	else if (W != ctrpart.W) return false;

	return true;
}

bool coordinate::operator!=(coordinate & ctrpart)
{
	if (X != ctrpart.X) return true;
	else if (Y != ctrpart.Y) return true;
	else if (Z != ctrpart.Z) return true;
	else if (W != ctrpart.W) return true;

	return false;
}

void coordinate::update(string line)
{
	vector<string> tokens;
	boost::trim_if(line, boost::is_any_of("\t ()"));
	boost::split(tokens, line, boost::is_any_of("\t ()"), boost::token_compress_on);

	if (tokens.size() > 0)
	{
		X = stod(tokens[0]);
		Y = stod(tokens[1]);
		Z = stod(tokens[2]);
		W = stod(tokens[3]);
	}
}

string coordinate::getString()
{
	return "(" + nemesis::to_string(X) + " " + nemesis::to_string(Y) + " " + nemesis::to_string(Z) + " " + nemesis::to_string(W) + ")";
}

string coordinate::getString(usize base)
{
	string spaces = space(base);
	return spaces + nemesis::to_string(X) + "\n" + spaces + nemesis::to_string(Y) + "\n" + spaces + nemesis::to_string(Z) + "\n" + spaces + nemesis::to_string(W);
}
