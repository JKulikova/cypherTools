#include <Decoder.h>
#include <blowfishConstants.h>
#include <main.h>
#include <fstream>

namespace{
    constexpr uint8_t EOT{4};

    std::vector<uint32_t> readMessage(std::istream& in){
        std::vector<uint32_t> alignedMessage;

        int word = 0;
        int byte = 0;
        while(!in.eof()){
            if(byte == 0)
                alignedMessage.push_back(0x00000000);
            if(byte < 4) {
                alignedMessage[word] = (alignedMessage[word] << 8) | static_cast<uint8_t>(in.get());
                ++byte;
            }
            else{
                byte = 0;
                ++word;
            }
        }
        while(byte < 4){
            alignedMessage[word] = (alignedMessage[word] << 8);
            ++byte;
        }

        return alignedMessage;
    }

    std::vector<uint32_t> readKey(std::string& key){
        std::ifstream in{ key, std::ios::binary };

        if(!in.good())
            throw std::runtime_error { "key input stream failure has occurred" };

        std::vector<uint32_t> alignedKey;

        int word = 0;
        int byte = 0;
        while(!in.eof() && word != blowfish::KEY_MAX_LENGTH / 8){
            if(byte == 0)
                alignedKey.push_back(0x00000000);
            if(byte < 4) {
                alignedKey[word] = (alignedKey[word] << 8) | static_cast<uint8_t>(in.get());
                ++byte;
            }
            else{
                byte = 0;
                ++word;
            }
        }
        while(byte < 4){
            alignedKey[word] = (alignedKey[word] << 8);
            ++byte;
        }

        in.close();

        return alignedKey;
    }

    uint32_t F(std::vector<std::vector<uint32_t>>& SBox, uint32_t x)
    {
        return ((SBox[0][(x >> 24) & 0xFF] + SBox[1][(x >> 16) & 0xFF]) ^
                SBox[2][(x >> 8) & 0xFF]) + SBox[3][(x) & 0xFF];
    }

    std::uint64_t feistel_net(uint32_t L0, uint32_t R0,
                              std::vector<uint32_t>& P,std::vector<std::vector<uint32_t>>& SBox,
                              bool reverse){
        uint64_t result;
        if(!reverse){
            for(int i = 0 ; i < blowfish::ROUNDS; ++i){
                L0 ^= P[i];
                R0 ^= F(SBox, L0);

                uint32_t Ltemp = R0;
                R0 = L0;
                L0 = Ltemp;
            }

            uint32_t Ltemp = R0;
            R0 = L0;
            L0 = Ltemp;

            L0 ^= P[blowfish::ROUND_KEYS_COUNT - 1];
            R0 ^= P[blowfish::ROUND_KEYS_COUNT - 2];

            result = R0;
            result = result << 32;
            result = result | L0;
        }
        else{
            for(int i = 0 ; i < blowfish::ROUNDS; ++i){
                L0 ^= P[blowfish::ROUNDS + 1 - i];
                R0 ^= F(SBox, L0);

                uint32_t Ltemp = R0;
                R0 = L0;
                L0 = Ltemp;
            }

            uint32_t Ltemp = R0;
            R0 = L0;
            L0 = Ltemp;

            L0 ^= P[0];
            R0 ^= P[1];

            result = R0;
            result = result << 32;
            result = result | L0;
        }
        return result;
    }

    void initialize(std::vector<uint32_t>& P,
                    std::vector<std::vector<uint32_t>>& SBoxes,
                    std::string keyFilename){

        for (int i = 0 ; i < blowfish::ROUND_KEYS_COUNT ; i++)
            P[i] = blowfish::FIXED_P[i] ;

        for (int i = 0 ; i < blowfish::SBOXES_COUNT ; i++)
        {
            for (int j = 0 ; j < blowfish::SBOX_LENGTH ; j++)
                SBoxes [i][j] = blowfish::FIXED_S [i][j] ;
        }


        std::vector<uint32_t> key = readKey(keyFilename);

        for(int i = 0; i < blowfish::ROUND_KEYS_COUNT; ++i){
            P[i] ^= key[i];
        }

        uint64_t temp = feistel_net(0x00000000, 0x00000000, P, SBoxes, false);

        P[0] = temp & UINT32_MAX;
        P[1] = (temp >> 32) & UINT32_MAX;

        for(int i = 2; i < blowfish::ROUND_KEYS_COUNT; i += 2){
            temp = feistel_net(P[i-2], P[i-1], P, SBoxes, false);

            P[i] = temp & UINT32_MAX;
            P[i + 1] = (temp >> 32) & UINT32_MAX;
        }

        temp = feistel_net(P[16], P[17], P, SBoxes, false);

        SBoxes[0][0] = temp & UINT32_MAX;
        SBoxes[0][1] = (temp >> 32) & UINT32_MAX;

        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < blowfish::SBOX_LENGTH; j += 2){
                temp = feistel_net(SBoxes[i][j], SBoxes[i][j + 1], P, SBoxes, false);

                SBoxes[i][j + 2] = temp & UINT32_MAX;
                SBoxes[i][j + 3] = (temp >> 32) & UINT32_MAX;
            }
        }
    }
} // namespace

void BlowfishDecoder::decode(std::istream& in, std::ostream& out) {
    if (!in.good())
        throw std::invalid_argument { "input stream is in a bad state" };

    if (!out.good())
        throw std::invalid_argument { "output stream is in a bad state" };

    std::vector<uint32_t> P(blowfish::ROUND_KEYS_COUNT);
    std::vector<std::vector<uint32_t>> SBoxes{blowfish::SBOXES_COUNT, std::vector<uint32_t>(blowfish::SBOX_LENGTH)};

    for (int i = 0 ; i < blowfish::ROUND_KEYS_COUNT ; i++)
        P[i] = blowfish::FIXED_P[i] ;

    for (int i = 0 ; i < blowfish::SBOXES_COUNT ; i++)
    {
        for (int j = 0 ; j < blowfish::SBOX_LENGTH ; j++)
            SBoxes [i][j] = blowfish::FIXED_S [i][j] ;
    }

    std::vector<uint32_t> message = std::move(readMessage(in));

    initialize(P, SBoxes, cypherTools::outputSubdir + key + cypherTools::fileExtension);

    for(int i = 0; i < message.size(); i += 2){
        uint64_t temp = feistel_net(message[i], message[i + 1], P, SBoxes, true);

        message[i] = temp & UINT32_MAX;
        message[i + 1] = (temp >> 32) & UINT32_MAX;
    }

    for(auto word : message){
        if(static_cast<char>((word >> 24) & UINT8_MAX) != EOT &&
                ((word >> 24) & UINT8_MAX) != 0xFF)
            out.put(static_cast<char>((word >> 24) & UINT8_MAX));
        else break;

        if(static_cast<char>((word >> 16) & UINT8_MAX) != EOT &&
           ((word >> 16) & UINT8_MAX) != 0xFF)
            out.put(static_cast<char>((word >> 16) & UINT8_MAX));
        else break;

        if(static_cast<char>((word >> 8) & UINT8_MAX) != EOT &&
           ((word >> 8) & UINT8_MAX) != 0xFF)
            out.put(static_cast<char>((word >> 8) & UINT8_MAX));
        else break;

        if(static_cast<char>(word & UINT8_MAX) != EOT &&
           (word & UINT8_MAX) != 0xFF)
            out.put(static_cast<char>(word & UINT8_MAX));
        else break;
    }
}
