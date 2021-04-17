#ifndef BINARY_IO_H
#define BINARY_IO_H

#include "io/streams.h"


namespace io
{
    void write_bits(u64 value, unsigned nbits, io::OutputStream& output);
    u64 read_bits(unsigned nbits, io::InputStream& input);
}

#endif
