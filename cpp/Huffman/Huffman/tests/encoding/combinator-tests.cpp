#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"
#include "io/memory-buffer.h"
#include "io/io-util.h"
#include <functional>


using namespace encoding;

namespace
{
    template<u64 IN, u64 OUT>
    void check(const std::vector<u64>& data, Encoding<IN, OUT> encoding)
    {
        io::MemoryBuffer<IN, u64> buffer1(data);
        io::MemoryBuffer<OUT> buffer2;
        io::MemoryBuffer<IN> buffer3;

        encode(buffer1.source(), encoding, buffer2.destination());
        decode(buffer2.source(), encoding, buffer3.destination());

        auto decoded_data = buffer3.data();

        REQUIRE(data.size() == decoded_data->size());

        for (auto i = 0; i != data.size(); ++i)
        {
            REQUIRE(data[i] == (*decoded_data)[i]);
        }
    }
}


#define TEST(ENC, ...) TEST_CASE("Pipeline " #ENC " on " #__VA_ARGS__) { check(std::vector<Datum> { __VA_ARGS__ }, (ENC)); }

#define TEST_DATUMS(encoding) \
    TEST(encoding, 0) \
    TEST(encoding, 1) \
    TEST(encoding, 0, 0) \
    TEST(encoding, 0, 1) \
    TEST(encoding, 1, 0) \
    TEST(encoding, 1, 1) \
    TEST(encoding, 5) \
    TEST(encoding, 5, 5, 5, 5) \
    TEST(encoding, 5, 5, 5, 5, 5, 5) \
    TEST(encoding, 97) \
    TEST(encoding, 97, 98, 97, 98, 97, 98) \
    TEST(encoding, 255) \
    TEST(encoding, 255, 255) \
    TEST(encoding, 255, 255, 255) \
    TEST(encoding, 255, 255, 255, 255) \
    TEST(encoding, 255, 255, 255, 255, 255)

TEST_DATUMS(eof_encoding<256>() | move_to_front<257>() | huffman_encoding<257>() | move_to_front<2>())
TEST_DATUMS(eof_encoding<256>() | huffman_encoding<257>() | bit_grouper<8>())
TEST_DATUMS(eof_encoding<256>() | move_to_front<257>())
TEST_DATUMS(eof_encoding<256>() | move_to_front<257>() | huffman_encoding<257>())
TEST_DATUMS(eof_encoding<256>() | move_to_front<257>() | huffman_encoding<257>() | bit_grouper<8>())

#endif
