#ifndef OBJECTMATCH_H_
#define OBJECTMATCH_H_

#include "coordinate.h"

void objectMatch(std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string signature, std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close,
	bool& open, bool& isEdited);

void objectMatch(bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string signature, bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited);

void objectMatch(int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string signature, int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited);

void objectMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string signature, double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close,
	bool& open, bool& isEdited);

void objectMatch(std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open,
	bool& isEdited);
void objectMatch(std::string name, std::string classname, std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base,
	bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string signature, std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output,
	vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

void objectMatch(coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void objectMatch(std::string name, std::string classname, std::string signature, coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close,
	bool& open, bool& isEdited);

#endif