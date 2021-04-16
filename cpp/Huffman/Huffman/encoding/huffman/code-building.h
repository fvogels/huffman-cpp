#ifndef CODE_BUILDING_H
#define CODE_BUILDING_H

#include "data/binary-tree.h"
#include <vector>
#include "util.h"

namespace encoding
{
    namespace huffman
    {
        std::vector<std::vector<Datum>> build_codes(const data::Node<Datum>& tree, u64 domain_size);
    }
}

#endif
