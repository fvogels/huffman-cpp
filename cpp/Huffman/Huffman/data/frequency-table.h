#ifndef FREQUENCY_TABLE_H
#define FREQUENCY_TABLE_H

#include "util.h"
#include <memory>
#include <vector>
#include <map>


namespace data
{
    template<typename T>
    class FrequencyTable
    {
    private:
        std::map<T, u64> frequencies;

    public:
        void increment(const T& value)
        {
            this->frequencies[value]++;
        }

        u64 operator[](const T& value) const
        {
            auto it = this->frequencies.find(value);

            if (it == this->frequencies.end())
            {
                return 0;
            }
            else
            {
                return it->second;
            }
        }
                
        std::vector<T> values() const
        {
            std::vector<T> result;

            for (auto it : this->frequencies)
            {
                result.push_back(it.first);
            }

            return result;
        }

        void add_to_domain(const T& value)
        {
            auto it = this->frequencies.find(value);

            if (it == this->frequencies.end())
            {
                this->frequencies[value] = 0;
            }
        }
    };

    //template<>
    //class FrequencyTable<u64>
    //{
    //private:
    //    std::unique_ptr<u64[]> frequencies;
    //    std::unique_ptr<bool[]> domain;
    //    u64 domain_size;

    //public:
    //    FrequencyTable(u64 domain_size) : frequencies(std::make_unique<u64[]>(domain_size)), domain(std::make_unique<bool[]>(domain_size)), domain_size(domain_size)
    //    {
    //        for (u64 i = 0; i != domain_size; ++i)
    //        {
    //            frequencies[i] = 0;
    //            domain[i] = false;
    //        }
    //    }

    //    u64 operator[](u64 value) const
    //    {
    //        return this->frequencies[value];
    //    }

    //    void increment(u64 value)
    //    {
    //        this->domain[value] = true;
    //        this->frequencies[value]++;
    //    }

    //    void add_to_domain(u64 value)
    //    {
    //        this->domain[value] = true;
    //    }

    //    std::vector<u64> values() const
    //    {
    //        std::vector<u64> result;

    //        for (u64 i = 0; i != this->domain_size; ++i)
    //        {
    //            if (this->domain[i])
    //            {
    //                result.push_back(i);
    //            }
    //        }

    //        return result;
    //    }
    //};

    template<typename T>
    data::FrequencyTable<T> count_frequencies(const std::vector<T>& xs)
    {
        data::FrequencyTable<T> result;

        for (auto& x : xs)
        {
            result.increment(x);
        }

        return result;
    }
}

#endif
