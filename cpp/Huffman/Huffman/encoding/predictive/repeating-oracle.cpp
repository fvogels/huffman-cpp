#include "encoding/predictive/repeating-oracle.h"


namespace
{
    class RepeatingOracleImplementation : public encoding::predictive::Oracle
    {
    private:
        Datum m_last_datum;

    public:
        RepeatingOracleImplementation(Datum datum) : m_last_datum(datum)
        {
            // NOP
        }

        void tell(Datum datum) override
        {
            m_last_datum = datum;
        }

        Datum predict() const override
        {
            return m_last_datum;
        }
    };
}

std::unique_ptr<encoding::predictive::Oracle> encoding::predictive::repeating_oracle(Datum datum)
{
    return std::make_unique<RepeatingOracleImplementation>(datum);
}
