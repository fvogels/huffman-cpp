#ifdef TEST_BUILD

#include "util.h"
#include "catch.hpp"


namespace
{
    void check(u64 domain_size, unsigned expected)
    {
        int actual = bits_needed(domain_size);

        CHECK(actual == expected);
    }
}

#define TEST(DS, E) TEST_CASE("bits_needed(" #DS ") -> " #E) { check(DS, E); }

TEST(2, 1)
TEST(3, 2)
TEST(4, 2)
TEST(5, 3)
TEST(6, 3)
TEST(7, 3)
TEST(8, 3)
TEST(9, 4)
TEST(128, 7)
TEST(129, 8)
TEST(1'000'000, 20)

#endif
