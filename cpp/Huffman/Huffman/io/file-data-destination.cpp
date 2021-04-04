#include "io/file-data-destination.h"
#include "io/data-destination.h"
#include "io/file-output-stream.h"


namespace
{
    class FileDataDestination: public io::DataDestination
    {
    private:
        std::string m_path;

    public:
        FileDataDestination(const std::string& path) : m_path(path) { }

        std::unique_ptr<io::OutputStream> create_output_stream() override
        {
            return io::create_file_output_stream(m_path);
        }
    };
}

std::unique_ptr<io::DataDestination> io::create_file_data_destination(const std::string& path)
{
    return std::make_unique<FileDataDestination>(path);
}
