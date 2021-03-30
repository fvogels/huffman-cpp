#include "encoding/move-to-front.h"
#include "defs.h"
#include <deque>
#include <assert.h>
#include <numeric>
#include <memory>


namespace
{
    class MoveToFrontEncoding : public encoding::Encoding<Data, Data>
    {
        u64 domain_size;

    public:
        MoveToFrontEncoding(u64 domain_size) : domain_size(domain_size)
        {
            // NOP
        }

        virtual Data encode(const Data& input) const
        {
            Data output;

            std::deque<Datum> table;
            add_range<Datum>(table, 0, this->domain_size);

            for (auto& datum : input)
            {
                unsigned index = 0;
                auto it = table.begin();

                while (*it != datum)
                {
                    ++it;
                    ++index;
                }

                table.erase(it);
                table.push_front(datum);
                output.push_back(index);
            }

            assert(output.size() == input.size());

            return output;
        }

        virtual Data decode(const Data& indices) const
        {
            Data result;

            std::deque<Datum> table;
            add_range<Datum>(table, 0, this->domain_size);

            for (auto& index : indices)
            {
                auto datum = table[index];

                table.erase(table.begin() + index);
                table.push_front(datum);
                result.push_back(datum);
            }

            assert(indices.size() == result.size());

            return result;
        }
    };

    struct NODE
    {
        Datum datum;
        NODE* next;
    };

    class FastMoveToFrontEncoding : public encoding::Encoding<Data, Data>
    {
        u64 domain_size;

    public:
        FastMoveToFrontEncoding(u64 domain_size) : domain_size(domain_size)
        {
            // NOP
        }

        virtual Data encode(const Data& input) const
        {
            Data output;
            auto table = this->create_initial_table();

            for (auto& datum : input)
            {
                u64 i = 0;
                NODE* last = &table[0];
                NODE* p = table[0].next;

                while (p->datum != datum)
                {
                    ++i;
                    last = p;
                    p = p->next;
                }

                output.push_back(i);
                last->next = p->next;
                p->next = table[0].next;
                table[0].next = p;
            }

            assert(output.size() == input.size());

            return output;
        }

        virtual Data decode(const Data& indices) const
        {
            Data result;
            auto table = this->create_initial_table();

            for (auto& index : indices)
            {
                NODE* last = &table[0];
                NODE* p = table[0].next;

                for (u64 i = 0; i < index; ++i)
                {
                    last = p;
                    p = p->next;
                }

                result.push_back(p->datum);
                last->next = p->next;
                p->next = table[0].next;
                table[0].next = p;
            }

            assert(indices.size() == result.size());

            return result;
        }

    private:
        std::unique_ptr<NODE[]> create_initial_table() const
        {
            std::unique_ptr<NODE[]> table = std::make_unique<NODE[]>(this->domain_size + 1);

            for (u64 i = 0; i != this->domain_size; ++i)
            {
                table[i].next = &table[i + 1];
                table[i + 1].datum = i;
            }

            return table;
        }
    };
}

std::shared_ptr<encoding::Encoding<Data, Data>> encoding::move_to_front_encoding(u64 domain_size)
{
    return std::make_shared<MoveToFrontEncoding>(domain_size);
}

std::shared_ptr<encoding::Encoding<Data, Data>> encoding::move_to_front_encoding_fast(u64 domain_size)
{
    return std::make_shared<FastMoveToFrontEncoding>(domain_size);
}