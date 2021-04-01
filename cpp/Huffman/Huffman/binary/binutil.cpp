#include "binary/binutil.h"
#include <assert.h>

void binary::write_bits(u64 value, unsigned nbits, io::OutputStream<Datum>& output)
{
    assert((value >> nbits) == 0);

    for (unsigned i = 0; i != nbits; ++i)
    {
        bool b = (value & (u64(1) << (nbits - i - 1))) != 0;
        output.write(b);
    }
}

u64 binary::read_bits(unsigned nbits, io::InputStream<Datum>& input)
{
    u64 result = 0;

    for ( unsigned i = 0; i != nbits; ++i )
    {
        auto bit = input.read();
        result = (result << 1) | u64(bit);
    }

    return result;
}

unsigned binary::bits_needed(u64 value)
{
    unsigned i = 0;

    while (value != 0)
    {
        value >>= 1;
        ++i;
    }

    return i;
}