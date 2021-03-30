#ifndef ENCODING_H
#define ENCODING_H

namespace encoding
{
    template<typename IN, typename OUT>
    class Encoding
    {
    public:
        virtual ~Encoding() { }

        virtual OUT encode(const IN& data) const = 0;
        virtual IN decode(const OUT& data) const = 0;
    };
}

#endif
