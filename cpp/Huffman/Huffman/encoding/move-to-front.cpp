#include "encoding/move-to-front.h"
#include "util.h"
#include <deque>
#include <assert.h>
#include <numeric>
#include <memory>


namespace
{
    class MoveToFrontEncodingImplementation : public encoding::EncodingImplementation
    {
        u64 domain_size;

    public:
        MoveToFrontEncodingImplementation(u64 domain_size) : domain_size(domain_size)
        {
            // NOP
        }

        virtual void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            std::deque<Datum> table;
            add_range<Datum>(table, 0, this->domain_size);

            while ( !input.end_reached() )
            {
                auto datum = input.read();
                unsigned index = 0;
                auto it = table.begin();

                while (*it != datum)
                {
                    ++it;
                    ++index;
                }

                table.erase(it);
                table.push_front(datum);
                output.write(index);
            }
        }

        virtual void decode(io::InputStream& indices, io::OutputStream& result) const override
        {
            std::deque<Datum> table;
            add_range<Datum>(table, 0, this->domain_size);

            while (!indices.end_reached())
            {
                auto index = indices.read();
                auto datum = table[index];

                table.erase(table.begin() + index);
                table.push_front(datum);
                result.write(datum);
            }
        }
    };

    struct NODE
    {
        Datum datum;
        NODE* next;
    };

    class MoveToFrontEncodingFastImplementation : public encoding::EncodingImplementation
    {
        u64 domain_size;

    public:
        MoveToFrontEncodingFastImplementation(u64 domain_size) : domain_size(domain_size)
        {
            // NOP
        }

        virtual void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            auto table = this->create_initial_table();

            while (!input.end_reached())
            {
                auto datum = input.read();
                u64 i = 0;
                NODE* last = &table[0];
                NODE* p = table[0].next;

                while (p->datum != datum)
                {
                    ++i;
                    last = p;
                    p = p->next;
                }

                output.write(i);
                last->next = p->next;
                p->next = table[0].next;
                table[0].next = p;
            }
        }

        virtual void decode(io::InputStream& indices, io::OutputStream& result) const override
        {
            auto table = this->create_initial_table();

            while (!indices.end_reached())
            {
                auto index = indices.read();
                NODE* last = &table[0];
                NODE* p = table[0].next;

                for (u64 i = 0; i < index; ++i)
                {
                    last = p;
                    p = p->next;
                }

                result.write(p->datum);
                last->next = p->next;
                p->next = table[0].next;
                table[0].next = p;
            }
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

std::shared_ptr<encoding::EncodingImplementation> encoding::create_move_to_front_encoding_implementation(u64 domain_size)
{
    return std::make_shared<MoveToFrontEncodingImplementation>(domain_size);
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_move_to_front_encoding_fast_implementation(u64 domain_size)
{
    return std::make_shared<MoveToFrontEncodingFastImplementation>(domain_size);
}
