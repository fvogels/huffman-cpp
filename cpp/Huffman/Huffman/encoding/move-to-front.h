#ifndef MOVE_TO_FRONT_CPP
#define MOVE_TO_FRONT_CPP

#include "defs.h"
#include "encoding/encoding.h"
#include <memory>

namespace encoding
{
    encoding::Encoding<Datum, Datum> move_to_front_encoding(u64 domain_size);
    encoding::Encoding<Datum, Datum> move_to_front_encoding_fast(u64 domain_size);
}

#endif
