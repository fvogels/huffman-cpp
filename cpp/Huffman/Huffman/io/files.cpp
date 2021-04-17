#include "io/files.h"
#include "io/streams.h"
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

        void write(Datum datum)
        {
            assert(datum <= std::numeric_limits<uint8_t>::max());

            file << uint8_t(datum);
        }
    };

    class FileDataSourceImplementation : public io::DataSourceImplementation
    {
    private:
        std::string m_path;

    public:
        FileDataSourceImplementation(const std::string& path) : m_path(path) { }

        std::unique_ptr<io::InputStream> create_input_stream() override
        {
            return io::create_file_input_stream(m_path);
        }
    };

    class FileDataDestinationImplementation : public io::DataDestinationImplementation
    {
    private:
        std::string m_path;

    public:
        FileDataDestinationImplementation(const std::string& path) : m_path(path) { }

        std::unique_ptr<io::OutputStream> create_output_stream() override
        {
            return io::create_file_output_stream(m_path);
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

io::DataSource<256> io::create_file_data_source(const std::string& path)
{
    return io::DataSource<256>(std::make_shared<FileDataSourceImplementation>(path));
}


io::DataDestination<256> io::create_file_data_destination(const std::string& path)
{
    return io::DataDestination<256>(std::make_shared<FileDataDestinationImplementation>(path));
}

