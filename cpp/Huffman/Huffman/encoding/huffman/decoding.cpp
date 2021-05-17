#include "encoding/huffman/decoding.h"


void encoding::huffman::decode_bits(io::InputStream& input, const data::Node<Datum>& tree, io::OutputStream& output)
{
    while (!input.end_reached())
    {
        auto datum = encoding::huffman::decode_single_datum(input, tree);
        output.write(datum);
    }
}

Datum encoding::huffman::decode_single_datum(io::InputStream& input, const data::Node<Datum>& tree)
{
    const data::Node<Datum>* current_node = &tree;

    while (!input.end_reached())
    {
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

            return datum;
        }
    }

    return 0;
}
