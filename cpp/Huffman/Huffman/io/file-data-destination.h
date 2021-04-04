#ifndef FILE_DATA_DESTINATION_H
#define FILE_DATA_DESTINATION_H

#include "data-destination.h"
#include <string>


namespace io
{
    std::unique_ptr<io::DataDestination> create_file_data_destination(const std::string& path);
}

#endif
