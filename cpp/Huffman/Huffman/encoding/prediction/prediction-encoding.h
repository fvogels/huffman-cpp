#ifndef PREDICTION_ENCODING_H
#define PREDICTION_ENCODING_H

#include "encoding/encoding.h"
#include "encoding/prediction/oracle.h"
#include <memory>


namespace encoding
{
    std::shared_ptr<EncodingImplementation> create_predictive_encoding_implementation(std::unique_ptr<encoding::prediction::Oracle> oracle);

    template<u64 N>
    Encoding<N, N> create_predictive_encoder(std::unique_ptr<encoding::prediction::Oracle> oracle)
    {
        return encoding::Encoding<IN, 2>(create_predictive_encoding_implementation(IN, std::move(oracle));
    }
}

#endif
