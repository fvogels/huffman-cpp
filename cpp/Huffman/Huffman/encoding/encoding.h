#ifndef ENCODING_H
#define ENCODING_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include "io/buffer.h"
#include "defs.h"
#include <memory>


namespace encoding
{
    class EncodingImplementation
    {
    public:
        virtual ~EncodingImplementation() { }

        virtual void encode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const = 0;
        virtual void decode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const = 0;
    };

    template<unsigned IN, unsigned OUT>
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
}

#endif
