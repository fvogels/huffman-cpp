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

    // TODO Move into cpp
    inline void transfer(io::InputStream& input, io::OutputStream& output)
    {
        while (!input.end_reached())
        {
            auto x = input.read();
            output.write(x);
        }
    }
}

#endif
