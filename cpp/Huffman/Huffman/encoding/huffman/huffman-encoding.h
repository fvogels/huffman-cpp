#ifndef HUFFMAN_ENCODING_H
#define HUFFMAN_ENCODING_H

#include "encoding/encoding.h"
#include "defs.h"
#include <memory>


namespace encoding
{
    Encoding<Datum, Datum> create_huffman_encoder(unsigned domain_size);
}

#endif
