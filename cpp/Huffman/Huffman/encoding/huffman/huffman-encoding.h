#ifndef HUFFMAN_ENCODING_H
#define HUFFMAN_ENCODING_H

#include "encoding/encoding.h"
#include "defs.h"
#include <memory>


namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_huffman_implementation(u64 domain_size);

    template<u64 IN>
    Encoding<IN, 2> huffman_encoder()
    {
        return encoding::Encoding<IN, 2>(create_huffman_implementation(IN));
    }
}

#endif
