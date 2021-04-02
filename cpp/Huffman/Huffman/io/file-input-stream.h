#ifndef FILE_INPUT_STREAM_H
#define FILE_INPUT_STREAM_H

#include "defs.h"
#include <cstdint>
#include <memory>
#include <string>


namespace io
{
    std::unique_ptr<io::InputStream<uint8_t>> create_file_input_stream(const std::string& path);
}

#endif
