#ifdef TEST_BUILD

#include "util.h"
#include "catch.hpp"


namespace
{
    void check(u64 domain_size, unsigned expected)
    {
        int actual = bytes_needed(domain_size);

        CHECK(actual == expected);
    }
}

#define TEST(DS, E) TEST_CASE("bytes_needed(" #DS ") -> " #E) { check(DS, E); }

TEST(2, 1)
TEST(255, 1)
TEST(256, 1)
TEST(257, 2)
TEST(65536, 2)
TEST(65537, 3)
TEST(1000000, 3)

#endif
