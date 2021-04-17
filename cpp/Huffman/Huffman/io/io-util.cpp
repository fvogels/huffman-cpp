#include "io/io-util.h"
#include <assert.h>


void io::transfer(io::InputStream& input, io::OutputStream& output)
{
    while (!input.end_reached())
    {
        auto x = input.read();
        output.write(x);
    }
}

void io::transfer(io::InputStream& input, io::OutputStream& output, unsigned count)
{
    while (count > 0)
    {
        assert(!input.end_reached());

        auto x = input.read();
        output.write(x);
        --count;
    }
}

