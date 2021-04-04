#ifndef BINUTIL_H
#define BINUTIL_H

#include "defs.h"
#include "io/input-stream.h"
#include "io/output-stream.h"
#include <vector>


namespace binary
{
    void write_bits(u64 value, unsigned nbits, io::OutputStream& output);
    u64 read_bits(unsigned nbits, io::InputStream& input);
}

#endif
