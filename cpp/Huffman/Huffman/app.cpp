#ifndef TEST_BUILD

#include "data-structures/frequency-table.h"
#include <random>
#include "easylogging++.h"

int main()
{
    Data data;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 255);

    for (u64 i = 0; i != 100'000'000; ++i)
    {
        data.push_back(distribution(generator));
    }

    {
        TIMED_SCOPE(s1, "1");
        data::FrequencyTable<u64> ft(256);

        for (auto x : data)
        {
            ft.increment(x);
        }
    }
}

#endif
