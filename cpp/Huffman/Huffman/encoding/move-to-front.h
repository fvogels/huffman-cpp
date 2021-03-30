#ifndef MOVE_TO_FRONT_CPP
#define MOVE_TO_FRONT_CPP

#include "defs.h"
#include "encoding/encoding.h"
#include <memory>

namespace encoding
{
    std::shared_ptr<Encoding<Data, Data>> move_to_front_encoding(uint64_t domain_size);
    std::shared_ptr<Encoding<Data, Data>> move_to_front_encoding_fast(uint64_t domain_size);
}

#endif
