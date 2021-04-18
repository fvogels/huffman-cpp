#ifndef DATA_ENDPOINTS_H
#define DATA_ENDPOINTS_H

#include "io/streams.h"
#include <memory>


namespace io
{
    class DataSourceImplementation
    {
    public:
        virtual std::unique_ptr<InputStream> create_input_stream() = 0;
    };

    template<u64 N>
    class DataSource
    {
    private:
        std::shared_ptr<DataSourceImplementation> m_implementation;

    public:
        static constexpr u64 domain_size = N;

        DataSource(std::shared_ptr<DataSourceImplementation> implementation) : m_implementation(implementation)
        {
            // NOP
        }

        DataSourceImplementation* operator->()
        {
            return m_implementation.get();
        }

        const DataSourceImplementation* operator->() const
        {
            return m_implementation.get();
        }
    };

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

        const DataSourceImplementation* operator->() const
        {
            return m_implementation.get();
        }
    };
}

#endif
