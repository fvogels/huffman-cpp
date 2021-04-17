#include "io/binary-io.h"


void io::write_bits(u64 value, unsigned nbits, io::OutputStream& output)
{
    assert((value >> nbits) == 0);

    for (unsigned i = 0; i != nbits; ++i)
    {
        bool b = (value & (u64(1) << (nbits - i - 1))) != 0;
        output.write(b);
    }
}

u64 io::read_bits(unsigned nbits, io::InputStream& input)
{
    u64 result = 0;

    for (unsigned i = 0; i != nbits; ++i)
    {
        auto bit = input.end_reached() ? 0 : input.read();
        assert(bit == 0 || bit == 1);
        result = (result << 1) | u64(bit);
    }

    return result;
}
