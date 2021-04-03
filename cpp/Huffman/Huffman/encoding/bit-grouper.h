#ifndef BIT_GROUPER_H
#define BIT_GROUPER_H

#include "encoding/encoding.h"
#include "defs.h"
#include <memory>

namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_bit_grouper_implementation(u64 group_size);

    template<u64 GROUP_SIZE>
    encoding::Encoding<2, (1 << GROUP_SIZE)> create_bit_grouper()
    {
        return create_bit_grouper_implementation(GROUP_SIZE);
    }
}

#endif
