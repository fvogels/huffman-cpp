#ifndef MAPPER_H
#define MAPPER_H

#include "encoding/encoding.h"
#include <functional>


namespace encoding
{
    template<typename IN, typename OUT>
    class MapperImplementation : public EncodingImplementation<IN, OUT>
    {
    private:
        std::function<OUT(const IN&)> m_function;
        std::function<IN(const OUT&)> m_inverse_function;

    public:
        MapperImplementation(std::function<OUT(const IN&)> function, std::function<IN(const OUT&)> inverse_function) : m_function(function), m_inverse_function(inverse_function)
        {
            // NOP
        }

        void encode(io::InputStream<IN>& input, io::OutputStream<OUT>& output) const
        {
            while (!input.end_reached())
            {
                output.write(m_function(input.read()));
            }
        }

        void decode(io::InputStream<OUT>& input, io::OutputStream<IN>& output) const
        {
            while (!input.end_reached())
            {
                output.write(m_inverse_function(input.read()));
            }
        }
    };

    template<typename IN, typename OUT>
    encoding::Encoding<IN, OUT> create_mapper(std::function<OUT(const IN&)> function, std::function<IN(const OUT&)> inverse_function)
    {
        return encoding::create_encoding<MapperImplementation<IN, OUT>>(function, inverse_function);
    }
}

#endif
