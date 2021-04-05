#include "io/file-data-destination.h"
#include "io/data-destination.h"
#include "io/file-streams.h"


namespace
{
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

io::DataDestination<256> io::create_file_data_destination(const std::string& path)
{
    return io::DataDestination<256>(std::make_shared<FileDataDestinationImplementation>(path));
}
