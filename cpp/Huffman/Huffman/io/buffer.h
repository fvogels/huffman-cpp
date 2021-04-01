#ifndef BUFFER_H
#define BUFFER_H

#include "io/input-stream.h"
#include "io/output-stream.h"
#include <assert.h>
#include <memory>
#include <vector>


namespace io
{
    template<typename T>
    class BufferInputStream : public InputStream<T>
    {
    private:
        std::shared_ptr<const std::vector<T>> m_contents;
        size_t m_index;

    public:
        BufferInputStream(std::shared_ptr<const std::vector<T>> contents) : m_contents(contents), m_index(0)
        {
            // NOP
        }

        const T& read() override
        {
            assert(m_index < m_contents->size());

            return (*m_contents)[m_index++];
        }

        bool end_reached() const override
        {
            return m_index == m_contents->size();
        }
    };

    template<typename T>
    class BufferOutputStream : public OutputStream<T>
    {
    private:
        std::shared_ptr<std::vector<T>> m_contents;

    public:
        BufferOutputStream(std::shared_ptr<std::vector<T>> contents) : m_contents(contents)
        {
            // NOP
        }

        void write(const T& value)
        {
            m_contents->push_back(value);
        }
    };

    template<typename T>
    class Buffer
    {
    private:
        std::shared_ptr<std::vector<T>> m_contents;

    public:
        Buffer() : Buffer(std::make_shared<std::vector<T>>())
        {
            // NOP
        }

        Buffer(std::shared_ptr<std::vector<T>> contents) : m_contents(contents)
        {
            // NOP
        }

        Buffer(const std::vector<T>& contents) : Buffer(std::make_shared<std::vector<T>>(contents))
        {
            // NOP
        }

        std::unique_ptr<InputStream<T>> create_input_stream() const
        {
            return std::make_unique<BufferInputStream<T>>(m_contents);
        }

        std::unique_ptr<OutputStream<T>> create_output_stream() const
        {
            return std::make_unique<BufferOutputStream<T>>(m_contents);
        }

        std::shared_ptr<const std::vector<T>> contents() const
        {
            return m_contents;
        }
    };
}

#endif
