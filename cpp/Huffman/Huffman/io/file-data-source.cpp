#include "io/file-data-source.h"
#include "io/data-source.h"
#include "io/file-input-stream.h"


namespace
{
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
}

io::DataSource<256> io::create_file_data_source(const std::string& path)
{
    return io::DataSource<256>(std::make_shared<FileDataSourceImplementation>(path));
}
