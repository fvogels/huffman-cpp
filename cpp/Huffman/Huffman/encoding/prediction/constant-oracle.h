#ifndef CONSTANT_ORACLE_H
#define CONSTANT_ORACLE_H

#include "encoding/prediction/oracle.h"
#include <memory>


namespace encoding
{
    namespace prediction
    {
        std::unique_ptr<Oracle> constant_oracle(Datum constant);
    }
}

#endif
