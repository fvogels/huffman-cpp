#ifndef MAPPER_H
#define MAPPER_H

#include "encoding/encoding.h"
#include "defs.h"
#include <functional>


namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_mapper_implementation(std::function<Datum(const Datum&)> function, std::function<Datum(const Datum&)> inverse_function);

    template<u64 IN, u64 OUT>
    encoding::Encoding<IN, OUT> create_mapper(std::function<Datum(const Datum&)> function, std::function<Datum(const Datum&)> inverse_function)
    {
        return encoding::Encoding<IN, OUT>(create_mapper_implementation(function, inverse_function));
    }
}

#endif
