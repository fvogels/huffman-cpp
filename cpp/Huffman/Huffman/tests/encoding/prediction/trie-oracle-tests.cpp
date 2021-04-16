#ifdef TEST_BUILD

#include "encoding/predictive/trie-oracle.h"
#include "catch.hpp"



namespace
{
    void tell(encoding::predictive::Oracle& oracle, const std::vector<Datum>& data)
    {
        for (auto& datum : data)
        {
            oracle.tell(datum);
        }
    }
}

#define TELL(oracle, ...) tell(oracle, std::vector<Datum> { __VA_ARGS__ } )


TEST_CASE("Trie Oracle (depth 1), initial prediction")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    REQUIRE(oracle->predict() == 0);
}

TEST_CASE("Trie Oracle (depth 1), [1,1] -> 1")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    TELL(*oracle, 1, 1);
    REQUIRE(oracle->predict() == 1);
}

TEST_CASE("Trie Oracle (depth 1), [1,2,1] -> 2")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    TELL(*oracle, 1, 2, 1);
    REQUIRE(oracle->predict() == 2);
}

TEST_CASE("Trie Oracle (depth 1), [1,1,1,1,2,1,2,1,2,1,2,1] -> 2")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    TELL(*oracle, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1);
    REQUIRE(oracle->predict() == 2);
}

TEST_CASE("Trie Oracle (depth 1), [1,1,1,1,2,1,2,1,2,1,2,1,2] -> 1")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    TELL(*oracle, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2);
    REQUIRE(oracle->predict() == 1);
}

TEST_CASE("Trie Oracle (depth 1), [1,2,3,1,2,3,1,2,3,1] -> 2")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    TELL(*oracle, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1);
    REQUIRE(oracle->predict() == 2);
}

TEST_CASE("Trie Oracle (depth 1), [1,2,3,1,2,3,1,2,3,1,2] -> 3")
{
    auto oracle = encoding::predictive::trie_oracle(1);

    TELL(*oracle, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2);
    REQUIRE(oracle->predict() == 3);
}

TEST_CASE("Trie Oracle (depth 2), [1,2,3,1,2,3,1,2,3,1,2] -> 3")
{
    auto oracle = encoding::predictive::trie_oracle(2);

    TELL(*oracle, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2);
    REQUIRE(oracle->predict() == 3);
}

TEST_CASE("Trie Oracle (depth 2), [1,2,1,1,2,1,1,2,1,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3,1,2] -> 1")
{
    auto oracle = encoding::predictive::trie_oracle(2);

    TELL(*oracle, 1, 2, 1, 1, 2, 1, 1, 2, 1, 3, 2, 3, 3, 2, 3, 3, 2, 3, 3, 2, 3, 3, 2, 3, 1, 2);
    REQUIRE(oracle->predict() == 1);
}

TEST_CASE("Trie Oracle (depth 2), [1,2,1,1,2,1,1,2,1,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3,3,2] -> 3")
{
    auto oracle = encoding::predictive::trie_oracle(2);

    TELL(*oracle, 1, 2, 1, 1, 2, 1, 1, 2, 1, 3, 2, 3, 3, 2, 3, 3, 2, 3, 3, 2, 3, 3, 2, 3, 3, 2);
    REQUIRE(oracle->predict() == 3);
}

#endif
