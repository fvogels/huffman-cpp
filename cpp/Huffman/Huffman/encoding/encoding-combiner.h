#ifndef ENCODING_COMBINER_H
#define ENCODING_COMBINER_H

#include "encoding/encoding.h"
#include "io/input-stream.h"
#include "io/output-stream.h"
#include "io/memory-data.h"
#include <memory>


namespace encoding
{
    template<u64 N1, u64 N2, u64 N3>
    class EncodingCombinerImplementation : public encoding::EncodingImplementation
    {
    private:
        Encoding<N1, N2> m_encoding1;
        Encoding<N2, N3> m_encoding2;

    public:
        EncodingCombinerImplementation(Encoding<N1, N2> encoding1, Encoding<N2, N3> encoding2) : m_encoding1(encoding1), m_encoding2(encoding2)
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const
        {
            io::MemoryBuffer<N2> buffer;

            m_encoding1->encode(input, *buffer.destination()->create_output_stream());
            m_encoding2->encode(*buffer.source()->create_input_stream(), output);
        }

        void decode(io::InputStream& input, io::OutputStream& output) const
        {
            io::MemoryBuffer<N2> buffer;

            m_encoding2->decode(input, *buffer.destination()->create_output_stream());
            m_encoding1->decode(*buffer.source()->create_input_stream(), output);
        }
    };


    template<u64 N1, u64 N2, u64 N3>
    Encoding<N1, N3> operator |(Encoding<N1, N2> encoding1, Encoding<N2, N3> encoding2)
    {
        return Encoding<N1, N3>(std::make_shared<EncodingCombinerImplementation<N1, N2, N3>>(encoding1, encoding2));
    }
}

#endif
