#ifndef FILE_DATA_SOURCE_H
#define FILE_DATA_SOURCE_H

#include "data-source.h"
#include <string>


namespace io
{
    io::DataSource<256> create_file_data_source(const std::string& path);
}

#endif
