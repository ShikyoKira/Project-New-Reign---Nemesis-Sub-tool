#include "xmlformat.h"
#include "src\hkx\hkbobject.h"
#include <set>
#include <boost\algorithm\string.hpp>

using namespace std;

void flaggingTranslation(string data_o, string& data_e)
{
	set<string> list;

	vecstr datalist_o;
	boost::trim_if(data_o, boost::is_any_of("\t "));
	boost::split(datalist_o, data_o, boost::is_any_of("|"), boost::token_compress_on);

	vecstr datalist_e;
	boost::trim_if(data_e, boost::is_any_of("\t "));
	boost::split(datalist_e, data_e, boost::is_any_of("|"), boost::token_compress_on);

	for (auto& each : datalist_o)
	{
		list.insert(each);
	}

	for (auto& each : datalist_e)
	{
		list.insert(each);
	}

	usize counter = 0;
	unordered_map<string, usize> numFlag;

	for (string flag : list)
	{
		numFlag[flag] = 1 << counter++;
	}

	usize flags_o;
	usize flags_e;

	for (auto& each : datalist_o)
	{
		flags_o |= numFlag[each];
	}

	for (auto& each : datalist_e)
	{
		flags_e |= numFlag[each];
	}

	if (flags_o == flags_e) data_e = data_o;

	return;
}

void paramMatch(string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, bool caseSensitive, bool flagging)
{
	if (flagging) flaggingTranslation(data_o, data_e);

	if (caseSensitive ? data_o == data_e : boost::to_lower_copy(data_o) == boost::to_lower_copy(data_e))
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, data_e));
		storeline.push_back(autoParam(base, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void paramMatch(string name, string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, bool caseSensitive, bool flagging)
{
	if (flagging) flaggingTranslation(data_o, data_e);

	if (caseSensitive ? data_o == data_e : boost::to_lower_copy(data_o) == boost::to_lower_copy(data_e))
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, name, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, name, data_e));
		storeline.push_back(autoParam(base, name, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void paramMatch(string name, usize numelements, string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, bool caseSensitive,
	bool flagging)
{
	if (flagging) flaggingTranslation(data_o, data_e);

	if (caseSensitive ? data_o == data_e : boost::to_lower_copy(data_o) == boost::to_lower_copy(data_e))
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, name, numelements, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, name, numelements, data_e));
		storeline.push_back(autoParam(base, name, numelements, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}



void paramMatch(bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(name, string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, usize numelements, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(name, numelements, string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}



void paramMatch(int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(name, to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, usize numelements, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(name, numelements, to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}



void paramMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(name, nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, usize numelements, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(name, numelements, nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}



void paramMatch(shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	paramMatch(data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited)
{
	paramMatch(name, data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}

void paramMatch(string name, usize numelements, shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, 
	bool& open, bool& isEdited)
{
	paramMatch(name, numelements, data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}



void paramMatch(coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openParam(base));
	voidMatch(data_o, data_e, output, storeline, base, false, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void paramMatch(string name, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openParam(base, name));
	voidMatch(data_o, data_e, output, storeline, base, false, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void paramMatch(string name, usize numelements, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openParam(base, name, numelements));
	voidMatch(data_o, data_e, output, storeline, base, false, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}



void paramMatch(qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openParam(base));
	voidMatch(data_o, data_e, output, storeline, base, false, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void paramMatch(string name, qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openParam(base, name));
	voidMatch(data_o, data_e, output, storeline, base, false, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}

void paramMatch(string name, usize numelements, qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	nemesis::try_close(open, output, storeline);
	output.push_back(openParam(base, name, numelements));
	voidMatch(data_o, data_e, output, storeline, base, false, open, isEdited);

	if (close) nemesis::try_close(open, output, storeline);

	output.push_back(closeParam(base));
}



void paramMatch(EventId data_o, EventId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, data_e));
		storeline.push_back(autoParam(base, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void paramMatch(string name, EventId data_o, EventId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, name, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, name, data_e));
		storeline.push_back(autoParam(base, name, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void paramMatch(string name, usize numelements, EventId data_o, EventId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, name, numelements, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, name, numelements, data_e));
		storeline.push_back(autoParam(base, name, numelements, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}



void paramMatch(VariableId data_o, VariableId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, data_e));
		storeline.push_back(autoParam(base, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void paramMatch(string name, VariableId data_o, VariableId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, name, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, name, data_e));
		storeline.push_back(autoParam(base, name, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void paramMatch(string name, usize numelements, VariableId data_o, VariableId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(autoParam(base, name, numelements, data_o));
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(autoParam(base, name, numelements, data_e));
		storeline.push_back(autoParam(base, name, numelements, data_o));
	}

	if (close) nemesis::try_close(open, output, storeline);
}

