#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

namespace io
{
    template<typename T>
    struct OutputStream
    {
        virtual ~OutputStream() { }

        virtual void write(const T& value) = 0;
    };
}

#endif
