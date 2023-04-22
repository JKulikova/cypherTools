#pragma once

#include <iostream>
#include <vector>

class Decoder {
public:
    virtual ~Decoder() = default;

    virtual void decode(std::istream&, std::ostream&) = 0;
};

class NoneDecoder final : public Decoder {
public:
    void decode(std::istream&, std::ostream&) override;
};

class RsaDecoder final : public Decoder {
    std::string secretKey = "SecretKey";

    std::vector<unsigned int> get_keys(std::string_view filename);
    unsigned int mod(unsigned int a, unsigned int e, unsigned int n);
public:
    void decode(std::istream&, std::ostream&) override;
};

class LsbDecoder final : public Decoder {
public:
    void decode(std::istream&, std::ostream&) override;
};
