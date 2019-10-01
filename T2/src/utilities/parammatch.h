#ifndef PARAMMATCH_H_
#define PARAMMATCH_H_

#include "coordinate.h"
#include "qstransform.h"
#include "eventinfopack.h"
#include "variableinfopack.h"

void paramMatch(std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, bool caseSensitive = true,
	bool flagging = false);
void paramMatch(std::string name, std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited,
	bool caseSensitive = true, bool flagging = false);
void paramMatch(std::string name, usize numelements, std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited,
	bool caseSensitive = true, bool flagging = false);

void paramMatch(bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void paramMatch(int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void paramMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void paramMatch(std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited);
void paramMatch(std::string name, usize numelements, std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base,
	bool close, bool& open, bool& isEdited);

void paramMatch(coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void paramMatch(qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void paramMatch(EventId data_o, EventId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, EventId data_o, EventId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, EventId data_o, EventId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void paramMatch(VariableId data_o, VariableId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, VariableId data_o, VariableId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void paramMatch(std::string name, usize numelements, VariableId data_o, VariableId data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

#endif