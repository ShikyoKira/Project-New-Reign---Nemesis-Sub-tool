#pragma once

#include <vector>

template<typename K, typename V>
class hkMap : public std::vector<std::pair<K, V>>
{
public:
    hkMap() {}
	hkMap(std::initializer_list<std::pair<K, V>> list) : std::vector<std::pair<K, V>>()
	{
		for (std::pair<K, V> it : list)
		{
			std::vector<std::pair<K, V>>::push_back(it);
		}
	}

	inline hkMap<K, V>& operator=(std::initializer_list<std::pair<K, V>> list)
	{
		std::vector<std::pair<K, V>>::clear();

		for (std::pair<K, V> it : list)
		{
			std::vector<std::pair<K, V>>::push_back(it);
		}

		return *this;
	}

	inline V& operator[](K& key)
	{
		for (auto& each : *this)
		{
			if (each.first == key)
			{
				return each.second;
			}
		}

		return std::vector<std::pair<K, V>>::begin()->second;
	}

	inline K operator[](V& key)
	{
		for (auto each : *this)
		{
			if (each.second == key)
			{
				return each.first;
			}
		}

		throw "hkMap : Key not found";
	}

};
