#include "nemesisformat.h"

void nemesis::try_open(bool& open, bool& isEdited, vecstr& output)
{
	if (!open)
	{
		open = true;
		isEdited = true;
		output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
	}
}

void nemesis::try_close(bool& open, vecstr& output, vecstr& storeline)
{
	if (open)
	{
		if (storeline.size() > 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
			storeline.clear();
		}

		output.push_back("<!-- CLOSE -->");
		open = false;
	}
}
