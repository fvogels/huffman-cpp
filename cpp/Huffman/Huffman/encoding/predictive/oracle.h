#ifndef ORACLE_H
#define ORACLE_H

#include "util.h"


namespace encoding
{
    namespace predictive
    {
        class Oracle
        {
        public:
            virtual ~Oracle() { }

            virtual void reset()             = 0;
            virtual void tell(Datum datum)   = 0;
            virtual Datum predict() const    = 0;
        };
    }
}

#endif
