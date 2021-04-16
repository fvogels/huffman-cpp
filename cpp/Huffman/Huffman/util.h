#ifndef DEFS_H
#define DEFS_H

#include <assert.h>
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

inline int binary_search(const int* ns, int length, int value)
{
    assert(ns != nullptr || length == 0);
    assert(length >= 0);

    int left = 0;
    int right = length - 1;

    while (left < right)
    {
        int middle = (left + right) / 2;
        assert(left <= middle && middle <= right);

        if (ns[middle] < value)
        {
            left = middle + 1;
        }
        else if (ns[middle] > value)
        {
            right = middle - 1;
        }
        else
        {
            left = right = middle;
        }
    }

    int result = left >= length || ns[left] != value ? -1 : left;

    assert(result == -1 || (0 <= result && result < length));
    assert(result == -1 || ns[result] == value);
    return result;
}

constexpr unsigned bits_needed(u64 domain_size)
{
    --domain_size;
    unsigned i = 0;

    while (domain_size != 0)
    {
        domain_size >>= 1;
        ++i;
    }

    return i;
}

constexpr unsigned bytes_needed(u64 domain_size)
{
    return (bits_needed(domain_size) + 7) / 8;
}

template<unsigned NBITS> struct SelectIntegerTypeByBytes;

template<> struct SelectIntegerTypeByBytes<1> { typedef uint8_t type; };
template<> struct SelectIntegerTypeByBytes<2> { typedef uint16_t type; };
template<> struct SelectIntegerTypeByBytes<3> { typedef uint32_t type; };
template<> struct SelectIntegerTypeByBytes<4> { typedef uint32_t type; };
template<> struct SelectIntegerTypeByBytes<5> { typedef uint64_t type; };
template<> struct SelectIntegerTypeByBytes<6> { typedef uint64_t type; };
template<> struct SelectIntegerTypeByBytes<7> { typedef uint64_t type; };
template<> struct SelectIntegerTypeByBytes<8> { typedef uint64_t type; };

template<u64 DOMAIN_SIZE> struct SelectIntegerTypeByDomainSize
{
    typedef typename SelectIntegerTypeByBytes<bytes_needed(DOMAIN_SIZE)>::type type;
};

#endif
