#ifndef REPEATING_ORACLE_H
#define REPEATING_ORACLE_H

#include "encoding/predictive/oracle.h"
#include <memory>


namespace encoding
{
    namespace predictive
    {
        std::unique_ptr<Oracle> repeating_oracle(Datum constant);
    }
}

#endif
