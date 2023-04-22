#include <Decoder.h>

#include <windows.h>

namespace {
    void readBMP(std::istream& in, std::vector<RGBQUAD>& colormap,
                 tagBITMAPFILEHEADER& header, tagBITMAPINFOHEADER& bitmapHeader) {
        in.read((char *) &header, sizeof(tagBITMAPFILEHEADER));

        in.read((char *) &bitmapHeader, sizeof(tagBITMAPINFOHEADER));

        RGBQUAD tempRGBQUAD{};
        while(!in.eof()) {
            in.read((char *) &tempRGBQUAD, sizeof(RGBQUAD));
            colormap.push_back(tempRGBQUAD);
        }
    }

    std::vector<unsigned char> decipher(std::vector<RGBQUAD> &colormap) {
        std::vector<unsigned char> message;
        while (true) {
            for (auto RGB: colormap) {
                int deciphered_byte =
                        (static_cast<int>(RGB.rgbBlue & 3) * 64) +
                        (static_cast<int>(RGB.rgbGreen & 3) * 16) +
                        (static_cast<int>(RGB.rgbRed & 3) * 4) +
                        static_cast<int>(RGB.rgbReserved & 3);
                if (deciphered_byte == 255) break;
                else message.push_back(static_cast<unsigned char>(deciphered_byte));
            }
            break;
        }

        return message;
    }
}// namespace

void LsbDecoder::decode(std::istream& in, std::ostream& out){
    if (!in.good())
        throw std::invalid_argument { "input stream is in a bad state" };

    if (!out.good())
        throw std::invalid_argument { "output stream is in a bad state" };

    std::vector<RGBQUAD> colormap;
    tagBITMAPFILEHEADER header{};
    tagBITMAPINFOHEADER bitmapHeader{};

    readBMP(in, colormap, header, bitmapHeader);

    std::vector<unsigned char> decodedMessage;

    decodedMessage = std::move(decipher(colormap));

    for(unsigned char byte : decodedMessage)
        out.put(static_cast<char>(byte));

    if(out.fail())
        throw std::runtime_error{"output stream failure has occurred"};
}
