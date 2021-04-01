#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"
#include "io/buffer.h"
#include "io/io-util.h"
#include <functional>


namespace
{
    void check(const std::vector<Datum>& data, encoding::Encoding<Datum, Datum> encoding)
    {
        io::Buffer<Datum> buffer1(data);
        io::Buffer<Datum> buffer2;
        io::Buffer<Datum> buffer3;

        encoding->encode(*buffer1.create_input_stream(), *buffer2.create_output_stream());
        encoding->decode(*buffer2.create_input_stream(), *buffer3.create_output_stream());

        auto decoded_data = buffer3.contents();

        REQUIRE(data.size() == decoded_data->size());

        for (auto i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*decoded_data)[i]);
        }
    }
}


#define TEST(encoding, ...) TEST_CASE("Pipeline " #encoding " on " #__VA_ARGS__) { check(std::vector<Datum> { __VA_ARGS__ }, (encoding)); } \

TEST(encoding::move_to_front_encoding(16) | encoding::create_huffman_encoder(16), 0)
TEST(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256), 0)
TEST(encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8), 0)
TEST(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8), 0)

#endif
