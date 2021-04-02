#include "file-input-stream.h"
#include <assert.h>
#include <fstream>


namespace
{
    class FileInputStream : public io::InputStream<uint8_t>
    {
    private:
        std::ifstream file;

    public:
        FileInputStream(const std::string& path) : file(path, std::ios::binary)
        {
            assert(file);
        }

        uint8_t read()
        {
            uint8_t datum;
            file >> datum;
            return datum;            
        }

        bool end_reached() const
        {
            return file.eof();
        }
    };
}

std::unique_ptr<io::InputStream<uint8_t>> io::create_file_input_stream(const std::string& path)
{
    return std::make_unique<FileInputStream>(path);
}
