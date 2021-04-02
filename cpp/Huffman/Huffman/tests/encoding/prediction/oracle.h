#ifndef ORACLE_H
#define ORACLE_H

namespace encoding
{
    template<typename T>
    class Oracle
    {
    public:
        virtual ~Oracle() { }

        virtual void tell(const T& t) = 0;
        virtual void predict() const  = 0;
    };
}

#endif
