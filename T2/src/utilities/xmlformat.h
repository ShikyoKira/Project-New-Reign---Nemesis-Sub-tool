#ifndef XMLFORMAT_H_
#define XMLFORMAT_H_

#include "Global.h"
#include "voidmatch.h"
#include "parammatch.h"
#include "objectmatch.h"
#include "stringmatch.h"
#include "nemesisformat.h"
#include "eventinfopack.h"
#include "variableinfopack.h"

namespace nemesis
{
	std::string to_string(double num);
}

// open object
std::string openObject(usize& base);
std::string openObject(usize& base, std::string name);
std::string openObject(usize& base, std::string name, std::string classname);
std::string openObject(usize& base, std::string name, std::string classname, std::string signature);

// auto object
std::string autoObject(usize base, std::string data);
std::string autoObject(usize base, std::string name, std::string data);
std::string autoObject(usize base, std::string name, std::string classname, std::string data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, std::string data);

std::string autoObject(usize base, bool data);
std::string autoObject(usize base, std::string name, bool data);
std::string autoObject(usize base, std::string name, std::string classname, bool data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, bool data);

std::string autoObject(usize base, int data);
std::string autoObject(usize base, std::string name, int data);
std::string autoObject(usize base, std::string name, std::string classname, int data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, int data);

std::string autoObject(usize base, double data);
std::string autoObject(usize base, std::string name, double data);
std::string autoObject(usize base, std::string name, std::string classname, double data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, double data);

std::string autoObject(usize base, std::shared_ptr<hkbobject> data);
std::string autoObject(usize base, std::string name, std::shared_ptr<hkbobject> data);
std::string autoObject(usize base, std::string name, std::string classname, std::shared_ptr<hkbobject> data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, std::shared_ptr<hkbobject> data);

std::string autoObject(usize base, coordinate data);
std::string autoObject(usize base, std::string name, coordinate data);
std::string autoObject(usize base, std::string name, std::string classname, coordinate data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, coordinate data);

std::string autoObject(usize base, EventId data);
std::string autoObject(usize base, std::string name, EventId data);
std::string autoObject(usize base, std::string name, std::string classname, EventId data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, EventId data);

std::string autoObject(usize base, VariableId data);
std::string autoObject(usize base, std::string name, VariableId data);
std::string autoObject(usize base, std::string name, std::string classname, VariableId data);
std::string autoObject(usize base, std::string name, std::string classname, std::string signature, VariableId data);

// close object
std::string closeObject(usize& base);
std::string closeObject();

// open param
std::string openParam(usize& base);
std::string openParam(usize& base, std::string name);
std::string openParam(usize& base, std::string name, usize numelements);

// auto param
std::string autoParam(usize base, std::string data);
std::string autoParam(usize base, std::string name, std::string data);
std::string autoParam(usize base, std::string name, usize numelements, std::string data);

std::string autoParam(usize base, bool data);
std::string autoParam(usize base, std::string name, bool data);
std::string autoParam(usize base, std::string name, usize numelements, bool data);

std::string autoParam(usize base, int data);
std::string autoParam(usize base, std::string name, int data);
std::string autoParam(usize base, std::string name, usize numelements, int data);

std::string autoParam(usize base, double data);
std::string autoParam(usize base, std::string name, double data);
std::string autoParam(usize base, std::string name, usize numelements, double data);

std::string autoParam(usize base, std::shared_ptr<hkbobject> data);
std::string autoParam(usize base, std::string name, std::shared_ptr<hkbobject> data);
std::string autoParam(usize base, std::string name, usize numelements, std::shared_ptr<hkbobject> data);

std::string autoParam(usize base, coordinate data);
std::string autoParam(usize base, std::string name, coordinate data);
std::string autoParam(usize base, std::string name, usize numelements, coordinate data);

std::string autoParam(usize base, qstransform data);
std::string autoParam(usize base, std::string name, qstransform data);
std::string autoParam(usize base, std::string name, usize numelements, qstransform data);

std::string autoParam(usize base, EventId data);
std::string autoParam(usize base, std::string name, EventId data);
std::string autoParam(usize base, std::string name, usize numelements, EventId data);

std::string autoParam(usize base, VariableId data);
std::string autoParam(usize base, std::string name, VariableId data);
std::string autoParam(usize base, std::string name, usize numelements, VariableId data);

// close param
std::string closeParam(usize& base);
std::string closeParam();

// open string
std::string openString(usize& base);

// auto string
std::string autoString(usize base, std::string data);
std::string autoString(usize base, int data);
std::string autoString(usize base, double data);
std::string autoString(usize base, std::shared_ptr<hkbobject> data);

// close string
std::string closeString(usize& base);

// open void
std::string openVoid(usize& base);

// auto void
std::string autoVoid(usize base, std::string data);
std::string autoVoid(usize base, int data);
std::string autoVoid(usize base, double data);
std::string autoVoid(usize base, std::shared_ptr<hkbobject> data);
std::string autoVoid(usize base, coordinate data);
std::string autoVoid(usize base, EventId data);
std::string autoVoid(usize base, VariableId data);

// close void
std::string closeVoid(usize& base);

// read param
bool readParam(std::string key, std::string line, std::string& output);
bool readParam(std::string key, std::string line, bool& output);
bool readParam(std::string key, std::string line, int& output);
bool readParam(std::string key, std::string line, usize& output);
bool readParam(std::string key, std::string line, double& output);
bool readParam(std::string key, std::string line, coordinate& output);
bool readParam(std::string line, std::vector<double>& output);
bool readParam(std::string key, std::string line, EventId& output);
bool readParam(std::string key, std::string line, VariableId& output);
bool readEleParam(std::string key, std::string line, usize& numelement);

// node ref list
bool getNodeRefList(std::string line, vecstr& list);

void outputExtraction(std::string filename, std::string classname, vecstr& output, bool isEdited);

#endif
