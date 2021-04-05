#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

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
    };
}

#endif
