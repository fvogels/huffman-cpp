#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/bit-grouper.h"
#include "io/memory-data.h"


namespace
{
    template<u64 GROUP_SIZE>
    void check(const std::vector<byte>& data)
    {
        auto encoding = encoding::create_bit_grouper<GROUP_SIZE>();

        io::MemoryBuffer<2> buffer1(data);
        io::MemoryBuffer<(1 << GROUP_SIZE)> buffer2;
        io::MemoryBuffer<2> buffer3;

        encoding::encode<2, 1 << GROUP_SIZE>(buffer1.source(), encoding, buffer2.destination());
        encoding::decode(buffer2.source(), encoding, buffer3.destination());

        auto result = buffer3.data();

        REQUIRE((data.size() + GROUP_SIZE - 1) / GROUP_SIZE * GROUP_SIZE == result->size());

        for (size_t i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*result)[i]);
        }
    }
}

#define TEST(GROUPSIZE, ...) TEST_CASE("Bit Grouper (group size " #GROUPSIZE ") on { " #__VA_ARGS__ " }") { check<GROUPSIZE>(std::vector<uint8_t> { __VA_ARGS__ }); }

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
