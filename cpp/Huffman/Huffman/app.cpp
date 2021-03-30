#ifndef TEST_BUILD

#include "encoding/encodings.h"
#include <random>
#include "easylogging++.h"

int main()
{
    TIMED_FUNC(tf);
    auto enc1 = encoding::move_to_front_encoding(256);
    auto enc2 = encoding::move_to_front_encoding2(256);
    Data data;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 255);

    for (u64 i = 0; i != 10'000'000; ++i)
    {
        data.push_back(distribution(generator));
    }

    {
        TIMED_SCOPE(s1, "1");
        auto enc = enc1->encode(data);
        auto dec = enc1->decode(enc);
    }
    {
        TIMED_SCOPE(s2, "2");
        auto enc = enc2->encode(data);
        auto dec = enc2->decode(enc);
    }
}

#endif
