#include <Encoder.h>

#include <stdexcept>

void NoneEncoder::encode(std::istream& in, std::ostream& out) {
    if (!in.good())
        throw std::invalid_argument { "input stream is in a bad state" };

    if (!out.good())
        throw std::invalid_argument { "output stream is in a bad state" };

    char byte;
    while (in.get(byte) && out)
        out.put(byte);

    if (!in.eof() && !in.good())
        throw std::runtime_error { "input stream failure has occurred" };

    if (!out.eof() && !out.good())
        throw std::runtime_error { "output stream failure has occurred" };
}
