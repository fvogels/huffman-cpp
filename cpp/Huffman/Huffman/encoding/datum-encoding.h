#ifndef DATUM_ENCODING_H
#define DATUM_ENCODING_H

#include "encoding/encoding.h"
#include "defs.h"
#include <cstdint>


namespace encoding
{
    encoding::Encoding<uint8_t, Datum> create_datum_encoding();
}

#endif
