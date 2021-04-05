#include "io/file-input-stream.h"
#include "io/memory-data.h"
#include <assert.h>
#include <fstream>


std::unique_ptr<io::InputStream> io::create_file_input_stream(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto data = std::make_shared<std::vector<uint8_t>>(size);
    file.read(reinterpret_cast<char*>(data->data()), size);

    io::MemoryBuffer<256, uint8_t> memory(data);
    return memory.source()->create_input_stream();
}
