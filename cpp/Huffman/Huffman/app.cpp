#ifndef TEST_BUILD

#include <fstream>
#include "encoding/encodings.h"
#include "io/files.h"
#include "io/io-util.h"
#include <iostream>


int main()
{
    const std::string file_a = R"(g:\temp\aaaaa\a.txt)";
    const std::string file_b = R"(g:\temp\aaaaa\b.txt)";
    const std::string file_c = R"(g:\temp\aaaaa\c.txt)";

    auto pipeline = encoding::create_huffman_encoder<256>() | encoding::create_bit_grouper<8>();
    
    {
        auto input = io::create_file_data_source(file_a);
        auto output = io::create_file_data_destination(file_b);

        encoding::encode(input, pipeline, output);
    }

    {
        auto input = io::create_file_data_source(file_b);
        auto output = io::create_file_data_destination(file_c);

        encoding::decode(input, pipeline, output);
    }
}

#endif
