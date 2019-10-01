#include "stringmatch.h"
#include "xmlformat.h"

using namespace std;

void stringMatch(string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, bool caseSensitive)
{
	if (caseSensitive ? data_o == data_e : boost::to_lower_copy(data_o) == boost::to_lower_copy(data_e))
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoString(base, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoString(base, data_e));
		storeline.push_back(autoString(base, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}
