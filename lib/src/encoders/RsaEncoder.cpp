#include <Encoder.h>
#include <main.h>

#include <fstream>
#include <chrono>

RsaEncoder::RsaEncoder() {
    std::random_device seeder;
    const auto seed = seeder.entropy() > 0 ?
                      seeder() :
                      std::chrono::high_resolution_clock::now().time_since_epoch().count();
    randomEngine = std::mt19937 { static_cast<std::mt19937::result_type>(seed) };
}

void RsaEncoder::encode(std::istream& in, std::ostream& out){
    if (!in.good())
        throw std::invalid_argument { "input stream is in a bad state" };

    if (!out.good())
        throw std::invalid_argument { "output stream is in a bad state" };

    if(!createKeys())
        throw std::runtime_error { "cyphering keys generation failure has occurred" };

    std::vector<unsigned int> key = get_keys(cypherTools::outputSubdir + openKey + cypherTools::fileExtension);

    char byte;
    while(in.get(byte) && out.good()){
        char tempCyphered = static_cast<char>(mod(byte, key[0], key[1]));
        out.put(tempCyphered);
    }

    if (!in.eof() && !in.good())
        throw std::runtime_error { "input stream failure has occurred" };

    if (!out.eof() && !out.good())
        throw std::runtime_error { "output stream failure has occurred" };
}

/*
 * Create open and secret keys and store tem into specified files
 */
bool RsaEncoder::createKeys(){
    unsigned int upperBoundary = 2 << (4);
    std::list<unsigned int> primes = getPrimes(upperBoundary);

    std::uniform_int_distribution<unsigned int> dist(0, primes.size() - 1);//random index from primes list

    auto it = primes.begin();
    std::advance(it, dist(randomEngine));
    unsigned int p = *it;
    primes.remove(p);

    it = primes.begin();
    std::advance(it, dist(randomEngine));
    unsigned int q = *it;
    primes.remove(q);

    unsigned int n = p * q;

    unsigned int eulerF = (p - 1) * (q - 1);

    unsigned int d = findCoprime(eulerF, n);

    unsigned int e = eulerF;

    while(e * d % eulerF != 1){
        --e;
    }

    std::ofstream open { cypherTools::outputSubdir + openKey + cypherTools::fileExtension, std::ios_base::binary };
    std::ofstream secret { cypherTools::outputSubdir + secretKey + cypherTools::fileExtension, std::ios_base::binary };

    if(open.good()){
        std::string first = std::to_string(e);
        open << e << std::endl;

        open << n;
        open.close();
    }
    else return false;

    if(secret.good()){
        secret << static_cast<unsigned int>(d) << std::endl;

        secret << n;
        secret.close();
    }
    else return false;

    return true;
}

/*
 * Use Sieve of Eratosthenes algorithm to filter prime numbers up to upper_boundary
 */
std::list<unsigned int> RsaEncoder::getPrimes(unsigned int upperBoundary){
    std::list<unsigned int> result;
    for(unsigned int i = 2; i < upperBoundary; ++i){
        result.push_back(i);
    }

    for(auto it = result.begin(); it != result.end(); it++){
        auto filter = std::next(it);
        while(filter != result.end()){
            if(*it == 0){
                break;
            }
            if((*filter % *it == 0)){
                *filter = 0;
            }
            std::advance(filter, 1);
        }
    }

    result.sort();
    result.remove_if([](unsigned int n){return n == 0;});

    return result;
}

unsigned int RsaEncoder::findCoprime(unsigned int eulerF, unsigned int n){
    unsigned int coprime = 1;

    std::list<unsigned int> eulerFCM = commonMultiples(eulerF);

    std::list<unsigned int> primes = getPrimes(n);
    for(auto i : eulerFCM) primes.remove(i);

    std::uniform_int_distribution<unsigned int> dist(0, primes.size() - 1);

    unsigned int count = dist(randomEngine);

    for(unsigned int i = 0; i < count; ++i){
        auto it = primes.begin();
        std::advance(it, dist(randomEngine));

        if(coprime * (*it) < n) coprime *= *it;
    }

    //check
    int match = 0;
    std::list<unsigned int> resultCM = commonMultiples(coprime);
    for(auto i : resultCM){
        if(std::find(eulerFCM.begin(), eulerFCM.end(), i) != std::end(eulerFCM)){
            ++match;
        }
    }

    if(match == 0)
        return coprime;
    else
        return 0;
}

/*
 * Search for common multiples to val
 */
std::list<unsigned int> RsaEncoder::commonMultiples(unsigned int val){
    std::list<unsigned int> commonMultiples;
    std::list<unsigned int> primes = getPrimes(val + 1);
    auto it = primes.begin();

    while(val != 1){
        auto isPrime = std::find(primes.begin(), primes.end(), val);
        if(isPrime != std::end(primes)){
            auto distance = std::distance(it, isPrime);
            std::advance(it, distance);
            val /= *it;
            commonMultiples.push_back(*it);
        }
        else {
            while (val % *it == 0) {
                val /= *it;
                commonMultiples.push_back(*it);
            }
            std::advance(it, 1);
        }
    }

    commonMultiples.sort();
    commonMultiples.unique();

    return commonMultiples;
}

/*
 * Get open or secret keys from specified file
 */
std::vector<unsigned int> RsaEncoder::get_keys(std::string_view filename){
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
 * Get cyphered symbol code
 */
unsigned int RsaEncoder::mod(unsigned int a, unsigned int e, unsigned int n){
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
