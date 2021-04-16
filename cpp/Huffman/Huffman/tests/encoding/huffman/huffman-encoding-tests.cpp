#ifdef TEST_BUILD

#include "catch.hpp"
#include "util.h"
#include "encoding/huffman/huffman-encoding.h"
#include "encoding/huffman/adaptive-huffman-encoding.h"
#include "io/memory-buffer.h"


namespace
{
    template<u64 IN>
    void check(const std::vector<Datum>& data, encoding::Encoding<IN, 2> encoding)
    {
        io::MemoryBuffer<IN, Datum> buffer1(data);
        io::MemoryBuffer<2> buffer2;
        io::MemoryBuffer<IN> buffer3;

        encoding::encode(buffer1.source(), encoding, buffer2.destination());
        encoding::decode(buffer2.source(), encoding, buffer3.destination());

        auto results = buffer3.data();

        REQUIRE(data.size() == results->size());

        for (size_t i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*results)[i]);
        }
    }
}

#define TESTN(N, ...) TEST_CASE("Huffman Encoding (DS=" #N ") on { " #__VA_ARGS__ " }") { check<N>(std::vector<Datum> { __VA_ARGS__ }, encoding::huffman_encoding<N>()); } \
                      TEST_CASE("Adaptive Huffman Encoding (DS=" #N ") on { " #__VA_ARGS__ " }") { check<N>(std::vector<Datum> { __VA_ARGS__ }, encoding::adaptive_huffman<N>()); }


#define TEST4(...)   TESTN(4, __VA_ARGS__)
#define TEST16(...)  TESTN(16, __VA_ARGS__)
#define TEST256(...) TESTN(256, __VA_ARGS__)

TEST4(0, 1)
TEST256(0, 1)
TEST256(1, 2)
TEST256(1, 1, 2)
TEST256(1, 1, 2, 2)
TEST256(1, 2, 3, 2, 1)
TEST256(1, 1, 2, 3, 3, 4, 4, 4, 4, 3, 2, 1, 2, 3, 4)

#endif
