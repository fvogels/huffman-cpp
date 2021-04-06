#ifndef PREDICTIVE_ENCODING_H
#define PREDICTIVE_ENCODING_H

#include "encoding/encoding.h"
#include "encoding/predictive/oracle.h"
#include <memory>


namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_predictive_encoding_implementation(u64 domain_size, std::unique_ptr<encoding::predictive::Oracle> oracle);

    template<u64 N>
    Encoding<N, N> predictive_encoding(std::unique_ptr<encoding::predictive::Oracle> oracle)
    {
        return encoding::Encoding<N, N>(create_predictive_encoding_implementation(N, std::move(oracle)));
    }
}

#endif
