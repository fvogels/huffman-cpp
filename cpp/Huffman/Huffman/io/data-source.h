#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "io/input-stream.h"
#include <memory>


namespace io
{
    class DataSource
    {
    public:
        virtual std::unique_ptr<InputStream> create_input_stream() = 0;
    };
}

#endif
