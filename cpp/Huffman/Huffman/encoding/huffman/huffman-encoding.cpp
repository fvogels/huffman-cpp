#include "encoding/huffman/huffman-encoding.h"
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
    class HuffmanEncodingImplementation : public encoding::EncodingImplementation
    {
        u64 m_domain_size;
        unsigned m_bits_per_datum;

    public:
        HuffmanEncodingImplementation(u64 domain_size) : m_domain_size(domain_size), m_bits_per_datum(binary::bits_needed(domain_size))
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            auto copy = copy_to_vector(input);
            auto frequencies = data::count_frequencies(copy);
            auto tree = encoding::huffman::build_tree(frequencies);
            auto codes = encoding::huffman::build_codes(*tree, m_domain_size + 1);

            encoding::huffman::encode_tree(*tree, m_bits_per_datum, output);
            this->encode_input(copy, codes, output);
        }

        void decode(io::InputStream& input, io::OutputStream& output) const override
        {
            auto tree = encoding::huffman::decode_tree(m_bits_per_datum, input);
            encoding::huffman::decode_bits(input, *tree, output, m_bits_per_datum);
        }

    private:
        std::vector<Datum> copy_to_vector(io::InputStream& input) const
        {
            std::vector<Datum> result;

            while (!input.end_reached())
            {
                result.push_back(input.read());
            }

            return result;
        }
        
        void encode_input(const std::vector<Datum>& input, std::vector<std::vector<Datum>>& codes, io::OutputStream& output) const
        {
            for ( auto& datum : input )
            {
                auto code = codes[datum];
                io::transfer(code, output);
            }
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_huffman_implementation(u64 domain_size)
{
    return std::make_shared<HuffmanEncodingImplementation>(domain_size);
}
