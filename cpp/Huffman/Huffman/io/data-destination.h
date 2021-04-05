#ifndef DATA_DESTINATION_H
#define DATA_DESTINATION_H

#include "io/streams.h"
#include <memory>

namespace io
{
    class DataDestinationImplementation
    {
    public:
        virtual std::unique_ptr<OutputStream> create_output_stream() = 0;
    };

    template<u64 N>
    class DataDestination
    {
    private:
        std::shared_ptr<DataDestinationImplementation> m_implementation;

    public:
        static constexpr u64 domain_size = N;

        DataDestination(std::shared_ptr<DataDestinationImplementation> implementation) : m_implementation(implementation)
        {
            // NOP
        }

        DataDestinationImplementation* operator->()
        {
            return m_implementation.get();
        }
    };
}

#endif
