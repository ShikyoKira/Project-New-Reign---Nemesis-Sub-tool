#ifndef GLOBALADDON_H_
#define GLOBALADDON_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

extern void ReferenceReplacementExt(std::string wrongReference, std::string rightReference); // replacement function for foreign principle

extern void ReferenceReplacement(std::string wrongReference, std::string rightReference);

extern void NemesisReaderFormat(std::vector<std::string>& output, bool hasID = false);

#endif