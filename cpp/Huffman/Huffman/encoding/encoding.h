#ifndef ENCODING_H
#define ENCODING_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include <memory>


namespace encoding
{
    template<typename IN, typename OUT>
    class EncodingImplementation
    {
    public:
        virtual ~EncodingImplementation() { }

        virtual void encode(io::InputStream<IN>& input, io::OutputStream<OUT>& output) const = 0;
        virtual void decode(io::InputStream<OUT>& input, io::OutputStream<IN>& output) const = 0;
    };

    template<typename IN, typename OUT>
    class Encoding
    {
        std::shared_ptr<EncodingImplementation<IN, OUT>> m_implementation;

    public:
        Encoding(std::shared_ptr<EncodingImplementation<IN, OUT>> implementation) : m_implementation(implementation)
        {
            // NOP
        }

        EncodingImplementation<IN, OUT>* operator->()
        {
            return m_implementation.get();
        }

        const EncodingImplementation<IN, OUT>* operator->() const
        {
            return m_implementation.get();
        }
    };
}

#endif
