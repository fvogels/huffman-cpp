#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include "data/frequency-table.h"
#include "data/binary-tree.h"
#include <memory>

namespace encoding
{
    namespace huffman
    {
        std::unique_ptr<data::Node<Datum>> build_tree(const data::FrequencyTable<Datum>& frequencies);
    }
}

#endif
