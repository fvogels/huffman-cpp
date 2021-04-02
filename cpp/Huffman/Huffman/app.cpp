#ifndef TEST_BUILD

#include <fstream>
#include "encoding/encodings.h"
#include "io/file-input-stream.h"
#include "io/file-output-stream.h"
#include "io/io-util.h"
#include <iostream>


int main()
{
    const std::string file_a = R"(g:\temp\aaaaa\a.txt)";
    const std::string file_b = R"(g:\temp\aaaaa\b.txt)";
    const std::string file_c = R"(g:\temp\aaaaa\c.txt)";

    auto pipeline = encoding::create_datum_encoding() | encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8) | ~encoding::create_datum_encoding();
    
    {
        io::Buffer<uint8_t> input_buffer;
        io::Buffer<uint8_t> output_buffer;
        auto input = io::create_file_input_stream(file_a);
        transfer(*input, *input_buffer.create_output_stream());

        // auto output = io::create_file_output_stream(file_b);
        auto output = output_buffer.create_output_stream();

        pipeline->encode(*input_buffer.create_input_stream(), *output_buffer.create_output_stream());

        std::cout << input_buffer.contents()->size() << " -> " << output_buffer.contents()->size();
    }

    //{
    //    auto input = io::create_file_input_stream(file_b);
    //    auto output = io::create_file_output_stream(file_c);

    //    pipeline->decode(*input, *output);
    //}
}

#endif
