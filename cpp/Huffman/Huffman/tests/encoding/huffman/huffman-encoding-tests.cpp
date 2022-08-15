#ifdef TEST_BUILD

#include "catch.hpp"
#include "util.h"
#include "encoding/huffman/huffman-encoding.h"
#include "encoding/huffman/adaptive-huffman-encoding.h"
#include "encoding/encodings.h"
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


namespace
{
    void check_compression(const std::string& input, size_t expected_bit_count)
    {
        auto original = io::MemoryBuffer<256>();
        auto output = io::MemoryBuffer<2>();

        for (auto c : input)
        {
            original.data()->push_back(c);
        }

        auto src = original.source();
        auto dst = output.destination();

        auto eof = encoding::eof_encoding<256>();
        auto huffman = encoding::huffman_encoding<257>();

        auto pipeline = eof | huffman;
        encoding::encode(src, pipeline, dst);

        REQUIRE(output.data()->size() == expected_bit_count);
    }

    void check_decompression(const std::string& input)
    {
        auto original = io::MemoryBuffer<256>();
        auto output = io::MemoryBuffer<2>();
        auto decompressed = io::MemoryBuffer<256>();

        for (auto c : input)
        {
            original.data()->push_back(c);
        }

        auto eof = encoding::eof_encoding<256>();
        auto huffman = encoding::huffman_encoding<257>();

        auto pipeline = eof | huffman;
        encoding::encode(original.source(), pipeline, output.destination());
        encoding::decode(output.source(), pipeline, decompressed.destination());

        REQUIRE(*original.data() == *decompressed.data());
    }

    void check_decompression_with_grouper(const std::string& input)
    {
        auto original = io::MemoryBuffer<256>();
        auto output = io::MemoryBuffer<256>();
        auto decompressed = io::MemoryBuffer<256>();

        for (auto c : input)
        {
            original.data()->push_back(c);
        }

        auto eof = encoding::eof_encoding<256>();
        auto huffman = encoding::huffman_encoding<257>();
        auto grouper = encoding::bit_grouper<8>();

        auto pipeline = eof | huffman | grouper;
        encoding::encode(original.source(), pipeline, output.destination());
        encoding::decode(output.source(), pipeline, decompressed.destination());

        REQUIRE(*original.data() == *decompressed.data());
    }
}

#define TEST_COMPRESSION_NBITS(str, bits)  TEST_CASE("Compressing " #str) { check_compression(str, bits); }
#define TEST_DECOMPRESSION(str)  TEST_CASE("Compressing/decompressing " #str) { check_decompression(str); check_decompression_with_grouper(str); }

TEST_COMPRESSION_NBITS("A", 1 + 1 + 9 + 1 + 9 + 1 + 1)
TEST_COMPRESSION_NBITS("AA", 1 + 1 + 9 + 1 + 9 + 1 + 1 + 1)
TEST_COMPRESSION_NBITS("AAA", 1 + 1 + 9 + 1 + 9 + 1 + 1 + 1 + 1)
TEST_COMPRESSION_NBITS("AAB", 1 + 1 + 9 + 1 + 1 + 9 + 1 + 9 + 1 + 1 + 2 + 2)
TEST_COMPRESSION_NBITS("AABB", 1 + 1 + 9 + 1 + 1 + 9 + 1 + 9 + 1 + 1 + 2 + 2 + 2)
TEST_COMPRESSION_NBITS("ABC", 1 + 1 + 1 + 9 + 1 + 9 + 1 + 1 + 9 + 1 + 9 + 2 + 2 + 2 + 2)

TEST_DECOMPRESSION("A")
TEST_DECOMPRESSION("AA")
TEST_DECOMPRESSION("AAA")
TEST_DECOMPRESSION("AB")
TEST_DECOMPRESSION("AAB")
TEST_DECOMPRESSION("AABB")
TEST_DECOMPRESSION("ABC")
TEST_DECOMPRESSION("ABCD")
TEST_DECOMPRESSION("ABCDE")
TEST_DECOMPRESSION("ABCDEF")
TEST_DECOMPRESSION("AABBCBACBABBBCDBABCBDBCBABCDBBABCBDBBBACBDBBCDB")
TEST_DECOMPRESSION(R"(

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<meta http-equiv="Content-Style-Type" content="text/css" />
<title>The Project Gutenberg EBook of A Tale of Two Cities, by Charles Dickens</title>
    <style type="text/css">

    body { margin-left: 20%; margin-right: 20%; background:#faebd0; text-align:justify}
    p { text-indent: 1em; margin-top: .25em; margin-bottom: .25em; }
    H1,H2,H3,H4,H5,H6 { text-align: center; margin-left: 15%; margin-right: 15%; }
    hr  { width: 50%; text-align: center;}
    div.fig    { display:block; margin:0 auto; text-align:center; }
    p.pfirst   {text-indent: 0}
    span.dropcap         { float: left; margin: 0 0.1em 0 0; line-height: .8 }

.no-break {page-break-before: avoid;} /* for epubs */

div.chapter {page-break-before: always; margin-top: 4em;}

p.letter {text-indent: 0%;
          margin-left: 10%;
          margin-right: 10%;
          margin-top: 1em;
          margin-bottom: 1em; }

a:link {color:blue; text-decoration:none}
a:visited {color:blue; text-decoration:none}
a:hover {color:red}

</style>
  </head>
  <body>

<div style='text-align:center; font-size:1.2em; font-weight:bold;'>The Project Gutenberg eBook of A Tale of Two Cities, by Charles Dickens</div>
<div style='display:block; margin:1em 0'>
This eBook is for the use of anyone anywhere in the United States and
most other parts of the world at no cost and with almost no restrictions
whatsoever. You may copy it, give it away or re-use it under the terms
)")

//
//TEST_CASE("Compression of { }")
//{
//    auto original = io::MemoryBuffer<256>(std::vector<uint8_t> { });
//    auto output = io::MemoryBuffer<2>();
//
//    auto src = original.source();
//    auto dst = output.destination();
//
//    auto eof = encoding::eof_encoding<256>();
//    auto huffman = encoding::huffman_encoding<257>();
//
//    auto pipeline = eof | huffman;
//    encoding::encode(src, pipeline, dst);
//
//    REQUIRE(output.data()->size() == 1 + 9);
//}
//
//
//TEST_CASE("Compression of { 'A' }")
//{
//    auto original = io::MemoryBuffer<256>(std::vector<uint8_t> { 'A' });
//    auto output = io::MemoryBuffer<2>();
//
//    auto src = original.source();
//    auto dst = output.destination();
//
//    auto eof = encoding::eof_encoding<256>();
//    auto huffman = encoding::huffman_encoding<257>();
//
//    auto pipeline = eof | huffman;
//    encoding::encode(src, pipeline, dst);
//
//    REQUIRE(output.data()->size() == 1 + 1 + 9 + 1 + 9 + 2);
//}

#endif
