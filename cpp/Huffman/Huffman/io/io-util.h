#ifndef IO_UTIL_H
#define IO_UTIL_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include <vector>


namespace io
{
    template<typename T>
    void transfer(const std::vector<T>& xs, io::OutputStream& output)
    {
        for (auto& x : xs)
        {
            output.write(x);
        }
    }

    void transfer(io::InputStream& input, io::OutputStream& output);

    void write_bits(u64 value, unsigned nbits, io::OutputStream& output);
    u64 read_bits(unsigned nbits, io::InputStream& input);
}

#endif
