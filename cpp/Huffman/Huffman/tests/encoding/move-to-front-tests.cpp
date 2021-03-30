#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"


namespace
{
    void check(const Data& data)
    {
        auto encoding = encoding::move_to_front_encoding(256);
        auto encoded_data = encoding->encode(data);
        auto decoded_data = encoding->decode(encoded_data);

        REQUIRE(data.size() == decoded_data.size());

        for (auto i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == decoded_data[i]);
        }
    }

    void check2(const Data& data)
    {
        auto encoding = encoding::move_to_front_encoding_fast(256);
        auto encoded_data = encoding->encode(data);
        auto decoded_data = encoding->decode(encoded_data);

        REQUIRE(data.size() == decoded_data.size());

        for (auto i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == decoded_data[i]);
        }
    }
}


#define TEST(...) TEST_CASE("Move To Front: " #__VA_ARGS__) { check( Data { __VA_ARGS__ } ); } \
                  TEST_CASE("Move To Front 2: " #__VA_ARGS__) { check2( Data { __VA_ARGS__ } ); }

TEST()
TEST(0)
TEST(1)
TEST(1, 2)
TEST(1, 1, 1, 1, 1)
TEST(1, 2, 3, 4, 1, 2, 3, 4)
TEST(1, 2, 3, 4, 5, 4, 3, 2, 1)

#endif