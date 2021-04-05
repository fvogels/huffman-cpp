#ifndef FILE_DATA_DESTINATION_H
#define FILE_DATA_DESTINATION_H

#include "data-destination.h"
#include <string>


namespace io
{
    io::DataDestination<256> create_file_data_destination(const std::string& path);
}

#endif
