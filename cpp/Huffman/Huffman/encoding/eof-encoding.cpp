#include "encoding/eof-encoding.h"


namespace
{
    class EofImplementation : public encoding::EncodingImplementation
    {
    private:
        u64 m_domain_size;

    public:
        EofImplementation(u64 domain_size) : m_domain_size(domain_size)
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            const Datum eof = m_domain_size;

            while (!input.end_reached())
            {
                auto datum = input.read();
                output.write(datum);
            }

            output.write(eof);
        }

        void decode(io::InputStream& input, io::OutputStream& output) const override
        {
            const Datum eof = m_domain_size;
            Datum datum;

            while ((datum = input.read()) != eof)
            {
                output.write(datum);
            }
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_eof_implementation(u64 domain_size)
{
    return std::make_shared<EofImplementation>(domain_size);
}