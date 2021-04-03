#include "encoding/inverter.h"


namespace
{
    class InverterImplementation : public encoding::EncodingImplementation
    {
    private:
        std::shared_ptr<encoding::EncodingImplementation> m_encoding;

    public:
        InverterImplementation(std::shared_ptr<encoding::EncodingImplementation> encoding) : m_encoding(encoding)
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

std::shared_ptr<encoding::EncodingImplementation> encoding::create_combiner_implementation(std::shared_ptr<encoding::EncodingImplementation> encoding)
{
    return std::make_shared<InverterImplementation>(encoding);
}