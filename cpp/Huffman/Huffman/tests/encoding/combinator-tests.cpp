#ifdef TEST_BUILD

#include "catch.hpp"
#include "defs.h"
#include "encoding/encodings.h"
#include "io/buffer.h"
#include "io/io-util.h"
#include <functional>


namespace
{
    template<typename IN, typename OUT>
    void check(const std::vector<IN>& data, encoding::Encoding<IN, OUT> encoding)
    {
        io::Buffer<IN> buffer1(data);
        io::Buffer<OUT> buffer2;
        io::Buffer<IN> buffer3;

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


#define TEST(encoding, type, ...) TEST_CASE("Pipeline " #encoding " on " #__VA_ARGS__) { check(std::vector<type> { __VA_ARGS__ }, (encoding)); }

#define TEST_DATUMS(encoding) \
    TEST(encoding, Datum, 0) \
    TEST(encoding, Datum, 1) \
    TEST(encoding, Datum, 0, 0) \
    TEST(encoding, Datum, 0, 1) \
    TEST(encoding, Datum, 1, 0) \
    TEST(encoding, Datum, 1, 1) \
    TEST(encoding, Datum, 5) \
    TEST(encoding, Datum, 5, 5, 5, 5, 5, 5) \
    TEST(encoding, Datum, 97) \
    TEST(encoding, Datum, 97, 98, 97, 98, 97, 98)

#define TEST_BYTES(encoding) \
    TEST(encoding, uint8_t, 0) \
    TEST(encoding, uint8_t, 1) \
    TEST(encoding, uint8_t, 0, 0) \
    TEST(encoding, uint8_t, 0, 1) \
    TEST(encoding, uint8_t, 1, 0) \
    TEST(encoding, uint8_t, 1, 1) \
    TEST(encoding, uint8_t, 5) \
    TEST(encoding, uint8_t, 5, 5, 5, 5, 5, 5) \
    TEST(encoding, uint8_t, 97) \
    TEST(encoding, uint8_t, 97, 98, 97, 98, 97, 98)

TEST_DATUMS(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256))
TEST_DATUMS(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256) | encoding::move_to_front_encoding(2))
TEST_DATUMS(encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8))
TEST_DATUMS(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8))
TEST_DATUMS(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8) | ~encoding::create_datum_encoding())
TEST_DATUMS(encoding::move_to_front_encoding(256) | encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(7) | ~encoding::create_datum_encoding())
TEST_DATUMS(encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8) | ~encoding::create_datum_encoding())

TEST_BYTES(encoding::create_datum_encoding() | encoding::move_to_front_encoding(256) | ~encoding::create_datum_encoding());
TEST_BYTES(encoding::create_datum_encoding() | encoding::create_huffman_encoder(256) | ~encoding::create_datum_encoding());
TEST_BYTES(encoding::create_datum_encoding() | encoding::create_huffman_encoder(256) | encoding::create_bit_grouper(8) | ~encoding::create_datum_encoding());

#endif
