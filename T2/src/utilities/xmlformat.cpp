#include <boost\algorithm\string.hpp>
#include "xmlformat.h"
#include "src\hkx\hkbobject.h"

using namespace std;

atomic<int> writtenFile = 0;

namespace nemesis
{
	string to_string(double num)
	{
		stringstream s_num;
		s_num << setprecision(6) << fixed << num;
		string st_num = s_num.str();

		if (num == 0 && st_num[0] == '-') return st_num.substr(1);

		return st_num;
	}
}

string space(usize base)
{
	string line;

	for (usize i = 0; i < base; ++i)
	{
		line.append("	");
	}

	return line;
}



string openObject(usize& base)
{
	++base;
	return space(base - 1) + "<hkobject>";
}

string openObject(usize& base, string name)
{
	++base;
	return space(base - 1) + "<hkobject name=\"" + name + "\">";
}

string openObject(usize& base, string name, string classname)
{
	++base;
	return space(base - 1) + "<hkobject name=\"" + name + "\" class=\"" + classname + "\">";
}

string openObject(usize& base, string name, string classname, string signature)
{
	++base;
	return space(base - 1) + "<hkobject name=\"" + name + "\" class=\"" + classname + "\" signature=\"" + signature + "\">";
}



string autoObject(usize base, string data)
{
	return openObject(base) + data + "</hkobject>";
}

string autoObject(usize base, string name, string data)
{
	return openObject(base, name) + data + "</hkobject>";
}

string autoObject(usize base, string name, string classname, string data)
{
	return openObject(base, name, classname) + data + "</hkobject>";
}

string autoObject(usize base, string name, string classname, string signature, string data)
{
	return openObject(base, name, classname, signature) + data + "</hkobject>";
}



string autoObject(usize base, bool data)
{
	return autoObject(base, string(data ? "true" : "false"));
}

string autoObject(usize base, string name, bool data)
{
	return autoObject(base, name, string(data ? "true" : "false"));
}

string autoObject(usize base, string name, string classname, bool data)
{
	return autoObject(base, name, classname, string(data ? "true" : "false"));
}

string autoObject(usize base, string name, string classname, string signature, bool data)
{
	return autoObject(base, name, classname, signature, string(data ? "true" : "false"));
}



string autoObject(usize base, int data)
{
	return autoObject(base, to_string(data));
}

string autoObject(usize base, string name, int data)
{
	return autoObject(base, name, to_string(data));
}

string autoObject(usize base, string name, string classname, int data)
{
	return autoObject(base, name, classname, to_string(data));
}

string autoObject(usize base, string name, string classname, string signature, int data)
{
	return autoObject(base, name, classname, signature, to_string(data));
}



string autoObject(usize base, double data)
{
	return autoObject(base, nemesis::to_string(data));
}

string autoObject(usize base, string name, double data)
{
	return autoObject(base, name, nemesis::to_string(data));
}

string autoObject(usize base, string name, string classname, double data)
{
	return autoObject(base, name, classname, nemesis::to_string(data));
}

string autoObject(usize base, string name, string classname, string signature, double data)
{
	return autoObject(base, name, classname, signature, nemesis::to_string(data));
}



string autoObject(usize base, shared_ptr<hkbobject> data)
{
	return autoObject(base, data ? data->ID : "null");
}

string autoObject(usize base, string name, shared_ptr<hkbobject> data)
{
	return autoObject(base, name, data ? data->ID : "null");
}

string autoObject(usize base, string name, string classname, shared_ptr<hkbobject> data)
{
	return autoObject(base, name, classname, data ? data->ID : "null");
}

string autoObject(usize base, string name, string classname, string signature, shared_ptr<hkbobject> data)
{
	return autoObject(base, name, classname, signature, data ? data->ID : "null");
}



string autoObject(usize base, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	string open = openObject(base);
	return open + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces +
		nemesis::to_string(data.W) + "\n" + closeObject(base);
}

string autoObject(usize base, string name, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	string open = openObject(base, name);
	return open + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces +
		nemesis::to_string(data.W) + "\n" + closeObject(base);
}

string autoObject(usize base, string name, string classname, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	string open = openObject(base, name, classname);
	return open + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces +
		nemesis::to_string(data.W) + "\n" + closeObject(base);
}

string autoObject(usize base, string name, string classname, string signature, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	return openObject(base, name, classname, signature) + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) +
		spaces + nemesis::to_string(data.W) + "\n" + closeObject(base);
}



string autoObject(usize base, EventId data)
{
	return autoObject(base, data.getID());
}

string autoObject(usize base, string name, EventId data)
{
	return autoObject(base, name, data.getID());
}

string autoObject(usize base, string name, string classname, EventId data)
{
	return autoObject(base, name, classname, data.getID());
}

string autoObject(usize base, string name, string classname, string signature, EventId data)
{
	return autoObject(base, name, classname, signature, data.getID());
}



string autoObject(usize base, VariableId data)
{
	return autoObject(base, data.getID());
}

string autoObject(usize base, string name, VariableId data)
{
	return autoObject(base, name, data.getID());
}

string autoObject(usize base, string name, string classname, VariableId data)
{
	return autoObject(base, name, classname, data.getID());
}

string autoObject(usize base, string name, string classname, string signature, VariableId data)
{
	return autoObject(base, name, classname, signature, data.getID());
}



string closeObject(usize& base)
{
	--base;
	return space(base) + "</hkobject>";
}

string closeObject()
{
	return "</hkobject>";
}



string openParam(usize& base)
{
	++base;
	return space(base - 1) + "<hkparam>";
}

string openParam(usize& base, string name)
{
	++base;
	return space(base - 1) + "<hkparam name=\"" + name + "\">";
}

string openParam(usize& base, string name, usize numelements)
{
	++base;
	return space(base - 1) + "<hkparam name=\"" + name + "\" numelements=\"" + to_string(numelements) + "\">";
}



string autoParam(usize base, string data)
{
	return openParam(base) + data + closeParam();
}

string autoParam(usize base, string name, string data)
{
	return openParam(base, name) + data + closeParam();
}

string autoParam(usize base, string name, usize numelements, string data)
{
	return openParam(base, name, numelements) + data + closeParam();
}



string autoParam(usize base, bool data)
{
	return openParam(base) + string(data ? "true" : "false") + closeParam();
}

string autoParam(usize base, string name, bool data)
{
	return openParam(base, name) + string(data ? "true" : "false") + closeParam();
}

string autoParam(usize base, string name, usize numelements, bool data)
{
	return openParam(base, name, numelements) + string(data ? "true" : "false") + closeParam();
}



string autoParam(usize base, int data)
{
	return openParam(base) + to_string(data) + closeParam();
}

string autoParam(usize base, string name, int data)
{
	return openParam(base, name) + to_string(data) + closeParam();
}

string autoParam(usize base, string name, usize numelements, int data)
{
	return openParam(base, name, numelements) + to_string(data) + closeParam();
}



string autoParam(usize base, double data)
{
	return openParam(base) + nemesis::to_string(data) + closeParam();
}

string autoParam(usize base, string name, double data)
{
	return openParam(base, name) + nemesis::to_string(data) + closeParam();
}

string autoParam(usize base, string name, usize numelements, double data)
{
	return openParam(base, name, numelements) + nemesis::to_string(data) + closeParam();
}



string autoParam(usize base, shared_ptr<hkbobject> data)
{
	return openParam(base) + (data ? data->ID : "null") + closeParam();
}

string autoParam(usize base, string name, shared_ptr<hkbobject> data)
{
	return openParam(base, name) + (data ? data->ID : "null") + closeParam();
}

string autoParam(usize base, string name, usize numelements, shared_ptr<hkbobject> data)
{
	return openParam(base, name, numelements) + (data ? data->ID : "null") + closeParam();
}



string autoParam(usize base, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	string open = openParam(base);
	return open + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces +
		nemesis::to_string(data.W) + "\n" + closeParam(base);
}

string autoParam(usize base, string name, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	string open = openParam(base, name);
	return open + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces +
		nemesis::to_string(data.W) + "\n" + closeParam(base);
}

string autoParam(usize base, string name, usize numelements, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	string open = openParam(base, name, numelements);
	return open + spaces + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces +
		nemesis::to_string(data.W) + "\n" + closeParam(base);
}



string autoParam(usize base, qstransform data)
{
	string spaces = "\n" + space(base + 1);
	string open = openParam(base);
	return open + spaces + nemesis::to_string(data.a3.a) + spaces + nemesis::to_string(data.a3.b) + spaces + nemesis::to_string(data.a3.c) + spaces +
		nemesis::to_string(data.b4.a) + spaces + nemesis::to_string(data.b4.b) + spaces + nemesis::to_string(data.b4.c) + spaces + nemesis::to_string(data.b4.d) + spaces +
		nemesis::to_string(data.c3.a) + spaces + nemesis::to_string(data.c3.b) + spaces + nemesis::to_string(data.c3.c) + "\n" + closeParam(base);
}

string autoParam(usize base, string name, qstransform data)
{
	string spaces = "\n" + space(base + 1);
	string open = openParam(base, name);
	return open + spaces + nemesis::to_string(data.a3.a) + spaces + nemesis::to_string(data.a3.b) + spaces + nemesis::to_string(data.a3.c) + spaces +
		nemesis::to_string(data.b4.a) + spaces + nemesis::to_string(data.b4.b) + spaces + nemesis::to_string(data.b4.c) + spaces + nemesis::to_string(data.b4.d) + spaces +
		nemesis::to_string(data.c3.a) + spaces + nemesis::to_string(data.c3.b) + spaces + nemesis::to_string(data.c3.c) + "\n" + closeParam(base);
}

string autoParam(usize base, string name, usize numelements, qstransform data)
{
	string spaces = "\n" + space(base + 1);
	string open = openParam(base, name, numelements);
	return open + spaces + nemesis::to_string(data.a3.a) + spaces + nemesis::to_string(data.a3.b) + spaces + nemesis::to_string(data.a3.c) + spaces +
		nemesis::to_string(data.b4.a) + spaces + nemesis::to_string(data.b4.b) + spaces + nemesis::to_string(data.b4.c) + spaces + nemesis::to_string(data.b4.d) + spaces +
		nemesis::to_string(data.c3.a) + spaces + nemesis::to_string(data.c3.b) + spaces + nemesis::to_string(data.c3.c) + "\n" + closeParam(base);
}



string autoParam(usize base, EventId data)
{
	return openParam(base) + data.getID() + closeParam();
}

string autoParam(usize base, string name, EventId data)
{
	return openParam(base, name) + data.getID() + closeParam();
}

string autoParam(usize base, string name, usize numelements, EventId data)
{
	return openParam(base, name, numelements) + data.getID() + closeParam();
}



string autoParam(usize base, VariableId data)
{
	return openParam(base) + data.getID() + closeParam();
}

string autoParam(usize base, string name, VariableId data)
{
	return openParam(base, name) + data.getID() + closeParam();
}

string autoParam(usize base, string name, usize numelements, VariableId data)
{
	return openParam(base, name, numelements) + data.getID() + closeParam();
}



string closeParam(usize& base)
{
	--base;
	return space(base) + closeParam();
}

string closeParam()
{
	return "</hkparam>";
}



string openString(usize& base)
{
	++base;
	return space(base - 1) + "<hkcstring>";
}



string autoString(usize base, string data)
{
	return openString(base) + data + "</hkcstring>";
}

string autoString(usize base, int data)
{
	return openString(base) + to_string(data) + "</hkcstring>";
}

string autoString(usize base, double data)
{
	return openString(base) + nemesis::to_string(data) + "</hkcstring>";
}

string autoString(usize base, shared_ptr<hkbobject> data)
{
	return openString(base) + data->ID + "</hkcstring>";
}



std::string closeString(usize& base)
{
	--base;
	return space(base) + "</hkcstring>";
}



string openVoid(usize& base)
{
	++base;
	return space(base - 1);
}



string autoVoid(usize base, string data)
{
	return openVoid(base) + data;
}

string autoVoid(usize base, int data)
{
	return openVoid(base) + to_string(data);
}

string autoVoid(usize base, double data)
{
	return openVoid(base) + nemesis::to_string(data);
}

string autoVoid(usize base, shared_ptr<hkbobject> data)
{
	return openVoid(base) + data->ID;
}

string autoVoid(usize base, coordinate data)
{
	string spaces = "\n" + space(base + 1);
	return openVoid(base) + nemesis::to_string(data.X) + spaces + nemesis::to_string(data.Y) + spaces + nemesis::to_string(data.Z) + spaces + nemesis::to_string(data.W);
}

string autoVoid(usize base, EventId data)
{
	return openVoid(base) + data.getID();
}

string autoVoid(usize base, VariableId data)
{
	return openVoid(base) + data.getID();
}



string closeVoid(usize& base)
{
	--base;
	return space(base);
}



bool readParam(string key, string line, string& output)
{
	string search = "<hkparam name=\"" + key + "\">";
	usize pos = line.find(search);

	if (pos == string::npos) return false;

	pos += search.length();
	usize pos2 = line.find(closeParam(), pos);

	if (pos2 == string::npos) return false;

	output = line.substr(pos, pos2 - pos);
	return true;
}

bool readParam(string key, string line, bool& output)
{
	string s_output;

	if (!readParam(key, line, s_output)) return false;

	output = s_output == "true";
	return true;
}

bool readParam(string key, string line, int& output)
{
	string s_output;

	if (!readParam(key, line, s_output)) return false;

	output = stoi(s_output);
	return true;
}

bool readParam(string key, string line, usize& output)
{
	string s_output;

	if (!readParam(key, line, s_output)) return false;

	output = stoi(s_output);
	return true;
}

bool readParam(string key, string line, double& output)
{
	string s_output;

	if (!readParam(key, line, s_output)) return false;

	output = stod(s_output);
	return true;
}

bool readParam(string key, string line, coordinate& output)
{
	string s_output;

	if (!readParam(key, line, s_output)) return false;

	output.update(s_output);
	return true;
}

bool readParam(string line, vector<double>& output)
{
	string s_output;
	output = vector<double>();

	vector<string> tokens;
	boost::trim_if(line, boost::is_any_of("\t ()"));
	boost::split(tokens, line, boost::is_any_of("\t ()"), boost::token_compress_on);
	output.reserve(tokens.size());

	for (auto& token : tokens)
	{
		output.push_back(stod(token));
	}

	return true;
}

bool readParam(string key, string line, EventId& output)
{
	int id;

	if (!readParam(key, line, id)) return false;

	output = EventId(id);
	return true;
}

bool readParam(string key, string line, VariableId& output)
{
	int id;

	if (!readParam(key, line, id)) return false;

	output = VariableId(id);
	return true;
}

bool readEleParam(string key, string line, usize& numelement)
{
	string search = "<hkparam name=\"" + key + "\" numelements=\"";
	usize pos = line.find(search);

	if (pos != string::npos)
	{
		pos += search.length();
		numelement = stoi(line.substr(pos, line.find("\">", pos) - pos));
		return true;
	}

	return false;
}



bool getNodeRefList(std::string line, vecstr& list)
{
	list = vecstr();

	if (line.find("\t#") == string::npos) return false;

	boost::trim_if(line, boost::is_any_of("\t "));
	boost::split(list, line, boost::is_any_of("\t "), boost::token_compress_on);

	for (auto each : list)
	{
		if (each[0] != '#') return false;
	}

	return true;
}



void outputExtraction(string filename, string classname, vecstr& output, bool isEdited)
{
	if (isEdited)
	{
		++writtenFile;
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			for (auto& line : output)
			{
				outputfile << line << "\n";
			}

			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit " + classname + " Output Not Found (File: " << filename << ")" << endl;
			Error = true;
			throw 5;
		}

		if (!IsFileExist(filename))
		{
			cout << "ERROR: Edit " + classname + " Output Not Found (File: " << filename << ")" << endl;
			Error = true;
			throw 5;
		}
	}
	else
	{
		if (IsFileExist(filename))
		{
			if (remove(filename.c_str()) != 0)
			{
				perror("Error deleting file");
				Error = true;
				throw 5;
			}
		}
	}
}
