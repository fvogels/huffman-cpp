#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"
#include "io/buffer.h"
#include "io/io-util.h"
#include <functional>


namespace
{
    void check(const std::vector<Datum>& data, std::function<std::unique_ptr<encoding::Encoding<Datum, Datum>>()> factory)
    {
        auto encoding = factory();
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

    void check_slow(const std::vector<Datum>& data)
    {
        std::function<std::unique_ptr<encoding::Encoding<Datum, Datum>>()> factory = []() {
            return encoding::move_to_front_encoding(256);
        };

        check(data, factory);
    }

    void check_fast(const std::vector<Datum>& data)
    {
        std::function<std::unique_ptr<encoding::Encoding<Datum, Datum>>()> factory = []() -> std::unique_ptr<encoding::Encoding<Datum, Datum>> {
            return encoding::move_to_front_encoding_fast(256);
        };

        check(data, factory);
    }
}


#define TEST(...) TEST_CASE("Move To Front: " #__VA_ARGS__) { check_slow( std::vector<Datum> { __VA_ARGS__ } ); } \
                  TEST_CASE("Move To Front (Fast version): " #__VA_ARGS__) { check_fast( std::vector<Datum> { __VA_ARGS__ } ); }

TEST()
TEST(0)
TEST(1)
TEST(1, 2)
TEST(1, 1, 1, 1, 1)
TEST(1, 2, 3, 4, 1, 2, 3, 4)
TEST(1, 2, 3, 4, 5, 4, 3, 2, 1)

#endif