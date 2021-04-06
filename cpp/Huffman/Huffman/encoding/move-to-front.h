#ifndef MOVE_TO_FRONT_CPP
#define MOVE_TO_FRONT_CPP

#include "defs.h"
#include "encoding/encoding.h"
#include <memory>

namespace encoding
{
    std::shared_ptr<encoding::EncodingImplementation> create_move_to_front_encoding_implementation(u64 domain_size);
    std::shared_ptr<encoding::EncodingImplementation> create_move_to_front_encoding_fast_implementation(u64 domain_size);

    template<unsigned N>
    encoding::Encoding<N, N> move_to_front()
    {
        return Encoding<N, N>(create_move_to_front_encoding_implementation(N));
    }

    template<unsigned N>
    encoding::Encoding<N, N> move_to_front_fast()
    {
        return Encoding<N, N>(create_move_to_front_encoding_fast_implementation(N));
    }
}

#endif
