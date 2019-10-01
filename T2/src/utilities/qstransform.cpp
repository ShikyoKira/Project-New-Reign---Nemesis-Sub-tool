#include "qstransform.h"

qstransform::qstransform(double a3a, double a3b, double a3c, double b4a, double b4b, double b4c, double b4d, double c3a, double c3b, double c3c) :
	a3(a3a, a3b, a3c), b4(b4a, b4b, b4c, b4d), c3(c3a, c3b, c3c)
{
}

bool qstransform::vector3::operator==(vector3 v3)
{
	return a == v3.a && b == v3.b && c == v3.c;
}

bool qstransform::vector3::operator!=(vector3 v3)
{
	return !(a == v3.a && b == v3.b && c == v3.c);
}

bool qstransform::vector4::operator==(vector4 v4)
{
	return a == v4.a && b == v4.b && c == v4.c && d == v4.d;
}

bool qstransform::vector4::operator!=(vector4 v4)
{
	return !(a == v4.a && b == v4.b && c == v4.c && d == v4.d);
}

bool qstransform::operator==(qstransform& n_qstransform)
{
	return a3 == n_qstransform.a3 && b4 == n_qstransform.b4 && c3 == n_qstransform.c3;
}

bool qstransform::operator!=(qstransform& n_qstransform)
{
	return !(a3 == n_qstransform.a3 && b4 == n_qstransform.b4 && c3 == n_qstransform.c3);
}
