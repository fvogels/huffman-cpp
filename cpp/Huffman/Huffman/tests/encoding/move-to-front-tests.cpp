#ifdef TEST_BUILD

#include "catch.hpp"
#include "util.h"
#include "encoding/encodings.h"
#include "io/memory-buffer.h"
#include "io/io-util.h"
#include <functional>


namespace
{
    void check(const std::vector<Datum>& data, std::function<encoding::Encoding<256, 256>()> factory)
    {
        auto encoding = factory();
        io::MemoryBuffer<256, Datum> buffer1(data);
        io::MemoryBuffer<256> buffer2;
        io::MemoryBuffer<256> buffer3;

        encoding::encode<256, 256>(buffer1.source(), encoding, buffer2.destination());
        encoding::decode<256, 256>(buffer2.source(), encoding, buffer3.destination());

        auto decoded_data = buffer3.data();

        REQUIRE(data.size() == decoded_data->size());

        for (auto i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*decoded_data)[i]);
        }
    }

    void check_slow(const std::vector<Datum>& data)
    {
        check(data, []() {
            return encoding::move_to_front<256>();
        });
    }

    void check_fast(const std::vector<Datum>& data)
    {
        check(data, []() {
            return encoding::move_to_front_fast<256>();
        });
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