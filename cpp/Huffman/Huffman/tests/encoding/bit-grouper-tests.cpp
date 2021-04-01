#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/bit-grouper.h"
#include "io/buffer.h"


namespace
{
    void check(unsigned group_size, const std::vector<Datum>& data)
    {
        auto encoding = encoding::create_bit_grouper(group_size);
        io::Buffer<Datum> buffer1(data);
        io::Buffer<Datum> buffer2;
        io::Buffer<Datum> buffer3;
        
        encoding->encode(*buffer1.create_input_stream(), *buffer2.create_output_stream());
        encoding->decode(*buffer2.create_input_stream(), *buffer3.create_output_stream());

        auto result = buffer3.contents();

        REQUIRE((data.size() + group_size - 1) / group_size * group_size == result->size());

        for (size_t i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*result)[i]);
        }
    }
}

#define TEST(GROUPSIZE, ...) TEST_CASE("Bit Grouper (group size " #GROUPSIZE ") on { " #__VA_ARGS__ " }") { check(GROUPSIZE, std::vector<Datum> { __VA_ARGS__ }); }

TEST(4)
TEST(4, 0)
TEST(4, 1)
TEST(4, 0, 0)
TEST(4, 0, 1)
TEST(4, 1, 0)
TEST(4, 1, 1)
TEST(4, 0, 0, 0, 0)
TEST(4, 1, 1, 1, 1)
TEST(4, 1, 1, 1, 1, 0)
TEST(4, 1, 1, 1, 1, 1)
TEST(4, 0, 1, 0, 1, 0, 1)
TEST(4, 0, 1, 0, 1, 0, 1, 0)
TEST(4, 0, 1, 0, 1, 0, 1, 0, 1)
TEST(2, 0, 1, 0, 1, 0, 1, 0, 1)
TEST(3, 0, 1, 0, 1, 0, 1, 0, 1)
TEST(5, 0, 1, 0, 1, 0, 1, 0, 1)

#endif
