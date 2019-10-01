#ifndef DATAMATCH_H_
#define DATAMATCH_H_

#include "Global.h"
#include "src\utilities\coordinate.h"
#include "src\utilities\qstransform.h"

void voidMatch(std::string data_o, std::string data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited, std::string space = "");
void voidMatch(bool data_o, bool data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void voidMatch(int data_o, int data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void voidMatch(double data_o, double data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void voidMatch(std::shared_ptr<hkbobject> data_o, std::shared_ptr<hkbobject> data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void voidMatch(coordinate data_o, coordinate data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);
void voidMatch(qstransform data_o, qstransform data_e, vecstr& output, vecstr& storeline, usize& base, bool close, bool& open, bool& isEdited);

#endif