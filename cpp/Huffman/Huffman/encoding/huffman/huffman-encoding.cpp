#include "encoding/huffman/huffman-encoding.h"
#include "encoding/huffman/tree-encoding.h"
#include "data-structures/frequency-table.h"
#include "data-structures/binary-tree.h"
#include "io/input-stream.h"
#include "io/io-util.h"
#include "binary/binutil.h"
#include "defs.h"
#include <algorithm>
#include <assert.h>
#include <utility>
#include <memory>

namespace
{
    typedef std::pair<Datum, u64> wpair;
    typedef data::Node<wpair>     wnode;
    typedef data::Leaf<wpair>     wleaf;
    typedef data::Branch<wpair>   wbranch;

    class HuffmanEncoding : public encoding::EncodingImplementation<Datum, Datum>
    {
        u64 m_domain_size;
        unsigned m_bits_per_datum;

    public:
        HuffmanEncoding(u64 domain_size) : m_domain_size(domain_size), m_bits_per_datum(binary::bits_needed(domain_size))
        {
            // NOP
        }

        void encode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const override
        {
            const Datum eof = m_domain_size;
            auto copy = copy_to_vector(input);
            copy.push_back(eof);
            auto frequencies = data::count_frequencies(copy);
            auto tree = this->build_tree(frequencies);
            auto codes = this->build_codes(*tree);

            this->encode_tree(*tree, output);
            this->encode_input(copy, codes, output);
        }

        void decode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const override
        {
            auto tree = this->decode_tree(input);
            this->decode_bits(input, *tree, output);
        }

    private:
        void decode_bits(io::InputStream<Datum>& input, const data::Node<Datum>& tree, io::OutputStream<Datum>& output) const
        {
            const Datum eof = m_domain_size;
            const data::Node<Datum>* current_node = &tree;
            bool end_reached = false;

            while (!end_reached)
            {
                assert(!input.end_reached());
                auto& bit = input.read();

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

        std::vector<Datum> copy_to_vector(io::InputStream<Datum>& input) const
        {
            std::vector<Datum> result;

            while (!input.end_reached())
            {
                result.push_back(input.read());
            }

            return result;
        }

        std::vector<std::unique_ptr<wnode>> create_nodes(const data::FrequencyTable<Datum>& frequencies) const
        {
            std::vector<std::unique_ptr<wnode>> result;

            for (auto& datum : frequencies.values())
            {
                auto frequency = frequencies[datum];
                wpair pair(datum, frequency);
                std::unique_ptr<wnode> node = std::make_unique<wleaf>(pair);
                result.push_back(std::move(node));
            }
            
            return result;
        }

        void sort_queue(std::vector<std::unique_ptr<wnode>>& nodes) const
        {
            std::sort(nodes.begin(), nodes.end(), [](const std::unique_ptr<wnode>& p, const std::unique_ptr<wnode>& q) {
                return weight(*p) > weight(*q); // sort from heavy to light!
            });
        }

        static u64 weight(const wnode& node)
        {
            if (node.is_leaf())
            {
                const wleaf& leaf = static_cast<const wleaf&>(node);
                return leaf.value().second;
            }
            else
            {
                const wbranch& branch = static_cast<const wbranch&>(node);
                return weight(branch.left_child()) + weight(branch.right_child());
            }
        }

        std::unique_ptr<data::Node<Datum>> build_tree(const data::FrequencyTable<Datum>& frequencies) const
        {
            auto queue = this->create_nodes(frequencies);
            assert(queue.size() > 0);

            while (queue.size() > 1)
            {
                this->sort_queue(queue);

                assert(weight(*queue[0]) >= weight(*queue[1]));

                auto left_node = std::move(queue.back());
                queue.pop_back();
                auto right_node = std::move(queue.back());
                queue.pop_back();

                auto branch = std::make_unique<wbranch>(std::move(left_node), std::move(right_node));

                queue.push_back(std::move(branch));
            }

            assert(queue.size() == 1);
            auto tree = std::move(queue.back());

            return data::map<wpair, Datum>(*tree, [](const wpair& pair) -> Datum { return pair.first; });
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

        void encode_tree(const data::Node<Datum>& tree, io::OutputStream<Datum>& output) const
        {
            encoding::encode_tree(tree, m_bits_per_datum, output);
        }

        std::unique_ptr<data::Node<Datum>> decode_tree(io::InputStream<Datum>& input) const
        {
            return encoding::decode_tree(m_bits_per_datum, input);
        }

        void encode_input(const std::vector<Datum>& input, std::vector<std::vector<Datum>>& codes, io::OutputStream<Datum>& output) const
        {
            for ( auto& datum : input )
            {
                auto code = codes[datum];
                io::transfer(code, output);
            }
        }
    };
}

std::unique_ptr<encoding::EncodingImplementation<Datum, Datum>> encoding::create_huffman_encoder(unsigned domain_size)
{
    return std::make_unique<HuffmanEncoding>(domain_size);
}
