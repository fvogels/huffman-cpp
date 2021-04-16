#ifndef TEST_BUILD

#include <fstream>
#include "encoding/encodings.h"
#include "encoding/predictive/oracles.h"
#include "io/files.h"
#include "io/io-util.h"
#include <iostream>


using namespace encoding;

u64 file_size(const std::string& path)
{
    std::ifstream file(path);
    file.seekg(0, std::ios::end);
    return file.tellg();
}


int main()
{
    const std::string file_a = R"(g:\temp\aaaaa\a.txt)";
    const std::string file_b = R"(g:\temp\aaaaa\b.txt)";
    const std::string file_c = R"(g:\temp\aaaaa\c.txt)";

    auto pipeline = predictive_encoding<256>(encoding::predictive::trie_oracle(5)) | eof_encoding<256>() | adaptive_huffman<257>() | bit_grouper<8>();
    
    {
        auto input = io::create_file_data_source(file_a);
        auto output = io::create_file_data_destination(file_b);

        encoding::encode(input, pipeline, output);
    }

    {
        auto input = io::create_file_data_source(file_b);
        auto output = io::create_file_data_destination(file_c);
        // io::MemoryBuffer<256> buffer;
        // auto output = buffer.destination();

        encoding::decode(input, pipeline, output);        
    }

    auto uncompressed_size = file_size(file_a);
    auto compressed_size = file_size(file_b);

    std::cout << uncompressed_size << " -> " << compressed_size;
}

#endif
