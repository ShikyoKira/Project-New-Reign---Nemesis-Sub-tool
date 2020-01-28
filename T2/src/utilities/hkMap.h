#pragma once

#include <unordered_map>

template<typename T, typename K>
class hkMap : public std::unordered_map<T, K>
{
public:
    inline hkMap() {}
    inline hkMap(std::initializer_list<std::pair<T, K>> list)
    {
        for (typename std::initializer_list<std::pair<T, K>>::const_iterator it = list.begin(); it != list.end(); ++it)
            insert(it->first, it->second);
    }

    hkMap<T, K>& operator=(std::initializer_list<std::pair<T, K>> list);
	K& operator[](T& key);
	T& operator[](K& key);
};
