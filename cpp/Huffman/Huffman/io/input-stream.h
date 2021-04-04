#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include "defs.h"


namespace io
{
    struct InputStream
    {
        virtual ~InputStream()           { }

        virtual u64  read()              = 0;
        virtual bool end_reached() const = 0;
    };
}

#endif

