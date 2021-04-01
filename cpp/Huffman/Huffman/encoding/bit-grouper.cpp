#include "encoding/bit-grouper.h"
#include "binary/binutil.h"


namespace
{
    class BitGrouper : public encoding::EncodingImplementation<Datum, Datum>
    {
    private:
        unsigned m_group_size;

    public:
        BitGrouper(unsigned group_size) : m_group_size(group_size)
        {
            // NOP
        }

        void encode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const override
        {
            u64 buffer = 0;
            unsigned nbits = 0;

            while (!input.end_reached())
            {
                auto datum = binary::read_bits(m_group_size, input);
                output.write(datum);
            }
        }

        void decode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const override
        {
            while (!input.end_reached())
            {
                auto datum = input.read();
                binary::write_bits(datum, m_group_size, output);
            }
        }
    };
}

std::unique_ptr<encoding::EncodingImplementation<Datum, Datum>> encoding::create_bit_grouper(unsigned group_size)
{
    return std::make_unique<BitGrouper>(group_size);
}
