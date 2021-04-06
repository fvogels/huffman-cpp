#include "encoding/predictive/predictive-encoding.h"
#include <assert.h>


namespace
{
    class PredictiveEncodingImplementation : public encoding::EncodingImplementation
    {
    private:
        u64 m_domain_size;
        std::unique_ptr<encoding::predictive::Oracle> m_oracle;

    public:
        PredictiveEncodingImplementation(u64 domain_size, std::unique_ptr<encoding::predictive::Oracle> oracle) : m_domain_size(domain_size), m_oracle(std::move(oracle))
        {
            // NOP
        }

        void encode(io::InputStream& input, io::OutputStream& output) const override
        {
            m_oracle->reset();

            while (!input.end_reached())
            {
                auto actual_datum = input.read();
                auto predicted_datum = m_oracle->predict();
                m_oracle->tell(actual_datum);
                auto correction = correct(actual_datum, predicted_datum);
                output.write(correction);
            }
        }

        void decode(io::InputStream& input, io::OutputStream& output) const override
        {
            m_oracle->reset();

            while (!input.end_reached())
            {
                auto correction = input.read();
                auto predicted_datum = m_oracle->predict();
                auto datum = apply_correction(predicted_datum, correction);
                m_oracle->tell(datum);
                output.write(datum);
            }
        }

    private:
        Datum correct(Datum actual_datum, Datum predicted_datum) const
        {
            assert(actual_datum < m_domain_size);
            assert(predicted_datum < m_domain_size);

            if (actual_datum >= predicted_datum)
            {
                return actual_datum - predicted_datum;
            }
            else
            {
                return m_domain_size - predicted_datum + actual_datum;
            }
        }

        Datum apply_correction(Datum predicted_datum, Datum correction) const
        {
            assert(predicted_datum < m_domain_size);
            assert(correction < m_domain_size);

            assert(predicted_datum + correction >= predicted_datum);
            return (predicted_datum + correction) % m_domain_size;
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_predictive_encoding_implementation(u64 domain_size, std::unique_ptr<encoding::predictive::Oracle> oracle)
{
    return std::make_shared<PredictiveEncodingImplementation>(domain_size, std::move(oracle));
}
