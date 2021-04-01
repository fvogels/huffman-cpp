#ifndef IO_UTIL_H
#define IO_UTIL_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include <vector>


namespace io
{
    template<typename T>
    void transfer(const std::vector<T>& xs, io::OutputStream<T>& output)
    {
        for (auto& x : xs)
        {
            output.write(x);
        }
    }

    template<typename T>
    void transfer(io::InputStream<T>& input, io::OutputStream<T>& output)
    {
        while (!input.end_reached())
        {
            auto x = input.read();
            output.write(x);
        }
    }
}

#endif
