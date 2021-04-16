#ifndef TRIE_ORACLE_H
#define TRIE_ORACLE_H

#include "encoding/predictive/oracle.h"
#include <memory>


namespace encoding
{
    namespace predictive
    {
        std::unique_ptr<Oracle> trie_oracle(unsigned max_depth);
    }
}

#endif
