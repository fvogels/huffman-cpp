#ifdef TEST_BUILD

#pragma warning(disable : 26444)

#include "catch.hpp"
#include "data/trie.h"
#include <algorithm>


TEST_CASE("Initialization of trie root data")
{
    data::Trie<char, int> trie;

    REQUIRE(trie.data == 0);
}

TEST_CASE("Child default data")
{
    data::Trie<char, int> trie;
    auto& child = trie['a'];

    REQUIRE(child.data == 0);
}

TEST_CASE("Grandchild default data")
{
    data::Trie<char, int> trie;
    auto& child = trie['a'];
    auto& grandchild = trie['b'];

    REQUIRE(grandchild.data == 0);
}

TEST_CASE("Enumerating keys (empty)")
{
    data::Trie<char, int> trie;
    auto keys = trie.keys();

    REQUIRE(keys.size() == 0);
}

TEST_CASE("Enumerating keys (one)")
{
    data::Trie<char, int> trie;
    trie['a'];
    auto keys = trie.keys();

    REQUIRE(keys.size() == 1);
    REQUIRE(keys[0] == 'a');
}

TEST_CASE("Enumerating keys (two)")
{
    data::Trie<char, int> trie;
    trie['a'];
    trie['b'];
    auto keys = trie.keys();

    REQUIRE(keys.size() == 2);
    REQUIRE(std::find(keys.begin(), keys.end(), 'a') != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), 'b') != keys.end());
}

#endif
