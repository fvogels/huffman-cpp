#include "encoding/encoding-combiner.h"


namespace
{
    class EncodingCombinerImplementation : public encoding::EncodingImplementation
    {
    private:
        std::shared_ptr<encoding::EncodingImplementation> m_e1;
        std::shared_ptr<encoding::EncodingImplementation> m_e2;

    public:
        EncodingCombinerImplementation(std::shared_ptr<encoding::EncodingImplementation> e1, std::shared_ptr<encoding::EncodingImplementation> e2) : m_e1(e1), m_e2(e2)
        {
            // NOP
        }

        void encode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const
        {
            io::Buffer<Datum> buffer;

            m_e1->encode(input, *buffer.create_output_stream());
            m_e2->encode(*buffer.create_input_stream(), output);
        }

        void decode(io::InputStream<Datum>& input, io::OutputStream<Datum>& output) const
        {
            io::Buffer<Datum> buffer;

            m_e2->decode(input, *buffer.create_output_stream());
            m_e1->decode(*buffer.create_input_stream(), output);
        }
    };
}

std::shared_ptr<encoding::EncodingImplementation> encoding::create_combiner_implementation(std::shared_ptr<encoding::EncodingImplementation> e1, std::shared_ptr<encoding::EncodingImplementation> e2)
{
    return std::make_shared<EncodingCombinerImplementation>(e1, e2);
}