#include "encoding/predictive/constant-oracle.h"


namespace
{
    class ConstantOracleImplementation : public encoding::predictive::Oracle
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

        void reset() override
        {
            // NOP
        }
    };
}

std::unique_ptr<encoding::predictive::Oracle> encoding::predictive::constant_oracle(Datum datum)
{
    return std::make_unique<ConstantOracleImplementation>(datum);
}
