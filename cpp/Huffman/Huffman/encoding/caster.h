#ifndef CASTER_H
#define CASTER_H

#include "encoding/encoding.h"
#include "encoding/mapper.h"


namespace encoding
{
    template<typename T, typename U>
    Encoding<T, U> create_caster()
    {
        return create_mapper<T, U>(
            [](const T& x) -> U
            {
                return static_cast<U>(x);
            },
            [](const U& x) -> T
            {
                return static_cast<T>(x);
            }
        );
    }
}

#endif
