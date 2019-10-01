#include "highestscore.h"
#include "src\hkx\hkbobject.h"

using namespace std;

orderPair::orderPair(int ori, int edit)
{
	original = ori;
	edited = edit;
}

bool matchIDCompare(shared_ptr<hkbobject> oldNode, shared_ptr<hkbobject> newNode)
{
	if (oldNode != nullptr)
	{
		if (newNode != nullptr && oldNode->ID == newNode->ID)
		{
			return true;
		}
	}
	else if (newNode == nullptr)
	{
		return true;
	}

	return false;
}

vector<orderPair> highestScore(map<int, map<int, int>> scorelist, size_t originalSize, size_t newSize)
{
	multimap<int, orderPair> sortedlist;
	vector<int> isNew;
	vector<int> isDeleted;
	set<int> newTaken;
	set<int> deleteTaken;
	set<int> taken;
	set<int> taken2;
	vector<orderPair> editOrder;
	size_t size = max(originalSize, newSize);
	
	editOrder.reserve(size);

	for (usize i = 0; i < size; ++i)
	{
		editOrder.push_back(orderPair());
	}

	if (min(originalSize, newSize) != 0)
	{
		for (auto& scores : scorelist)
		{
			for (auto& score : scores.second)
			{
				sortedlist.insert(make_pair(score.second, orderPair(scores.first, score.first)));
			}
		}

		multimap<int, orderPair>::reverse_iterator mark;

		for (auto score = sortedlist.rbegin(); score != sortedlist.rend(); ++score)
		{
			auto i_taken = taken.find(score->second.original);
			auto i_taken2 = taken2.find(score->second.edited);

			if (i_taken == taken.end() && i_taken2 == taken2.end())
			{
				editOrder[score->second.original] = score->second;
				taken.insert(score->second.original);
				taken2.insert(score->second.edited);
				isDeleted.erase(remove(isDeleted.begin(), isDeleted.end(), score->second.original), isDeleted.end());
				isNew.erase(remove(isNew.begin(), isNew.end(), score->second.edited), isNew.end());
			}
			else if (i_taken == taken.end() && deleteTaken.find(score->second.original) == deleteTaken.end())
			{
				isDeleted.push_back(score->second.original);
				deleteTaken.insert(score->second.original);
			}
			else if (i_taken2 == taken2.end() && newTaken.find(score->second.edited) == newTaken.end())
			{
				isNew.push_back(score->second.edited);
				newTaken.insert(score->second.edited);
			}
			else if (taken.size() + deleteTaken.size() == originalSize && taken2.size() + isNew.size() == newSize)
			{
				mark = ++score;
				break;
			}
		}

		for (auto& each : isDeleted)
		{
			editOrder[each].original = each;
			editOrder[each].edited = -1;
		}

		if (isNew.size() != size - originalSize)
		{
			for (auto score = mark; score != sortedlist.rend(); ++score)
			{
				auto i_taken = taken.find(score->second.original);
				auto i_taken2 = taken2.find(score->second.edited);

				if (i_taken == taken.end() && i_taken2 == taken2.end())
				{
					editOrder[score->second.original] = score->second;
					taken.insert(score->second.original);
					taken2.insert(score->second.edited);
					isDeleted.erase(remove(isDeleted.begin(), isDeleted.end(), score->second.original), isDeleted.end());
					isNew.erase(remove(isNew.begin(), isNew.end(), score->second.edited), isNew.end());
				}
				else if (i_taken == taken.end() && deleteTaken.find(score->second.original) == deleteTaken.end())
				{
					isDeleted.push_back(score->second.original);
					deleteTaken.insert(score->second.original);
				}
				else if (i_taken2 == taken2.end() && newTaken.find(score->second.edited) == newTaken.end())
				{
					isNew.push_back(score->second.edited);
					newTaken.insert(score->second.edited);
				}
			}

			if (isNew.size() != size - originalSize)
			{
				cout << "ERROR: Highest Score bug detected. Extra size: " << isNew.size() << ", New size: " << size << ", Original size: " << originalSize << endl;
				Error = true;
				throw 5;
			}
		}

		int counter = 0;

		for (size_t i = originalSize; i < size; ++i)
		{
			editOrder[i] = orderPair(-1, isNew[counter]);
			++counter;
		}
	}
	else if (newSize == 0)
	{
		for (size_t i = 0; i < originalSize; ++i)
		{
			editOrder[i] = orderPair(i, -1);
		}
	}
	else
	{
		for (size_t i = 0; i < newSize; ++i)
		{
			editOrder[i] = orderPair(-1, i);
		}
	}

	return editOrder;
}

vector<orderPair> highestScore(map<int, map<int, double>> scorelist, size_t originalSize, size_t newSize)
{
	multimap<double, orderPair> sortedlist;
	vector<int> isNew;
	vector<int> isDeleted;
	set<int> newTaken;
	set<int> deleteTaken;
	set<int> taken;
	set<int> taken2;
	vector<orderPair> editOrder;
	size_t size = max(originalSize, newSize);

	editOrder.reserve(size);

	for (size_t i = 0; i < size; ++i)
	{
		editOrder.push_back(orderPair());
	}

	if (min(originalSize, newSize) != 0)
	{
		for (auto& scores : scorelist)
		{
			for (auto& score : scores.second)
			{
				sortedlist.insert(make_pair(score.second, orderPair(scores.first, score.first)));
			}
		}

		multimap<double, orderPair>::reverse_iterator mark;

		for (auto score = sortedlist.rbegin(); score != sortedlist.rend(); ++score)
		{
			auto i_taken = taken.find(score->second.original);
			auto i_taken2 = taken2.find(score->second.edited);

			if (i_taken == taken.end() && i_taken2 == taken2.end())
			{
				editOrder[score->second.original] = score->second;
				taken.insert(score->second.original);
				taken2.insert(score->second.edited);
				isDeleted.erase(remove(isDeleted.begin(), isDeleted.end(), score->second.original), isDeleted.end());
				isNew.erase(remove(isNew.begin(), isNew.end(), score->second.edited), isNew.end());
			}
			else if (i_taken == taken.end() && deleteTaken.find(score->second.original) == deleteTaken.end())
			{
				isDeleted.push_back(score->second.original);
				deleteTaken.insert(score->second.original);
			}
			else if (i_taken2 == taken2.end() && newTaken.find(score->second.edited) == newTaken.end())
			{
				isNew.push_back(score->second.edited);
				newTaken.insert(score->second.edited);
			}
			else if (taken.size() + deleteTaken.size() == originalSize && taken2.size() + isNew.size() == newSize)
			{
				mark = ++score;
				break;
			}
		}

		for (auto& each : isDeleted)
		{
			editOrder[each].original = each;
			editOrder[each].edited = -1;
		}

		if (isNew.size() != size - originalSize)
		{
			for (auto score = mark; score != sortedlist.rend(); ++score)
			{
				auto i_taken = taken.find(score->second.original);
				auto i_taken2 = taken2.find(score->second.edited);

				if (i_taken == taken.end() && i_taken2 == taken2.end())
				{
					editOrder[score->second.original] = score->second;
					taken.insert(score->second.original);
					taken2.insert(score->second.edited);
					isDeleted.erase(remove(isDeleted.begin(), isDeleted.end(), score->second.original), isDeleted.end());
					isNew.erase(remove(isNew.begin(), isNew.end(), score->second.edited), isNew.end());
				}
				else if (i_taken == taken.end() && deleteTaken.find(score->second.original) == deleteTaken.end())
				{
					isDeleted.push_back(score->second.original);
					deleteTaken.insert(score->second.original);
				}
				else if (i_taken2 == taken2.end() && newTaken.find(score->second.edited) == newTaken.end())
				{
					isNew.push_back(score->second.edited);
					newTaken.insert(score->second.edited);
				}
			}

			if (isNew.size() != size - originalSize)
			{
				cout << "ERROR: Highest Score bug detected. Extra size: " << isNew.size() << ", New size: " << size << ", Original size: " << originalSize << endl;
				Error = true;
				throw 5;
			}
		}

		int counter = 0;

		for (size_t i = originalSize; i < size; ++i)
		{
			editOrder[i].original = -1;
			editOrder[i].edited = isNew[counter];
			++counter;
		}
	}
	else if (newSize == 0)
	{
		for (size_t i = 0; i < originalSize; ++i)
		{
			editOrder[i] = orderPair(i, -1);
		}
	}
	else
	{
		for (size_t i = 0; i < newSize; ++i)
		{
			editOrder[i] = orderPair(-1, i);
		}
	}

	return editOrder;
}
