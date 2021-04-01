#ifndef ENCODING_H
#define ENCODING_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include "io/buffer.h"
#include <memory>


namespace encoding
{
    template<typename IN, typename OUT>
    class EncodingImplementation
    {
    public:
        typedef IN input_type;
        typedef OUT output_type;

        virtual ~EncodingImplementation() { }

        virtual void encode(io::InputStream<IN>& input, io::OutputStream<OUT>& output) const = 0;
        virtual void decode(io::InputStream<OUT>& input, io::OutputStream<IN>& output) const = 0;
    };

    template<typename IN, typename OUT>
    class Encoding
    {
        std::shared_ptr<EncodingImplementation<IN, OUT>> m_implementation;

    public:
        typedef IN input_type;
        typedef OUT output_type;

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

    template<typename T, class... Ts>
    Encoding<typename T::input_type, typename T::output_type> create_encoding(Ts... args)
    {
        return Encoding<typename T::input_type, typename T::output_type>(std::make_shared<T>(args...));
    }
}

#endif
