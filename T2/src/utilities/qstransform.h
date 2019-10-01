#ifndef QSTRANSFORM_H_
#define QSTRANSFORM_H_

struct qstransform
{
	struct vector3
	{
		double a;
		double b;
		double c;

		vector3() {}
		vector3(double n_a, double n_b, double n_c) : a(n_a), b(n_b), c(n_c) {}

		bool operator==(vector3 v3);
		bool operator!=(vector3 v3);
	};

	struct vector4
	{
		double a;
		double b;
		double c;
		double d;

		vector4() {}
		vector4(double n_a, double n_b, double n_c, double n_d) : a(n_a), b(n_b), c(n_c), d(n_d) {}

		bool operator==(vector4 v4);
		bool operator!=(vector4 v4);
	};

	vector3 a3;
	vector4 b4;
	vector3 c3;

	qstransform() {}
	qstransform(vector3 n_a3, vector4 n_b4, vector3 n_c3) : a3(n_a3), b4(n_b4), c3(n_c3) {}
	qstransform(double a3a, double a3b, double a3c, double b4a, double b4b, double b4c, double b4d, double c3a, double c3b, double c3c);

	bool operator==(qstransform& n_qstransform);
	bool operator!=(qstransform& n_qstransform);
};

#endif
