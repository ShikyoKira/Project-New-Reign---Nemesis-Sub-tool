#include <iostream>
#include <boost\algorithm\string.hpp>
#include "levenshteindistance.h"

using namespace std;

usize str_diff(string str1, string str2)
{
	boost::to_lower(str1);
	boost::to_lower(str2);
	size_t m = str1.size();
	size_t n = str2.size();

	if (m == 0) return n;
	if (n == 0) return m;

	size_t *costs = new size_t[n + 1];

	for (size_t k = 0; k <= n; k++)
	{
		costs[k] = k;
	}

	size_t i = 0;

	for (std::string::const_iterator it1 = str1.begin(); it1 != str1.end(); ++it1, ++i)
	{
		costs[0] = i + 1;
		size_t corner = i;
		size_t j = 0;

		for (std::string::const_iterator it2 = str2.begin(); it2 != str2.end(); ++it2, ++j)
		{
			size_t upper = costs[j + 1];

			if (*it1 == *it2)
			{
				costs[j + 1] = corner;
			}
			else
			{
				size_t t(upper<corner ? upper : corner);
				costs[j + 1] = (costs[j]<t ? costs[j] : t) + 1;
			}

			corner = upper;
		}
	}

	size_t result = costs[n];
	delete[] costs;
	return result;
}

double str_similarity(string& str1, string& str2)
{
	return str1.length() >= str2.length() ? double(double(str1.length() - str_diff(str1, str2)) / str1.length()) :
		double(double(str2.length() - str_diff(str1, str2)) / str2.length());
}
