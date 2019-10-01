#ifndef HKBVARIABLEVALUE_H_
#define HKBVARIABLEVALUE_H_

struct hkbvariablevalue
{
	bool proxy;

	int value;

	hkbvariablevalue() : proxy(true) {}
	hkbvariablevalue(int n_value) : value(n_value), proxy(false) {}
};

#endif
