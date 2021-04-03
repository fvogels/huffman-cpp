#ifndef ENCODING_COMBINER_H
#define ENCODING_COMBINER_H

#include "encoding/encoding.h"
#include "io/input-stream.h"
#include "io/output-stream.h"
#include "io/buffer.h"
#include <memory>


namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_combiner_implementation(std::shared_ptr<encoding::EncodingImplementation> e1, std::shared_ptr<encoding::EncodingImplementation> e2);

    template<u64 N1, u64 N2, u64 N3>
    Encoding<N1, N3> operator |(Encoding<N1, N2> e1, Encoding<N2, N3> e2)
    {
        return Encoding<N1, N3>(create_combiner_implementation(e1.implementation(), e2.implementation()));
    }
}

#endif
