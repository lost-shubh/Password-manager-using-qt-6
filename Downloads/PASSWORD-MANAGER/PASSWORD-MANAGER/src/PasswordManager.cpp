#include "PasswordManager.h"
#include "Encryption.h"
#include "PasswordEntry.h"

#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iterator> // Only needed if using istreambuf_iterator (not needed for hex version)
// #include <openssl/crypto.h> // No longer needed for HMAC comparison

PasswordManager::PasswordManager(const std::string& masterPass, const std::string& storeFilename)
    : masterPassword(masterPass), entries(), encryption(), salt(), filename(storeFilename), needsSave(false)
{
    if (masterPassword.empty()) throw std::invalid_argument("Master password cannot be empty.");
    if (filename.empty()) throw std::invalid_argument("Storage filename cannot be empty.");

    std::ifstream fileCheck(filename, std::ios::binary);
    bool fileExists = fileCheck.good();
    fileCheck.close();

    if (fileExists) {
        std::ifstream saltFile(filename); // Read as text
        if (!saltFile) throw std::runtime_error("Password file exists but cannot be opened: " + filename);
        std::string saltHex;
        if (!std::getline(saltFile, saltHex)) {
            saltFile.close(); throw std::runtime_error("Cannot read salt line from password file: " + filename);
        }
        saltFile.close();
        try {
             salt = Encryption::fromHex(saltHex);
             if (salt.size() != static_cast<size_t>(Encryption::SALT_SIZE)) throw std::runtime_error("Invalid salt size loaded from file.");
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to parse salt from password file: " + std::string(e.what()));
        }
        loadFromFile(); // Attempt to load the rest (hex IV, hex data)
        needsSave = false;
    } else {
        std::cout << "Password file '" << filename << "' not found. Creating new store." << std::endl;
        try {
            salt = encryption.generateSalt();
            needsSave = true;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to generate initial salt: " + std::string(e.what()));
        }
    }
}

PasswordManager::~PasswordManager() {
    try { if (needsSave) { saveToFile(); } }
    catch (const std::exception& e) { std::cerr << "ERROR: Save failed on exit: " << e.what() << std::endl; }
    catch (...) { std::cerr << "ERROR: Unknown error during save on exit." << std::endl; }
}

void PasswordManager::loadFromFile() {
    std::ifstream file(filename); // Open as text
    if (!file) throw std::runtime_error("loadFromFile: Cannot open password file: " + filename);

    try {
        std::string saltHexLine, ivHex, dataHex;
        if (!std::getline(file, saltHexLine)) throw std::runtime_error("File structure error: Cannot read salt line."); // Read & ignore salt
        if (!std::getline(file, ivHex)) throw std::runtime_error("File structure error: Cannot read IV line.");
        std::stringstream buffer; buffer << file.rdbuf(); dataHex = buffer.str();
        if (file.bad()) throw std::runtime_error("Error reading password file data block.");
        file.close();
        while (!dataHex.empty() && (dataHex.back() == '\n' || dataHex.back() == '\r')) dataHex.pop_back();

        if (ivHex.empty() || dataHex.empty()) { // Valid empty store
            entries.clear(); needsSave = false; return;
        }

        std::vector<unsigned char> iv = Encryption::fromHex(ivHex); // throws
        std::vector<unsigned char> encryptedData = Encryption::fromHex(dataHex); // throws
        std::string decrypted = encryption.decrypt(encryptedData, masterPassword, salt, iv); // throws

        entries.clear(); std::stringstream ss(decrypted); std::string line; int lineNum = 0;
        while (std::getline(ss, line)) {
            lineNum++; if (!line.empty()) { try { entries.push_back(PasswordEntry::deserialize(line)); }
                catch (const std::exception& e) { std::cerr << "Warning: Skipping corrupted entry line #" << lineNum << ": " << e.what() << std::endl; } } }
        needsSave = false;
    } catch (const std::ios_base::failure& e) { if(file.is_open()) file.close(); throw std::runtime_error("File I/O error during load: " + std::string(e.what()));
    } catch (const std::exception& e) { if (file.is_open()) file.close(); throw std::runtime_error("Failed during password file load/decrypt: " + std::string(e.what()));
    } catch (...) { if(file.is_open()) file.close(); throw std::runtime_error("An unknown error occurred during password file load."); }
}

void PasswordManager::saveToFile() {
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        std::stringstream ss; for (const auto& entry : entries) ss << entry.serialize() << "\n"; std::string serializedData = ss.str();
        std::vector<unsigned char> iv = encryption.generateIV(); // throws
        std::vector<unsigned char> encryptedData = encryption.encrypt(serializedData, masterPassword, salt, iv); // throws
        std::string saltHex = Encryption::toHex(salt);
        std::string ivHex = Encryption::toHex(iv);
        std::string dataHex = Encryption::toHex(encryptedData);
        file.open(filename, std::ios::out | std::ios::trunc); // Open as text, overwrite
        if (!file) { /* Exception should be thrown */ }
        file << saltHex << std::endl; file << ivHex << std::endl; file << dataHex;
        if (!file.good()) { throw std::runtime_error("Error writing data to password file stream."); }
        file.close();
        needsSave = false;
    } catch (const std::ios_base::failure& e) { if(file.is_open()) file.close(); throw std::runtime_error("File I/O error during save: " + std::string(e.what()));
    } catch (const std::exception& e) { if(file.is_open()) file.close(); throw std::runtime_error("Failed to save password data: " + std::string(e.what()));
    } catch(...) { if(file.is_open()) file.close(); throw std::runtime_error("An unknown error occurred during password file save."); }
}

std::string PasswordManager::generatePasswordInternal(int length, bool includeSpecialChars) {
    if (length <= 0) { std::cerr << "Warning: Invalid password length: " << length << "\n"; return ""; }
    const std::string A = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", S = "!@#$%^&*()-_=+[]{}|;:,.<>?";
    std::string V = A; if (includeSpecialChars) V += S; std::random_device r; std::mt19937 g(r());
    if (V.empty()) { return ""; } if (V.size() > static_cast<size_t>(std::numeric_limits<int>::max())) { throw std::runtime_error("Char set too large."); }
    std::uniform_int_distribution<> d(0, static_cast<int>(V.size()) - 1); std::string p = ""; p.reserve(static_cast<size_t>(length));
    for (int i = 0; i < length; ++i) { p += V[static_cast<size_t>(d(g))]; } return p;
}
std::string PasswordManager::generateRandomPassword(int length, bool includeSpecialChars) { return generatePasswordInternal(length, includeSpecialChars); }

bool PasswordManager::addEntry(const PasswordEntry& entry) {
    if (entry.title.empty() || entry.password.empty()) { std::cerr << "Error: Entry title/password empty.\n"; return false; }
    entries.push_back(entry); needsSave = true; return true;
}
PasswordEntry PasswordManager::addEntryWithGeneratedPassword(const std::string& title, const std::string& username, const std::string& url, const std::string& notes, int passwordLength, bool includeSpecialChars) {
    if (title.empty()) { throw std::invalid_argument("Title cannot be empty."); }
    std::string gp = generatePasswordInternal(passwordLength, includeSpecialChars);
    if (gp.empty()) { throw std::runtime_error("Password generation failed."); }
    PasswordEntry ne(title, username, gp, url, notes);
    entries.push_back(ne); needsSave = true; return ne;
}
const std::vector<PasswordEntry>& PasswordManager::getAllEntries() const { return entries; }
std::vector<PasswordEntry> PasswordManager::findEntries(const std::string& searchTerm) const {
    std::vector<PasswordEntry> R; if (searchTerm.empty()) return R; for (const auto& e : entries) { if (e.title.find(searchTerm) != std::string::npos || e.username.find(searchTerm) != std::string::npos || (!e.url.empty() && e.url.find(searchTerm) != std::string::npos)) { R.push_back(e); } } return R;
}
bool PasswordManager::updateEntry(size_t index, const PasswordEntry& updatedEntry) {
    if (index >= entries.size()) { std::cerr << "Error: Invalid index for update: " << index << "\n"; return false; } if (updatedEntry.title.empty() || updatedEntry.password.empty()) { std::cerr << "Error: Updated title/password empty.\n"; return false; } entries[index] = updatedEntry; needsSave = true; return true;
}
bool PasswordManager::deleteEntry(size_t index) {
    if (index >= entries.size()) { std::cerr << "Error: Invalid index for delete: " << index << "\n"; return false; }
    auto it = entries.begin() + static_cast<std::vector<PasswordEntry>::difference_type>(index);
    entries.erase(it); needsSave = true; return true;
}
void PasswordManager::changeMasterPassword(const std::string& newMasterPassword) {
    if (newMasterPassword.empty()) { throw std::invalid_argument("New master password cannot be empty."); }
    masterPassword = newMasterPassword;
    try { salt = encryption.generateSalt(); }
    catch (const std::exception& e) { throw std::runtime_error("Salt generation failed: " + std::string(e.what())); }
    needsSave = true;
}
void PasswordManager::saveChanges() { if (needsSave) saveToFile(); }