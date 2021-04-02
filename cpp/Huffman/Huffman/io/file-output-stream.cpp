#include "file-output-stream.h"
#include <assert.h>
#include <fstream>
#include <iostream>

namespace
{
    class FileOutputStream : public io::OutputStream<uint8_t>
    {
    private:
        std::ofstream file;

    public:
        FileOutputStream(const std::string& path) : file(path, std::ios::binary)
        {
            assert(file);
        }

        void write(const uint8_t& datum)
        {
            file << datum;
        }

        bool end_reached() const
        {
            return file.eof();
        }
    };
}

std::unique_ptr<io::OutputStream<uint8_t>> io::create_file_output_stream(const std::string& path)
{
    return std::make_unique<FileOutputStream>(path);
}
