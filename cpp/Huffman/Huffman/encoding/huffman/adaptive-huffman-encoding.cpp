#include "encoding/huffman/adaptive-huffman-encoding.h"
#include "encoding/huffman/tree-encoding.h"
#include "encoding/huffman/tree-building.h"
#include "encoding/huffman/code-building.h"
#include "encoding/huffman/decoding.h"
#include "data/frequency-table.h"
#include "data/binary-tree.h"
#include "io/memory-buffer.h"
#include "io/streams.h"
#include "io/io-util.h"
#include "binary/binutil.h"
#include "defs.h"
#include <assert.h>
#include <utility>
#include <memory>

namespace
{
    class AdaptiveHuffmanEncodingImplementation : public encoding::EncodingImplementation
    {
        u64 m_domain_size;
        unsigned m_bits_per_datum;
        Datum m_eof;
        Datum m_nyt;

    public:
        AdaptiveHuffmanEncodingImplementation(u64 domain_size) : m_domain_size(domain_size), m_eof(domain_size), m_nyt(domain_size + 1), m_bits_per_datum(binary::bits_needed(domain_size + 2)) // +1 for eof, // +1 for nyt
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            data::FrequencyTable<Datum> frequencies;
            frequencies.add_to_domain(m_eof);
            frequencies.add_to_domain(m_nyt);

            while (!input.end_reached())
            {
                auto datum = input.read();
                auto tree = encoding::huffman::build_tree(frequencies);
                auto codes = encoding::huffman::build_codes(*tree, m_domain_size);

                if (codes[datum].size() == 0) // Code length 0 is impossible since EOF and NYT are guaranteed to be included in the tree
                {
                    io::transfer(codes[m_nyt], output);
                    io::write_bits(datum, m_bits_per_datum, output);
                }
                else
                {
                    io::transfer(codes[datum], output);
                }

                frequencies.increment(datum);
            }
        }

        void decode(io::InputStream& input, io::OutputStream& output) const override
        {
            data::FrequencyTable<Datum> frequencies;
            frequencies.add_to_domain(m_eof);
            frequencies.add_to_domain(m_nyt);

            while (true)
            {
                auto tree = encoding::huffman::build_tree(frequencies);
                auto datum = encoding::huffman::decode_single_datum(input, *tree, m_domain_size + 2);
                
                if (datum == m_eof)
                {
                    return;
                }
                else if (datum == m_nyt)
                {
                    transfer(input, output, m_bits_per_datum);
                }
                else
                {
                    output.write(datum);
                }

                frequencies.increment(datum);
            }
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_adaptive_huffman_implementation(u64 domain_size)
{
    return std::make_shared<AdaptiveHuffmanEncodingImplementation>(domain_size);
}
