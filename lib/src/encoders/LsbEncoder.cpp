#include <Encoder.h>
#include <main.h>

#include <windows.h>
#include <fstream>

namespace{
    void readBMP(std::istream& in, std::vector<RGBQUAD>& colormap,
                 tagBITMAPFILEHEADER& header, tagBITMAPINFOHEADER& bitmapHeader) {
        in.read((char *) &header, sizeof(tagBITMAPFILEHEADER));

        in.read((char *) &bitmapHeader, sizeof(tagBITMAPINFOHEADER));

        RGBQUAD tempRGBQUAD{};
        while (!in.eof()) {
            in.read((char *) &tempRGBQUAD, sizeof(RGBQUAD));
            colormap.push_back(tempRGBQUAD);
        }
    }

    std::vector<unsigned char> readMessage(std::string& filename){
        std::ifstream message{ filename, std::ios::binary};
        if(!message.good())
            throw std::invalid_argument{"message input stream is in a bad state"};

        std::vector<unsigned char> result;

        while(!message.eof())
            result.push_back(message.get());

        return result;
    }

    void cypher(std::vector<RGBQUAD>& colormap, std::vector<unsigned char>& message){
        message.push_back(0xFF);
        int colormap_i = 0;
        for(auto letter : message){
            unsigned char Res = letter & 3;
            unsigned char R = (letter >> 2) & 3;
            unsigned char G = (letter >> 4) & 3;
            unsigned char B = (letter >> 6) & 3;


            colormap[colormap_i].rgbReserved =
                    ((colormap[colormap_i].rgbReserved >> 2) << 2) | Res;
            colormap[colormap_i].rgbRed =
                    ((colormap[colormap_i].rgbRed >> 2) << 2) | R;
            colormap[colormap_i].rgbGreen =
                    ((colormap[colormap_i].rgbGreen >> 2) << 2) | G;
            colormap[colormap_i].rgbBlue =
                    ((colormap[colormap_i].rgbBlue >> 2) << 2) | B;

            ++colormap_i;
        }
    }
} // namespace

LsbEncoder::LsbEncoder(std::string value) {
    bmpMessageSource = cypherTools::outputSubdir + std::move(value);
}

void LsbEncoder::encode(std::istream& in, std::ostream& out) {
    if (!in.good())
        throw std::invalid_argument { "input stream is in a bad state" };

    if (!out.good())
        throw std::invalid_argument { "output stream is in a bad state" };

    std::vector<RGBQUAD> colormap;
    tagBITMAPFILEHEADER header{};
    tagBITMAPINFOHEADER bitmapHeader{};

    readBMP(in, colormap, header, bitmapHeader);

    std::vector<unsigned char> message = readMessage(bmpMessageSource);

    cypher(colormap, message);

    out.write((char*)&header, sizeof(tagBITMAPFILEHEADER));
    out.write((char*)&bitmapHeader, sizeof(tagBITMAPINFOHEADER));
    for(auto item : colormap){
        out.write((char*)&item, sizeof(RGBQUAD));
    }

    if(out.fail())
        throw std::runtime_error{"output stream failure has occurred"};
}
