#include <main.h>
#include <ParamsParser.h>
#include <Encoder.h>

#include <iostream>
#include <fstream>
#include <memory>

namespace {
    std::unique_ptr<Encoder> createEncoderFromName(std::string_view name) {
        if (name == "none")
            return std::make_unique<NoneEncoder>();

        if (name == "rsa")
            return std::make_unique<RsaEncoder>();

        throw std::invalid_argument { "unknown algorithm: \"" + std::string { name } + "\"" };
    }
} // namespace

void cypherTools::encode(std::span<const std::string> args) {
    ParamsParser params = ParamsParser::parse(args);

    if (!params.isAlgorithmPresent()) {
        std::cout << "Warning: No algorithm specified. "
                  << "File data will not be encoded."
                  << std::endl;
        params.setAlgorithm("none");
    }

    std::string algorithm = params.getAlgorithm();
    std::string filename = params.getFilename();

    std::ifstream in { outputSubdir + params.getFilename() , std::ios_base::binary};
    if(in.bad())
        throw std::runtime_error { "unable to open input file" };

    std::ofstream out { outputSubdir + std::string("encode-") + params.getFilename(), std::ios_base::binary };
    if(out.bad())
        throw std::runtime_error { "unable to open output file" };

    std::cout << "Initialising encoding with params:\n"
              << "    Algorithm: " << algorithm << "\n"
              << "    Filename: " << filename << std::endl;

    std::unique_ptr<Encoder> encoder = createEncoderFromName(algorithm);
    encoder->encode(in, out);

    std::cout << "Successful." << std::endl;
}
