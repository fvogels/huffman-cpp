#ifndef TREE_ENCODING_H
#define TREE_ENCODING_H

#include "io/output-stream.h"
#include "io/input-stream.h"
#include "data-structures/binary-tree.h"
#include <memory>


namespace encoding
{
    void encode_tree(const data::Node<Datum>& tree, unsigned bits_per_datum, io::OutputStream& output);
    std::unique_ptr<data::Node<Datum>> decode_tree(unsigned bits_per_datum, io::InputStream& input);
}

#endif
