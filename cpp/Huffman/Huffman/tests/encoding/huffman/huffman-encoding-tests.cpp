#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/huffman/huffman-encoding.h"
#include "io/buffer.h"


namespace
{
    void check(const std::vector<Datum>& data)
    {
        auto encoder = encoding::create_huffman_encoder<256>();
        io::Buffer<Datum> buffer1(data);
        io::Buffer<Datum> buffer2;
        io::Buffer<Datum> buffer3;

        encoder->encode(*buffer1.create_input_stream(), *buffer2.create_output_stream());
        encoder->decode(*buffer2.create_input_stream(), *buffer3.create_output_stream());

        auto results = buffer3.contents();

        REQUIRE(data.size() == results->size());

        for (size_t i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*results)[i]);
        }
    }
}

#define TEST(...) TEST_CASE("Huffman Encoding on { " #__VA_ARGS__ " }") { check(std::vector<Datum> { __VA_ARGS__ }); }

TEST(1, 2)
TEST(1, 1, 2)
TEST(1, 1, 2, 2)
TEST(1, 2, 3, 2, 1)
TEST(1, 1, 2, 3, 3, 4, 4, 4, 4, 3, 2, 1, 2, 3, 4)

#endif
