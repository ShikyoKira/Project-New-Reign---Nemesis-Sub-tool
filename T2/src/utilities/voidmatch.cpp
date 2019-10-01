#include "xmlformat.h"
#include "src\hkx\hkbobject.h"
#include "src\utilities\coordinate.h"

using namespace std;

string space(usize base);

void voidMatch(string data_o, string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, string spaces)
{
	if(spaces.length() == 0) spaces = space(base);

	if (data_o == data_e)
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(spaces + data_o);
	}
	else
	{
		nemesis::try_open(open, isEdited, output);
		output.push_back(spaces + data_e);
		storeline.push_back(spaces + data_o);
	}

	if (close) nemesis::try_close(open, output, storeline);
}

void voidMatch(bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	voidMatch(string(data_o ? "true" : "false"), string(data_e ? "true" : "false"), output, storeline, base, close, open, isEdited);
}

void voidMatch(int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	voidMatch(to_string(data_o), to_string(data_e), output, storeline, base, close, open, isEdited);
}

void voidMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	voidMatch(nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited);
}

void voidMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, string space)
{
	voidMatch(nemesis::to_string(data_o), nemesis::to_string(data_e), output, storeline, base, close, open, isEdited, space);
}

void voidMatch(shared_ptr<hkbobject> data_o, shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	voidMatch(data_o ? data_o->ID : "null", data_e ? data_e->ID : "null", output, storeline, base, close, open, isEdited);
}

void voidMatch(coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	string spaces = space(base);
	voidMatch(data_o.X, data_e.X, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.Y, data_e.Y, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.Z, data_e.Z, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.W, data_e.W, output, storeline, base, close, open, isEdited, spaces);
}

void voidMatch(qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited)
{
	string spaces = space(base);
	voidMatch(data_o.a3.a, data_e.a3.a, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.a3.b, data_e.a3.b, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.a3.c, data_e.a3.c, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.b4.a, data_e.b4.a, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.b4.b, data_e.b4.b, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.b4.c, data_e.b4.c, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.b4.d, data_e.b4.d, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.c3.a, data_e.c3.a, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.c3.b, data_e.c3.b, output, storeline, base, close, open, isEdited, spaces);
	voidMatch(data_o.c3.c, data_e.c3.c, output, storeline, base, close, open, isEdited, spaces);
}
