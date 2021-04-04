#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

#include "defs.h"


namespace io
{
    struct OutputStream
    {
        virtual ~OutputStream() { }

        virtual void write(u64 value) = 0;
    };
}

#endif
