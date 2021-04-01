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

    template<typename T1, typename T2, typename T3>
    class EncodingCombinerImplementation : EncodingImplementation<T1, T3>
    {
    private:
        Encoding<T1, T2> m_e1;
        Encoding<T2, T3> m_e2;

    public:
        EncodingCombinerImplementation(Encoding<T1, T2> e1, Encoding<T2, T3> e2) : m_e1(e1), m_e2(e2)
        {
            // NOP
        }

        void encode(io::InputStream<T1>& input, io::OutputStream<T3>& output) const
        {
            io::Buffer<T2> buffer;

            m_e1.encode(input, *buffer.create_output_stream());
            m_e2.encode(*buffer.create_input_stream(), output);
        }

        void decode(io::InputStream<T3>& input, io::OutputStream<T1>& output) const
        {
            io::Buffer<T2> buffer;

            m_e2.decode(input, *buffer.create_output_stream());
            m_e1.decode(*buffer.create_input_stream(), output);
        }
    };

    template<typename T, class... Ts>
    Encoding<typename T::input_type, typename T::output_type> create_encoding(Ts... args)
    {
        auto encoding = std::make_shared<T>(args...);
        return Encoding<typename T::input_type, typename T::output_type>(encoding);
    }

    template<typename T1, typename T2, typename T3>
    Encoding<T1, T3> operator |(Encoding<T1, T2> e1, Encoding<T2, T3> e2)
    {
        return create_encoding<encoding::EncodingCombinerImplementation<T1, T2, T3>>(e1, e2);
    }
}

#endif
