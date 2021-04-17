#ifdef TEST_BUILD

#include "io/binary-io.h"
#include "io/memory-buffer.h"
#include "util.h"
#include "catch.hpp"


namespace
{
    void check(u64 n, unsigned nbits)
    {
        io::MemoryBuffer<655365, Datum> buffer;
        auto input = buffer.source()->create_input_stream();
        auto output = buffer.destination()->create_output_stream();
        io::write_bits(n, nbits, *output);
        auto result = io::read_bits(nbits, *input);

        REQUIRE(n == result);
    }
}

#define TEST(n, nbits) TEST_CASE("Converting " #n " to binary and back (" #nbits " bits)") { check(n, nbits); }


TEST(0, 1)
TEST(1, 1)
TEST(0, 8)
TEST(1, 8)
TEST(2, 8)
TEST(3, 8)
TEST(135, 8)
TEST(255, 8)
TEST(255, 16)
TEST(256, 16)
TEST(4876, 16)
TEST(23468, 16)

#endif
