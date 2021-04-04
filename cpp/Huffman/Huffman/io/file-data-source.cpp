#include "io/file-data-source.h"
#include "io/data-source.h"
#include "io/file-input-stream.h"


namespace
{
    class FileDataSource : public io::DataSource
    {
    private:
        std::string m_path;

    public:
        FileDataSource(const std::string& path) : m_path(path) { }

        std::unique_ptr<io::InputStream> create_input_stream() override
        {
            return io::create_file_input_stream(m_path);
        }
    };
}

std::unique_ptr<io::DataSource> io::create_file_data_source(const std::string& path)
{
    return std::make_unique<FileDataSource>(path);
}
