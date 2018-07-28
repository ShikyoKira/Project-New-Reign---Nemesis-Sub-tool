#include "highestscore.h"

using namespace std;

vector<orderPair> highestScore(map<int, map<int, int>> scorelist, size_t originalSize, size_t newSize)
{
	int counter = 0;
	multimap<int, orderPair> sortedlist;
	unordered_map<int, bool> taken;
	unordered_map<int, bool> taken2;
	unordered_map<int, bool> isNew;
	unordered_map<int, bool> isDeleted;
	vector<int> added;
	vector<orderPair> editOrder;
	size_t size = max(originalSize, newSize);
	
	editOrder.reserve(size);

	for (size_t i = 0; i < size; ++i)
	{
		orderPair dummy;
		editOrder.push_back(dummy);
	}

	if (min(originalSize, newSize) != 0)
	{
		for (auto& scores : scorelist)
		{
			for (auto& score : scores.second)
			{
				orderPair dummy;
				dummy.original = scores.first;
				dummy.edited = score.first;
				sortedlist.insert(make_pair(score.second, dummy));
			}
		}

		for (auto score = sortedlist.rbegin(); score != sortedlist.rend(); ++score)
		{
			if (!taken[score->second.original] && !taken2[score->second.edited])
			{
				editOrder[score->second.original] = score->second;
				taken[score->second.original] = true;
				taken2[score->second.edited] = true;
				isDeleted[score->second.original] = false;
				isNew[score->second.edited] = false;
			}
			else if (!taken[score->second.original])
			{
				isDeleted[score->second.original] = true;
			}
			else if (!taken2[score->second.edited])
			{
				isNew[score->second.edited] = true;
			}
		}

		for (size_t i = 0; i < originalSize; ++i)
		{
			if (isDeleted[i])
			{
				orderPair dummy;
				dummy.original = i;
				dummy.edited = -1;
				editOrder[i] = dummy;
			}
		}

		for (auto& newly : isNew)
		{
			if (newly.second)
			{
				added.push_back(newly.first);
			}
		}

		if (added.size() != size - originalSize)
		{
			cout << "ERROR: Highest Score bug detected" << endl;
			Error = true;
			return editOrder;
		}

		for (size_t i = originalSize; i < size; ++i)
		{
			orderPair dummy;
			dummy.original = -1;
			dummy.edited = added[counter];
			editOrder[i] = dummy;
			++counter;
		}
	}
	else if (newSize == 0)
	{
		for (size_t i = 0; i < originalSize; ++i)
		{
			orderPair dummy;
			dummy.original = i;
			dummy.edited = -1;
			editOrder[i] = dummy;
		}
	}
	else
	{
		for (size_t i = 0; i < newSize; ++i)
		{
			orderPair dummy;
			dummy.original = -1;
			dummy.edited = i;
			editOrder[i] = dummy;
		}
	}

	return editOrder;
}

vector<orderPair> highestScore(map<int, map<int, double>> scorelist, size_t originalSize, size_t newSize)
{
	int counter = 0;
	multimap<double, orderPair> sortedlist;
	unordered_map<int, bool> taken;
	unordered_map<int, bool> taken2;
	unordered_map<int, bool> isNew;
	unordered_map<int, bool> isDeleted;
	vector<int> added;
	vector<orderPair> editOrder;
	size_t size = max(originalSize, newSize);

	editOrder.reserve(size);

	for (size_t i = 0; i < size; ++i)
	{
		orderPair dummy;
		editOrder.push_back(dummy);
	}

	for (auto& scores : scorelist)
	{
		for (auto& score : scores.second)
		{
			orderPair dummy;
			dummy.original = scores.first;
			dummy.edited = score.first;
			sortedlist.insert(make_pair(score.second, dummy));
		}
	}

	for (auto score = sortedlist.rbegin(); score != sortedlist.rend(); ++score)
	{
		if (!taken[score->second.original] && !taken2[score->second.edited])
		{
			editOrder[score->second.original] = score->second;
			taken[score->second.original] = true;
			taken2[score->second.edited] = true;
			isNew[score->second.edited] = false;
		}
		else if (!taken2[score->second.edited])
		{
			isNew[score->second.edited] = true;
		}
		else if (!taken[score->second.original])
		{
			isDeleted[score->second.original] = true;
		}
	}

	for (size_t i = 0; i < originalSize; ++i)
	{
		if (isDeleted[i])
		{
			orderPair dummy;
			dummy.original = i;
			dummy.edited = -1;
			editOrder[i] = dummy;
		}
	}

	for (auto& newly : isNew)
	{
		if (newly.second)
		{
			added.push_back(newly.first);
		}
	}

	if (added.size() != size - originalSize)
	{
		cout << "ERROR: Highest Score bug detected";
		Error = true;
		return editOrder;
	}

	for (size_t i = originalSize; i < size; ++i)
	{
		orderPair dummy;
		dummy.original = -1;
		dummy.edited = added[counter];
		editOrder[i] = dummy;
		++counter;
	}

	return editOrder;
}
