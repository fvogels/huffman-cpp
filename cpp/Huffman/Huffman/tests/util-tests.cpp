#ifdef TEST_BUILD

#include "util.h"
#include "catch.hpp"


namespace
{
    void check(int value, const std::vector<int>& values, int expected)
    {
        int actual = binary_search(values.data(), (int) values.size(), value);

        CHECK(actual == expected);
    }
}

#define TEST(expected, value, ...) TEST_CASE("Binary search for " #value " in " #__VA_ARGS__) { check(value, std::vector<int> { __VA_ARGS__ }, expected ); }


TEST(-1, 0)
TEST(0, 0, 0)
TEST(-1, 1, 0)
TEST(1, 1, 0, 1, 2, 3)
TEST(-1, 1, 0, 2, 4, 6)
TEST(1, 2, 0, 2, 4, 6)
TEST(0, 0, 0, 2, 4, 6)
TEST(3, 6, 0, 2, 4, 6)
TEST(-1, 7, 0, 2, 4, 6)
TEST(-1, 8, 0, 2, 4, 6)
TEST(-1, 0, 1, 2, 4, 5, 7)
TEST(0, 1, 1, 2, 4, 5, 7)
TEST(1, 2, 1, 2, 4, 5, 7)
TEST(-1, 3, 1, 2, 4, 5, 7)
TEST(2, 4, 1, 2, 4, 5, 7)
TEST(3, 5, 1, 2, 4, 5, 7)
TEST(-1, 6, 1, 2, 4, 5, 7)
TEST(4, 7, 1, 2, 4, 5, 7)
TEST(-1, 8, 1, 2, 4, 5, 7)

#endif
