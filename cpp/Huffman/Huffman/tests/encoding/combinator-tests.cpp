#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"
#include "io/buffer.h"
#include "io/io-util.h"
#include <functional>


namespace
{
    template<u64 IN, u64 OUT>
    void check(const std::vector<Datum>& data, encoding::Encoding<IN, OUT> encoding)
    {
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
    TEST(encoding, 5, 5, 5, 5, 5, 5) \
    TEST(encoding, 97) \
    TEST(encoding, 97, 98, 97, 98, 97, 98)

TEST_DATUMS(encoding::move_to_front_encoding<256>() | encoding::create_huffman_encoder<256>())
TEST_DATUMS(encoding::move_to_front_encoding<256>() | encoding::create_huffman_encoder<256>() | encoding::move_to_front_encoding<2>())
TEST_DATUMS(encoding::create_huffman_encoder<256>() | encoding::create_bit_grouper<8>())
TEST_DATUMS(encoding::move_to_front_encoding<256>() | encoding::create_huffman_encoder<256>() | encoding::create_bit_grouper<8>())

#endif
