#include "encoding/datum-encoding.h"
#include "encoding/mapper.h"


encoding::Encoding<uint8_t, Datum> encoding::create_datum_encoding()
{
    return create_mapper<uint8_t, Datum>(
        [](const uint8_t& x) -> Datum
    {
        return static_cast<Datum>(x);
    },
        [](const Datum& x) -> uint8_t
    {
        assert(x < 256);
        return static_cast<uint8_t>(x);
    }
    );
}

