#include "encoding/huffman/huffman-encoding.h"
#include "encoding/huffman/tree-encoding.h"
#include "encoding/huffman/tree-builder.h"
#include "data/frequency-table.h"
#include "data/binary-tree.h"
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
        HuffmanEncodingImplementation(u64 domain_size) : m_domain_size(domain_size), m_bits_per_datum(binary::bits_needed(domain_size + 1)) // +1 for eof
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            const Datum eof = m_domain_size;
            auto copy = copy_to_vector(input);
            copy.push_back(eof);
            auto frequencies = data::count_frequencies(copy);
            auto tree = encoding::huffman::build_tree(frequencies);
            auto codes = this->build_codes(*tree);

            this->encode_tree(*tree, output);
            this->encode_input(copy, codes, output);
        }

        void decode(io::InputStream& input, io::OutputStream& output) const override
        {
            auto tree = this->decode_tree(input);
            this->decode_bits(input, *tree, output);
        }

    private:
        void decode_bits(io::InputStream& input, const data::Node<Datum>& tree, io::OutputStream& output) const
        {
            const Datum eof = m_domain_size;
            const data::Node<Datum>* current_node = &tree;
            bool end_reached = false;

            while (!end_reached)
            {
                assert(!input.end_reached());
                auto bit = input.read();

                assert(current_node->is_branch());

                auto branch = static_cast<const data::Branch<Datum>*>(current_node);
                
                if (bit == 0)
                {
                    current_node = &branch->left_child();
                }
                else
                {
                    current_node = &branch->right_child();
                }

                if (current_node->is_leaf())
                {
                    auto leaf = static_cast<const data::Leaf<Datum>*>(current_node);
                    auto datum = leaf->value();

                    if (datum == eof)
                    {
                        end_reached = true;
                    }
                    else
                    {
                        output.write(leaf->value());
                        current_node = &tree;
                    }
                }
            }
        }

        std::vector<Datum> copy_to_vector(io::InputStream& input) const
        {
            std::vector<Datum> result;

            while (!input.end_reached())
            {
                result.push_back(input.read());
            }

            return result;
        }
        
        std::vector<std::vector<Datum>> build_codes(const data::Node<Datum>& tree) const
        {
            std::vector<std::vector<Datum>> result(this->m_domain_size + 1); // +1 for EOF
            std::vector<Datum> prefix;
            build_codes(tree, prefix, &result);
            return result;
        }

        void build_codes(const data::Node<Datum>& node, std::vector<Datum>& prefix, std::vector<std::vector<Datum>>* result) const
        {
            if (node.is_branch())
            {
                auto& branch = static_cast<const data::Branch<Datum>&>(node);

                prefix.push_back(0);
                build_codes(branch.left_child(), prefix, result);
                prefix.pop_back();
                prefix.push_back(1);
                build_codes(branch.right_child(), prefix, result);
                prefix.pop_back();
            }
            else
            {
                auto& leaf = static_cast<const data::Leaf<Datum>&>(node);
                auto& datum = leaf.value();

                (*result)[datum] = prefix; // don't forget ()!
            }
        }

        void encode_tree(const data::Node<Datum>& tree, io::OutputStream& output) const
        {
            encoding::encode_tree(tree, m_bits_per_datum, output);
        }

        std::unique_ptr<data::Node<Datum>> decode_tree(io::InputStream& input) const
        {
            return encoding::decode_tree(m_bits_per_datum, input);
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
