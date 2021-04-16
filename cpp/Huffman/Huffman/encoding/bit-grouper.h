#ifndef BIT_GROUPER_H
#define BIT_GROUPER_H

#include "encoding/encoding.h"
#include "util.h"
#include <memory>

namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_bit_grouper_implementation(unsigned group_size);

    template<unsigned GROUP_SIZE>
    encoding::Encoding<2, 1 << GROUP_SIZE>bit_grouper()
    {
        return create_bit_grouper_implementation(GROUP_SIZE);
    }
}

#endif
