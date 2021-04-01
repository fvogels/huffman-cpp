#ifndef MOVE_TO_FRONT_CPP
#define MOVE_TO_FRONT_CPP

#include "defs.h"
#include "encoding/encoding.h"
#include <memory>

namespace encoding
{
    std::unique_ptr<EncodingImplementation<Datum, Datum>> move_to_front_encoding(uint64_t domain_size);
    std::unique_ptr<EncodingImplementation<Datum, Datum>> move_to_front_encoding_fast(uint64_t domain_size);
}

#endif
