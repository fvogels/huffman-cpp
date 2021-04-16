#ifdef TEST_BUILD

#include "catch.hpp"
#include "util.h"
#include "encoding/huffman/tree-encoding.h"
#include "io/memory-buffer.h"


namespace
{
    void check(const data::Node<Datum>& tree, unsigned bits_per_datum)
    {
        io::MemoryBuffer<256> buffer;
        auto input = buffer.source()->create_input_stream();
        auto output = buffer.destination()->create_output_stream();

        encoding::huffman::encode_tree(tree, bits_per_datum, *output);
        auto result = encoding::huffman::decode_tree(bits_per_datum, *input);

        REQUIRE(tree.equal_to(*result));
    }

    std::unique_ptr<data::Node<Datum>> L(Datum datum)
    {
        return std::make_unique<data::Leaf<Datum>>(datum);
    }

    std::unique_ptr<data::Node<Datum>> B(std::unique_ptr<data::Node<Datum>> left, std::unique_ptr<data::Node<Datum>> right)
    {
        return std::make_unique<data::Branch<Datum>>(std::move(left), std::move(right));
    }
}

#define TEST(t, bpd)    TEST_CASE("Encoding/decoding tree " #t ", " #bpd " bits per datum")    \
                        {                                                                      \
                            auto tree = t;                                                     \
                            auto bits_per_datum = bpd;                                         \
                            check(*tree, bits_per_datum);                                      \
                        }

TEST(L(0), 8)
TEST(L(0), 16)
TEST(L(15), 8)
TEST(L(15), 16)
TEST(B(L(0), L(1)), 16)
TEST(B(B(L(0), L(1)), L(2)), 4)

#endif
