#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/huffman/huffman-encoding.h"
#include "io/memory-data.h"


namespace
{
    void check(const std::vector<Datum>& data)
    {
        auto encoding = encoding::create_huffman_encoder<256>();

        io::MemoryBuffer<256, Datum> buffer1(data);
        io::MemoryBuffer<2> buffer2;
        io::MemoryBuffer<256> buffer3;

        encoding::encode(buffer1.source(), encoding, buffer2.destination());
        encoding::decode(buffer2.source(), encoding, buffer3.destination());

        auto results = buffer3.data();

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
