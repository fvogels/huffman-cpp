#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

namespace io
{
    template<typename T>
    struct InputStream
    {
        virtual ~InputStream() { }

        virtual T read() = 0;
        virtual bool end_reached() const = 0;
    };
}

#endif

