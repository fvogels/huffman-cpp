#ifndef ORACLE_H
#define ORACLE_H

#include "defs.h"


namespace encoding
{
    namespace prediction
    {
        class Oracle
        {
        public:
            virtual ~Oracle() { }

            virtual void tell(Datum datum) = 0;
            virtual Datum predict() const = 0;
        };
    }
}

#endif
