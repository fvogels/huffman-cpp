#include "encoding/huffman/decoding.h"


void encoding::huffman::decode_bits(io::InputStream& input, const data::Node<Datum>& tree, io::OutputStream& output, u64 domain_size, Datum eof)
{
    //const Datum eof = domain_size + 1;
    //const data::Node<Datum>* current_node = &tree;
    //bool end_reached = false;

    //while (!end_reached)
    //{
    //    assert(!input.end_reached());
    //    auto bit = input.read();

    //    assert(current_node->is_branch());

    //    auto branch = static_cast<const data::Branch<Datum>*>(current_node);

    //    if (bit == 0)
    //    {
    //        current_node = &branch->left_child();
    //    }
    //    else
    //    {
    //        current_node = &branch->right_child();
    //    }

    //    if (current_node->is_leaf())
    //    {
    //        auto leaf = static_cast<const data::Leaf<Datum>*>(current_node);
    //        auto datum = leaf->value();

    //        if (datum == eof)
    //        {
    //            end_reached = true;
    //        }
    //        else
    //        {
    //            output.write(leaf->value());
    //            current_node = &tree;
    //        }
    //    }
    //}

    Datum datum;

    while ((datum = encoding::huffman::decode_single_datum(input, tree, domain_size)) != eof)
    {
        output.write(datum);
    }
}

Datum encoding::huffman::decode_single_datum(io::InputStream& input, const data::Node<Datum>& tree, u64 domain_size)
{
    const Datum eof = domain_size + 1;
    const data::Node<Datum>* current_node = &tree;

    while (true)
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

            return datum;
        }
    }
}
