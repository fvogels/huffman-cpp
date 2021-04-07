#include "encoding/bit-grouper.h"
#include "io/io-util.h"
#include "binary/binutil.h"
#include <assert.h>


namespace
{
    class BitGrouper : public encoding::EncodingImplementation
    {
    private:
        unsigned m_group_size;

    public:
        BitGrouper(unsigned group_size) : m_group_size(group_size)
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            u64 buffer = 0;
            unsigned nbits = 0;

            while (!input.end_reached())
            {
                auto datum = io::read_bits(m_group_size, input);
                output.write(datum);
            }
        }

        void decode(io::InputStream& input, io::OutputStream& output) const override
        {
            while (!input.end_reached())
            {
                auto datum = input.read();
                io::write_bits(datum, m_group_size, output);
            }
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_bit_grouper_implementation(unsigned group_size)
{
    return std::make_shared<BitGrouper>(group_size);
}