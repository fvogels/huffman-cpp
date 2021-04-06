#ifndef CONSTANT_ORACLE_H
#define CONSTANT_ORACLE_H

#include "encoding/predictive/oracle.h"
#include <memory>


namespace encoding
{
    namespace predictive
    {
        std::unique_ptr<Oracle> constant_oracle(Datum constant);
    }
}

#endif
