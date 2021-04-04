#ifndef FILE_DATA_SOURCE_H
#define FILE_DATA_SOURCE_H

#include "data-source.h"
#include <string>


namespace io
{
    std::unique_ptr<io::DataSource> create_file_data_source(const std::string& path);
}

#endif
