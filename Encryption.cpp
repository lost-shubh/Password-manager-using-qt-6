#include "Encryption.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <limits>

using EvpCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

std::vector<unsigned char> Encryption::deriveKey(const std::string& password, const std::vector<unsigned char>& salt) {
    const size_t expected_salt_size = static_cast<size_t>(SALT_SIZE);
    if (salt.size() != expected_salt_size) {
         std::stringstream ss;
         ss << "deriveKey: Salt size must be " << expected_salt_size << " bytes (got " << salt.size() << ").";
         throw std::invalid_argument(ss.str());
    }
    std::vector<unsigned char> key(KEY_SIZE);
    int pass_len_int = static_cast<int>(password.length());
    if (password.length() > static_cast<size_t>(std::numeric_limits<int>::max())) {
        throw std::runtime_error("Password too long for OpenSSL API.");
    }
    int result = PKCS5_PBKDF2_HMAC(password.c_str(), pass_len_int, salt.data(), SALT_SIZE, PBKDF2_ITERATIONS, EVP_sha256(), KEY_SIZE, key.data());
    if (result != 1) { throw std::runtime_error("Failed to derive key using PBKDF2-HMAC-SHA256."); }
    return key;
}

std::vector<unsigned char> Encryption::generateSalt() {
    std::vector<unsigned char> salt(SALT_SIZE);
    if (RAND_bytes(salt.data(), SALT_SIZE) != 1) { throw std::runtime_error("Failed to generate secure random salt."); }
    return salt;
}

std::vector<unsigned char> Encryption::generateIV() {
    std::vector<unsigned char> iv(IV_SIZE);
    if (RAND_bytes(iv.data(), IV_SIZE) != 1) { throw std::runtime_error("Failed to generate secure random IV."); }
    return iv;
}

std::vector<unsigned char> Encryption::encrypt(const std::string& plaintext, const std::string& password,
                                             const std::vector<unsigned char>& salt, const std::vector<unsigned char>& iv) {
    const size_t expected_iv_size = static_cast<size_t>(IV_SIZE);
    if (iv.size() != expected_iv_size) {
         std::stringstream ss;
         ss << "encrypt: IV size must be " << expected_iv_size << " bytes (got " << iv.size() << ").";
         throw std::invalid_argument(ss.str());
    }
    std::vector<unsigned char> key = deriveKey(password, salt);
    EvpCipherCtxPtr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    if (!ctx) { throw std::runtime_error("Encryption: Failed to create EVP_CIPHER_CTX."); }
    if (1 != EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, key.data(), iv.data())) {
        throw std::runtime_error("Encryption: Failed to initialize cipher context (EVP_EncryptInit_ex).");
    }
    size_t max_ciphertext_len = plaintext.length() + static_cast<size_t>(EVP_CIPHER_CTX_block_size(ctx.get()));
    std::vector<unsigned char> ciphertext(max_ciphertext_len);
    int bytes_written1 = 0;
    int bytes_written2 = 0;
    int plaintext_len_int = static_cast<int>(plaintext.length());
    if (plaintext.length() > static_cast<size_t>(std::numeric_limits<int>::max())) {
         throw std::runtime_error("Plaintext too long for OpenSSL API.");
    }
    if (1 != EVP_EncryptUpdate(ctx.get(), ciphertext.data(), &bytes_written1, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext_len_int)) {
        throw std::runtime_error("Encryption: EVP_EncryptUpdate failed.");
    }
    if (1 != EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + bytes_written1, &bytes_written2)) {
        throw std::runtime_error("Encryption: EVP_EncryptFinal_ex failed.");
    }
    ciphertext.resize(static_cast<size_t>(bytes_written1) + static_cast<size_t>(bytes_written2));
    return ciphertext;
}

std::string Encryption::decrypt(const std::vector<unsigned char>& ciphertext, const std::string& password,
                              const std::vector<unsigned char>& salt, const std::vector<unsigned char>& iv) {
    const size_t expected_iv_size = static_cast<size_t>(IV_SIZE);
     if (iv.size() != expected_iv_size) {
         std::stringstream ss;
         ss << "decrypt: IV size must be " << expected_iv_size << " bytes (got " << iv.size() << ").";
         throw std::invalid_argument(ss.str());
    }
    std::vector<unsigned char> key = deriveKey(password, salt);
    EvpCipherCtxPtr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    if (!ctx) { throw std::runtime_error("Decryption: Failed to create EVP_CIPHER_CTX."); }
    if (1 != EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, key.data(), iv.data())) {
        throw std::runtime_error("Decryption: Failed to initialize cipher context (EVP_DecryptInit_ex).");
    }
    std::vector<unsigned char> plaintext(ciphertext.size());
    int bytes_written1 = 0;
    int bytes_written2 = 0;
    int ciphertext_len_int = static_cast<int>(ciphertext.size());
     if (ciphertext.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
         throw std::runtime_error("Ciphertext too long for OpenSSL API.");
    }
    if (1 != EVP_DecryptUpdate(ctx.get(), plaintext.data(), &bytes_written1, ciphertext.data(), ciphertext_len_int)) {
        throw std::runtime_error("Decryption: EVP_DecryptUpdate failed. Possible incorrect password or data corruption.");
    }
    if (1 != EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + bytes_written1, &bytes_written2)) {
        throw std::runtime_error("Decryption: EVP_DecryptFinal_ex failed. Invalid master password or corrupted data.");
    }
    plaintext.resize(static_cast<size_t>(bytes_written1) + static_cast<size_t>(bytes_written2));
    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
}

std::string Encryption::toHex(const std::vector<unsigned char>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::vector<unsigned char> Encryption::fromHex(const std::string& hex) {
    if (hex.length() % 2 != 0 || hex.empty()) {
        throw std::invalid_argument("Hex string must have a non-zero, even number of characters for conversion.");
    }
    std::vector<unsigned char> data;
    data.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        try {
            unsigned long byteValue = std::stoul(byteString, nullptr, 16);
            if (byteValue > 255) {
                 throw std::out_of_range("Hex value '" + byteString + "' out of range for uchar.");
            }
            data.push_back(static_cast<unsigned char>(byteValue));
        } catch (const std::invalid_argument& e) {
            throw std::invalid_argument("Invalid non-hex char in string: '" + byteString + "'");
        } catch (const std::out_of_range& e) {
             throw std::invalid_argument("Hex value processing error for '" + byteString + "': " + e.what());
        }
    }
    return data;
}