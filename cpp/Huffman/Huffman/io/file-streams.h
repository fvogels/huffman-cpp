#ifndef FILE_OUTPUT_STREAM_H
#define FILE_OUTPUT_STREAM_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include <cstdint>
#include <memory>
#include <string>


namespace io
{
    std::unique_ptr<io::InputStream> create_file_input_stream(const std::string& path);
    std::unique_ptr<io::OutputStream> create_file_output_stream(const std::string& path);
}

#endif
