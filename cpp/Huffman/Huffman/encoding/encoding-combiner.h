#ifndef ENCODING_COMBINER_H
#define ENCODING_COMBINER_H

#include "encoding/encoding.h"
#include "io/input-stream.h"
#include "io/output-stream.h"
#include "io/buffer.h"
#include <memory>


namespace encoding
{
    template<typename T1, typename T2, typename T3>
    class EncodingCombinerImplementation : public EncodingImplementation<T1, T3>
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

            m_e1->encode(input, *buffer.create_output_stream());
            m_e2->encode(*buffer.create_input_stream(), output);
        }

        void decode(io::InputStream<T3>& input, io::OutputStream<T1>& output) const
        {
            io::Buffer<T2> buffer;

            m_e2->decode(input, *buffer.create_output_stream());
            m_e1->decode(*buffer.create_input_stream(), output);
        }
    };

    template<typename T1, typename T2, typename T3>
    Encoding<T1, T3> operator |(Encoding<T1, T2> e1, Encoding<T2, T3> e2)
    {
        return create_encoding<EncodingCombinerImplementation<T1, T2, T3>>(e1, e2);
    }
}

#endif
