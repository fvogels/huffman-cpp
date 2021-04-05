#ifndef DEFS_H
#define DEFS_H

#include <cstdint>
#include <vector>
#include <deque>


typedef uint8_t    byte;
typedef uint64_t   u64;
typedef uint64_t   Datum; // TODO Remove?

template<typename T>
void add_range(std::deque<T>& vector, const T& start, const T& end)
{
    T current = start;

    while (current != end)
    {
        vector.push_back(current);
        ++current;
    }
}

template<typename T>
unsigned find_index(const std::deque<T>& vector, const T& elt)
{
    unsigned index = 0;

    while (index < vector.size() && vector[index] != elt)
    {
        ++index;
    }

    return index;
}

template<typename T, typename U>
bool has_dynamic_type(const U& u)
{
    return dynamic_cast<const T*>(&u) != nullptr;
}

#endif
