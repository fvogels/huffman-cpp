#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"
#include "encoding/predictive/oracles.h"
#include "io/memory-buffer.h"
#include "io/io-util.h"
#include <functional>


namespace
{
    void check(std::unique_ptr<encoding::predictive::Oracle> oracle, const std::vector<Datum>& data)
    {
        auto encoding = encoding::predictive_encoder<256>(std::move(oracle));
        io::MemoryBuffer<256, Datum> buffer1(data);
        io::MemoryBuffer<256> buffer2;
        io::MemoryBuffer<256> buffer3;

        encoding::encode(buffer1.source(), encoding, buffer2.destination());
        encoding::decode(buffer2.source(), encoding, buffer3.destination());

        auto decoded_data = buffer3.data();

        REQUIRE(data.size() == decoded_data->size());

        for (auto i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*decoded_data)[i]);
        }
    }

    std::unique_ptr<encoding::predictive::Oracle> constant(Datum datum)
    {
        return encoding::predictive::constant_oracle(datum);
    }

    std::unique_ptr<encoding::predictive::Oracle> repeat(Datum datum)
    {
        return encoding::predictive::repeating_oracle(datum);
    }
}


#define TEST_ORACLE(oracle, ...) TEST_CASE("Predictive Encoding with " #oracle ": " #__VA_ARGS__) { check(oracle, std::vector<Datum> { __VA_ARGS__ } ); }

#define TEST(...) \
    TEST_ORACLE(constant(0), __VA_ARGS__) \
    TEST_ORACLE(constant(1), __VA_ARGS__) \
    TEST_ORACLE(repeat(0), __VA_ARGS__) \
    TEST_ORACLE(repeat(1), __VA_ARGS__)


TEST()
TEST(0)
TEST(1)
TEST(1, 2)
TEST(1, 1, 1, 1, 1)
TEST(1, 2, 3, 4, 1, 2, 3, 4)
TEST(1, 2, 3, 4, 5, 4, 3, 2, 1)

#endif
