#include "objectmatch.h"
#include "src\hkx\hkbobject.h"

using namespace std;

void objectMatch(string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		if (open)
		{
			if (storeline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}

			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back(autoObject(base, data_o));
	}
	else
	{
		if (!open)
		{
			open = true;
			isEdited = true;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		}

		output.push_back(autoObject(base, data_e));
		storeline.push_back(autoObject(base, data_o));
	}

	if (open && close)
	{
		if (storeline.size() > 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
		}

		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}
}

void objectMatch(string name, string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		if (open)
		{
			if (storeline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}

			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back(autoObject(base, name, data_o));
	}
	else
	{
		if (!open)
		{
			open = true;
			isEdited = true;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		}

		output.push_back(autoObject(base, name, data_e));
		storeline.push_back(autoObject(base, name, data_o));
	}

	if (open && close)
	{
		if (storeline.size() > 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
		}

		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}
}

void objectMatch(string name, string classname, string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		if (open)
		{
			if (storeline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}

			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back(autoObject(base, name, classname, data_o));
	}
	else
	{
		if (!open)
		{
			open = true;
			isEdited = true;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		}

		output.push_back(autoObject(base, name, classname, data_e));
		storeline.push_back(autoObject(base, name, classname, data_o));
	}

	if (open && close)
	{
		if (storeline.size() > 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
		}

		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}
}

void objectMatch(string name, string classname, string signature, string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close,
	bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		if (open)
		{
			if (storeline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}

			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		output.push_back(autoObject(base, name, classname, signature, data_o));
	}
	else
	{
		if (!open)
		{
			open = true;
			isEdited = true;
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
		}

		output.push_back(autoObject(base, name, classname, signature, data_e));
		storeline.push_back(autoObject(base, name, classname, signature, data_o));
	}

	if (open && close)
	{
		if (storeline.size() > 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
		}

		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}
}



void objectMatch(bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, classname, string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, string signature, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited)
{
	objectMatch(name, classname, signature, string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}



void objectMatch(int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, classname, to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, string signature, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited)
{
	objectMatch(name, classname, signature, to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}



void objectMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, classname, nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, string signature, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited)
{
	objectMatch(name, classname, signature, nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}



void objectMatch(shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	objectMatch(name, data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close,
	bool& open, bool& isEdited)
{
	objectMatch(name, classname, data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}

void objectMatch(string name, string classname, string signature, shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base,
	bool close, bool& open, bool& isEdited)
{
	objectMatch(name, classname, signature, data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}



void objectMatch(coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openObject(base));
	voidMatch(data_o, data_e, output, storeline, base, close, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void objectMatch(string name, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openObject(base, name));
	voidMatch(data_o, data_e, output, storeline, base, close, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void objectMatch(string name, string classname, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openObject(base, name, classname));
	voidMatch(data_o, data_e, output, storeline, base, close, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void objectMatch(string name, string classname, string signature, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openObject(base, name, classname, signature));
	voidMatch(data_o, data_e, output, storeline, base, close, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

