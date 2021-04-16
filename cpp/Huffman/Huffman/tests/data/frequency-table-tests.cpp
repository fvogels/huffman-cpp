#ifdef TEST_BUILD

#pragma warning(disable : 26444)

#include "catch.hpp"
#include "data/frequency-table.h"


namespace
{
    template<typename T>
    data::FrequencyTable<T> create_empty_table()
    {
        return data::FrequencyTable<T>();
    }
}


TEMPLATE_TEST_CASE("Values of empty FrequencyTable<T>", "", int, u64, unsigned)
{
    auto ft = create_empty_table<TestType>();
    auto values = ft.values();

    REQUIRE(values.size() == 0);
}

TEMPLATE_TEST_CASE("Adding to domain", "", int, u64, unsigned)
{
    auto ft = create_empty_table<TestType>();
    ft.add_to_domain(5);
    auto values = ft.values();

    REQUIRE(ft[5] == 0);
    REQUIRE(values.size() == 1);
    REQUIRE(values[0] == 5);
}

TEMPLATE_TEST_CASE("Values of frequency table {1 => 1}", "", int, u64, unsigned)
{
    auto ft = create_empty_table<TestType>();
    ft.increment(1);
    auto values = ft.values();

    REQUIRE(values.size() == 1);
    REQUIRE(values[0] == 1);
}

TEMPLATE_TEST_CASE("Default frequency is zero", "", int, u64, unsigned)
{
    auto ft = create_empty_table<TestType>();

    REQUIRE(ft[0] == 0);
}

TEMPLATE_TEST_CASE("Reading from const table", "", int, u64, unsigned)
{
    const auto ft = create_empty_table<TestType>();

    REQUIRE(ft[0] == 0);
}

#endif