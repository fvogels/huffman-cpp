#ifndef EOF_ENCODING_H
#define EOF_ENCODING_H

#include "encoding/encoding.h"
#include "util.h"
#include <memory>

namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_eof_implementation(u64 domain_size);

    template<u64 N>
    encoding::Encoding<N, N+1> eof_encoding()
    {
        return create_eof_implementation(N);
    }
}

#endif
