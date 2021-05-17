#ifndef DECODING_H
#define DECODING_H

#include "io/streams.h"
#include "data/binary-tree.h"
#include "util.h"


namespace encoding
{
    namespace huffman
    {
        void decode_bits(io::InputStream& input, const data::Node<Datum>& tree, io::OutputStream& output);

        Datum decode_single_datum(io::InputStream& input, const data::Node<Datum>& tree);
    }
}

#endif
