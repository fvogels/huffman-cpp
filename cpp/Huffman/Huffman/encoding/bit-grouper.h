#ifndef BIT_GROUPER_H
#define BIT_GROUPER_H

#include "encoding/encoding.h"
#include "defs.h"
#include <memory>

namespace encoding
{
    std::unique_ptr<encoding::EncodingImplementation<Datum, Datum>> create_bit_grouper(unsigned group_size);
}

#endif
