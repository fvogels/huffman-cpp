#ifndef ADAPTIVE_HUFFMAN_ENCODING
#define ADAPTIVE_HUFFMAN_ENCODING

#include "encoding/encoding.h"
#include "defs.h"
#include <memory>


namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_adaptive_huffman_implementation(u64 domain_size);

    template<u64 IN>
    Encoding<IN, 2> adaptive_huffman_encoder()
    {
        return encoding::Encoding<IN, 2>(create_adaptive_huffman_implementation(IN));
    }
}

#endif

