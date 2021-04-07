#ifndef INVERTER_H
#define INVERTER_H

#include "encoding/encoding.h"
#include "defs.h"


namespace encoding
{
    std::shared_ptr<encoding::EncodingImplementation> create_combiner_implementation(std::shared_ptr<encoding::EncodingImplementation> encoding);

    template<u64 IN, u64 OUT>
    encoding::Encoding<OUT, IN> operator ~(const Encoding<IN, OUT>& encoding)
    {
        return Encoding<OUT, IN>(create_combiner_implementation(encoding.implementation()));
    }
}

#endif
