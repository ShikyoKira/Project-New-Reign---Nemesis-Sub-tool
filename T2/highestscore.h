#ifndef HIGHEST_SCORE_H_
#define HIGHEST_SCORE_H_

#include "Global.h"

struct orderPair
{
	int original = -1;
	int edited = -1;

	orderPair() {}
	orderPair(int ori, int edit);
};

bool matchIDCompare(std::shared_ptr<hkbobject> oldNode, std::shared_ptr<hkbobject> newNode);

std::vector<orderPair> highestScore(std::map<int, std::map<int, int>> scorelist, size_t originalSize, size_t newSize);
std::vector<orderPair> highestScore(std::map<int, std::map<int, double>> scorelist, size_t originalSize, size_t newSize);

#endif
