#include "io/file-streams.h"
#include "io/input-stream.h"
#include "io/memory-buffer.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <cstdint>


namespace
{
    class FileOutputStream : public io::OutputStream
    {
    private:
        std::ofstream file;

    public:
        FileOutputStream(const std::string& path) : file(path, std::ios::binary)
        {
            assert(file);
        }

        void write(u64 datum)
        {
            assert(datum <= std::numeric_limits<uint8_t>::max());

            file << uint8_t(datum);
        }
    };
}

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

std::unique_ptr<io::OutputStream> io::create_file_output_stream(const std::string& path)
{
    return std::make_unique<FileOutputStream>(path);
}

