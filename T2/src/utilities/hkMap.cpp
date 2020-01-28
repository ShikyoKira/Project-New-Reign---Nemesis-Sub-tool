#include "hkMap.h"

using namespace std;

template<typename T, typename K>
inline T& hkMap<T, K>::operator[](K& key)
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

template<typename T, typename K>
hkMap<T, K>& hkMap<T, K>::operator=(std::initializer_list<std::pair<T, K>> list)
{
	for (typename std::initializer_list<std::pair<T, K>>::const_iterator it = list.begin(); it != list.end(); ++it)
		insert(it->first, it->second);

	return *this;
}

template<typename T, typename K>
inline K& hkMap<T, K>::operator[](T& key)
{
	auto cur = find(key);
	return cur == end() ? begin() : cur.second;
}
