#include "encoding/inverter.h"


namespace
{
    class InverterImplementation : public encoding::EncodingImplementation
    {
    private:
        std::shared_ptr<encoding::EncodingImplementation> m_encoding;

    public:
        InverterImplementation(std::shared_ptr<EncodingImplementation> encoding) : m_encoding(encoding)
        {
            // NOP
        }

        void encode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const
        {
            m_encoding->decode(input, output);
        }

        void decode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const
        {
            m_encoding->encode(input, output);
        }
    };
}

