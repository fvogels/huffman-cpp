#ifndef INVERTER_H
#define INVERTER_H

#include "encoding/encoding.h"


namespace encoding
{
    template<typename IN, typename OUT>
    class InverterImplementation : public EncodingImplementation<OUT, IN>
    {
    private:
        Encoding<IN, OUT> m_encoding;

    public:
        InverterImplementation(Encoding<IN, OUT> encoding) : m_encoding(encoding)
        {
            // NOP
        }

        void encode(io::InputStream<OUT>& input, io::OutputStream<IN>& output) const
        {
            m_encoding->decode(input, output);
        }

        void decode(io::InputStream<IN>& input, io::OutputStream<OUT>& output) const
        {
            m_encoding->encode(input, output);
        }
    };

    template<typename IN, typename OUT>
    encoding::Encoding<OUT, IN> operator ~(const Encoding<IN, OUT>& encoding)
    {
        return create_encoding<InverterImplementation<IN, OUT>>(encoding);
    }
}

#endif
