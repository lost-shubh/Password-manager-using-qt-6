#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/rand.h>

class Encryption {
public:
    static constexpr int KEY_SIZE = 32;
    static constexpr int IV_SIZE = 16;
    static constexpr int SALT_SIZE = 16;
    static constexpr int PBKDF2_ITERATIONS = 10000;

    Encryption() = default;
    ~Encryption() = default;
    Encryption(const Encryption&) = delete;
    Encryption& operator=(const Encryption&) = delete;
    Encryption(Encryption&&) = delete;
    Encryption& operator=(Encryption&&) = delete;

    std::vector<unsigned char> generateSalt();
    std::vector<unsigned char> generateIV();
    std::vector<unsigned char> encrypt(const std::string& plaintext, const std::string& password,
                                      const std::vector<unsigned char>& salt, const std::vector<unsigned char>& iv);
    std::string decrypt(const std::vector<unsigned char>& ciphertext, const std::string& password,
                       const std::vector<unsigned char>& salt, const std::vector<unsigned char>& iv);
    static std::string toHex(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> fromHex(const std::string& hex);

private:
    std::vector<unsigned char> deriveKey(const std::string& password, const std::vector<unsigned char>& salt);
};

#endif // ENCRYPTION_H