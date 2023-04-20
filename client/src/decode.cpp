#include <main.h>
#include <Decoder.h>
#include <ParamsParser.h>

#include <memory>
#include <fstream>

namespace {
    std::unique_ptr<Decoder> createDecoderFromName(std::string_view name) {
        if (name == "none")
            return std::make_unique<NoneDecoder>();

        if (name == "rsa")
            return std::make_unique<RsaDecoder>();

        throw std::invalid_argument { "unknown algorithm: \"" + std::string { name } + "\"" };
    }
} // namespace

void cypherTools::decode(std::span<const std::string> args) {
    ParamsParser params = ParamsParser::parse(args);

    if (!params.isAlgorithmPresent()) {
        std::cout << "Warning: No algorithm specified. "
                  << "File data will not be encoded."
                  << std::endl;
        params.setAlgorithm("none");
    }

    std::string algorithm = params.getAlgorithm();
    std::string filename = params.getFilename();

    std::ifstream in { outputSubdir + params.getFilename(), std::ios_base::binary };
    if(in.bad())
        throw std::runtime_error { "unable to open input file" };

    std::ofstream out { outputSubdir + std::string("decode-") + params.getFilename(), std::ios_base::binary };
    if(out.bad())
        throw std::runtime_error { "unable to open output file" };

    std::cout << "Initialising decoding with params:\n"
              << "    Algorithm: " << algorithm << "\n"
              << "    Filename: " << filename << std::endl;

    std::unique_ptr<Decoder> decoder = createDecoderFromName(algorithm);
    decoder->decode(in, out);

    std::cout << "Successful." << std::endl;
}
