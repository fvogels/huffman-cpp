#ifndef ENCODING_H
#define ENCODING_H

#include "io/streams.h"
#include "io/data-endpoints.h"
#include "util.h"
#include <memory>


namespace encoding
{
    class EncodingImplementation
    {
    public:
        virtual ~EncodingImplementation() { }

        virtual void encode(io::InputStream& input, io::OutputStream& output) const = 0;
        virtual void decode(io::InputStream& input, io::OutputStream& output) const = 0;
    };

    template<u64 IN, u64 OUT>
    class Encoding
    {
        std::shared_ptr<EncodingImplementation> m_implementation;

    public:
        constexpr static unsigned input_domain_size = IN;
        constexpr static unsigned output_domain_size = OUT;

        Encoding(std::shared_ptr<EncodingImplementation> implementation) : m_implementation(implementation)
        {
            // NOP
        }

        EncodingImplementation* operator->()
        {
            return m_implementation.get();
        }

        const EncodingImplementation* operator->() const
        {
            return m_implementation.get();
        }

        std::shared_ptr<EncodingImplementation> implementation() const
        {
            return m_implementation;
        }
    };

    template<u64 IN, u64 OUT>
    void encode(io::DataSource<IN> source, Encoding<IN, OUT> encoding, io::DataDestination<OUT> destination)
    {
        auto input_stream = source->create_input_stream();
        auto output_stream = destination->create_output_stream();

        encoding->encode(*input_stream, *output_stream);
    }

    template<u64 IN, u64 OUT>
    void decode(io::DataSource<OUT> source, Encoding<IN, OUT> encoding, io::DataDestination<IN> destination)
    {
        auto input_stream = source->create_input_stream();
        auto output_stream = destination->create_output_stream();

        encoding->decode(*input_stream, *output_stream);
    }
}

#endif
