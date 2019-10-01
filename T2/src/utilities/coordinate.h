#ifndef COORDINATE_H_
#define COORDINATE_H_

#include "Global-Type.h"

struct coordinate
{
	double X;
	double Y;
	double Z;
	double W;

	coordinate() {}
	coordinate(double n_X, double n_Y, double n_Z, double n_W) : X(n_X), Y(n_Y), Z(n_Z), W(n_W) {}
	coordinate(std::string n_X, std::string n_Y, std::string n_Z, std::string n_W) : X(stod(n_X)), Y(stod(n_Y)), Z(stod(n_Z)), W(stod(n_W)) {}

	bool operator==(coordinate& ctrpart);
	bool operator!=(coordinate& ctrpart);
	void update(std::string line);
	std::string getString();
	std::string getString(usize base);
};

#endif