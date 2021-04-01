#include "encoding/huffman/tree-encoding.h"
#include "encoding/encoding.h"
#include "binary/binutil.h"
#include "defs.h"


void encoding::encode_tree(const data::Node<Datum>& tree, unsigned bits_per_datum, io::OutputStream<Datum>& output)
{
    if (tree.is_branch())
    {
        auto& branch = static_cast<const data::Branch<Datum>&>(tree);
        output.write(0);
        encode_tree(branch.left_child(), bits_per_datum, output);
        encode_tree(branch.right_child(), bits_per_datum, output);
    }
    else
    {
        auto& leaf = static_cast<const data::Leaf<Datum>&>(tree);
        auto datum = leaf.value();
        output.write(1);
        binary::write_bits(datum, bits_per_datum, output);
    }
}

std::unique_ptr<data::Node<Datum>> encoding::decode_tree(unsigned bits_per_datum, io::InputStream<Datum>& input)
{
    if (input.read() == 0)
    {
        auto left_child = decode_tree(bits_per_datum, input);
        auto right_child = decode_tree(bits_per_datum, input);

        return std::make_unique<data::Branch<Datum>>(std::move(left_child), std::move(right_child));
    }
    else
    {
        auto datum = binary::read_bits(bits_per_datum, input);

        return std::make_unique<data::Leaf<Datum>>(datum);
    }
}
