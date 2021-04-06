#include "encoding/prediction/constant-oracle.h"


namespace
{
    class ConstantOracleImplementation : public encoding::prediction::Oracle
    {
    private:
        Datum m_datum;

    public:
        ConstantOracleImplementation(Datum datum) : m_datum(datum)
        {
            // NOP
        }

        void tell(Datum datum) override
        {
            // NOP
        }

        Datum predict() const override
        {
            return m_datum;
        }
    };
}

std::unique_ptr<encoding::prediction::Oracle> encoding::prediction::constant_oracle(Datum datum)
{
    return std::make_unique<ConstantOracleImplementation>(datum);
}
