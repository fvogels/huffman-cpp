#ifndef BINUTIL_H
#define BINUTIL_H

#include "defs.h"
#include "io/streams.h"
#include <vector>


namespace binary
{
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
}

#endif
