#include "io/file-input-stream.h"
#include "io/buffer.h"
#include <assert.h>
#include <fstream>


std::unique_ptr<io::InputStream> io::create_file_input_stream(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    io::Buffer<uint8_t> buffer(data);
    return buffer.create_input_stream();
}
