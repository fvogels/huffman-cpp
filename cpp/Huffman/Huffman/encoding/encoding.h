#ifndef ENCODING_H
#define ENCODING_H

#include "io/input-stream.h"
#include "io/output-stream.h"


namespace encoding
{
    template<typename IN, typename OUT>
    class Encoding
    {
    public:
        virtual ~Encoding() { }

        virtual void encode(io::InputStream<IN>& input, io::OutputStream<OUT>& output) const = 0;
        virtual void decode(io::InputStream<OUT>& input, io::OutputStream<IN>& output) const = 0;
    };
}

#endif
