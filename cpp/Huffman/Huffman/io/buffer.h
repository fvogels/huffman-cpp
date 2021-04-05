#ifndef BUFFER_H
#define BUFFER_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include "io/data-source.h"
#include "io/data-destination.h"
#include <assert.h>
#include <numeric>
#include <memory>
#include <vector>


namespace io
{
    //template<typename T>
    //class BufferInputStream : public InputStream
    //{
    //private:
    //    std::shared_ptr<const std::vector<T>> m_contents;
    //    size_t m_index;

    //public:
    //    BufferInputStream(std::shared_ptr<const std::vector<T>> contents) : m_contents(contents), m_index(0)
    //    {
    //        // NOP
    //    }

    //    u64 read() override
    //    {
    //        assert(m_index < m_contents->size());

    //        return (*m_contents)[m_index++];
    //    }

    //    bool end_reached() const override
    //    {
    //        return m_index == m_contents->size();
    //    }
    //};

    //template<typename T>
    //class BufferOutputStream : public OutputStream
    //{
    //private:
    //    std::shared_ptr<std::vector<T>> m_contents;

    //public:
    //    BufferOutputStream(std::shared_ptr<std::vector<T>> contents) : m_contents(contents)
    //    {
    //        // NOP
    //    }

    //    void write(u64 value)
    //    {
    //        assert(value <= std::numeric_limits<T>::max());

    //        m_contents->push_back(static_cast<T>(value));
    //    }
    //};

    //template<typename T>
    //class Buffer : public DataSource, public DataDestination
    //{
    //private:
    //    std::shared_ptr<std::vector<T>> m_contents;

    //public:
    //    Buffer() : Buffer(std::make_shared<std::vector<T>>())
    //    {
    //        // NOP
    //    }

    //    Buffer(std::shared_ptr<std::vector<T>> contents) : m_contents(contents)
    //    {
    //        // NOP
    //    }

    //    Buffer(const std::vector<T>& contents) : Buffer(std::make_shared<std::vector<T>>(contents))
    //    {
    //        // NOP
    //    }

    //    std::unique_ptr<InputStream> create_input_stream() override
    //    {
    //        return std::make_unique<BufferInputStream<T>>(m_contents);
    //    }

    //    std::unique_ptr<OutputStream> create_output_stream() override
    //    {
    //        return std::make_unique<BufferOutputStream<T>>(m_contents);
    //    }

    //    std::shared_ptr<const std::vector<T>> contents() const
    //    {
    //        return m_contents;
    //    }
    //};
}

#endif
