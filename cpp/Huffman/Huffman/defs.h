#ifndef DEFS_H
#define DEFS_H

#include <cstdint>
#include <vector>
#include <deque>


typedef uint64_t               u64;
typedef uint64_t               Datum;

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

constexpr unsigned bits_needed(u64 domain_size)
{
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
