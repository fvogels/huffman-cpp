#include "encoding/mapper.h"


namespace
{
    class MapperImplementation : public encoding::EncodingImplementation
    {
    private:
        std::function<Datum(const Datum&)> m_function;
        std::function<Datum(const Datum&)> m_inverse_function;

    public:
        MapperImplementation(std::function<Datum(const Datum&)> function, std::function<Datum(const Datum&)> inverse_function) : m_function(function), m_inverse_function(inverse_function)
        {
            // NOP
        }

        void encode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const
        {
            while (!input.end_reached())
            {
                output.write(m_function(input.read()));
            }
        }

        void decode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const
        {
            while (!input.end_reached())
            {
                output.write(m_inverse_function(input.read()));
            }
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_mapper_implementation(std::function<Datum(const Datum&)> function, std::function<Datum(const Datum&)> inverse_function)
{
    return std::make_shared<MapperImplementation>(function, inverse_function);
}