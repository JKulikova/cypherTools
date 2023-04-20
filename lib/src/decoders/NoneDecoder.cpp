#include "Decoder.h"

#include "Encoder.h"

void NoneDecoder::decode(std::istream& in, std::ostream& out) {
    if (!in.good())
        throw std::invalid_argument { "input stream is in a bad state" };

    if (!out.good())
        throw std::invalid_argument { "output stream is in a bad state" };

    NoneEncoder encoderDecoder {};
    encoderDecoder.encode(in, out);
}
