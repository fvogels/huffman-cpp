#ifndef FILE_INPUT_STREAM_H
#define FILE_INPUT_STREAM_H

#include "io/input-stream.h"
#include <cstdint>
#include <memory>
#include <string>


namespace io
{
    std::unique_ptr<io::InputStream> create_file_input_stream(const std::string& path);
}

#endif
