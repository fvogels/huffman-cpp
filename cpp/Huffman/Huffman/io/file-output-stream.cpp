#include "file-output-stream.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <limits>


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

std::unique_ptr<io::OutputStream> io::create_file_output_stream(const std::string& path)
{
    return std::make_unique<FileOutputStream>(path);
}
