#include <main.h>
#include <Decoder.h>

#include <fstream>

void RsaDecoder::decode(std::istream& in, std::ostream& out) {
    std::vector<unsigned int> key = get_keys(cypherTools::outputSubdir + secretKey + cypherTools::fileExtension);

    std::string cypheredMessage;
    std::string temp;

    char byte;
    while(in.get(byte) && out.good()){
        char decipheredChar = static_cast<char>(mod(byte, key[0], key[1]));
        out.put(decipheredChar);
    }

    if (!in.eof() && !in.good())
        throw std::runtime_error { "input stream failure has occurred" };

    if (!out.eof() && !out.good())
        throw std::runtime_error { "output stream failure has occurred" };
}

/*
 * Get open or secret keys from specified file
 */
std::vector<unsigned int> RsaDecoder::get_keys(std::string_view filename){
    std::ifstream file(filename.data());
    std::vector<unsigned int> keys(2);

    for(int i = 0; i < 2; ++i){
        std::string temp;
        std::getline(file, temp);

        keys[i] = std::atoi(temp.c_str());
    }

    return keys;
}

/*
 * Get deciphered symbol code
 */
unsigned int RsaDecoder::mod(unsigned int a, unsigned int e, unsigned int n){
    unsigned int result = 1;
    if(e < 1){
        result = 0;
    }
    while(e > 0){
        result = result * a % n;
        e -= 1;
    }
    return result;
}
