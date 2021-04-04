#ifndef DATA_DESTINATION_H
#define DATA_DESTINATION_H

#include "io/output-stream.h"
#include <memory>

namespace io
{
    class DataDestination
    {
    public:
        virtual std::unique_ptr<OutputStream> create_output_stream() = 0;
    };
}

#endif
