#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <bitset>
#include <list>

class Encoder {
public:
    virtual ~Encoder() = default;

    virtual void encode(std::istream&, std::ostream&) = 0;
};

class NoneEncoder final : public Encoder {
public:
    void encode(std::istream&, std::ostream&) override;
};

class RsaEncoder final : public Encoder {
    std::string openKey = "OpenKey";
    std::string secretKey = "SecretKey";

    std::mt19937 randomEngine;

    bool createKeys();
    std::list<unsigned int> getPrimes(unsigned int upperBoundary);
    unsigned int findCoprime(unsigned int eulerF, unsigned int n);
    std::list<unsigned int> commonMultiples(unsigned int val);
    std::vector<unsigned int> get_keys(std::string_view filename);
    unsigned int mod(unsigned int a, unsigned int e, unsigned int n);

public:
    RsaEncoder();

    void encode(std::istream&, std::ostream&) override;
};

class LsbEncoder final : public Encoder {
    std::string bmpMessageSource;
public:
    explicit LsbEncoder(std::string value);

    void encode(std::istream&, std::ostream&) override;
};
