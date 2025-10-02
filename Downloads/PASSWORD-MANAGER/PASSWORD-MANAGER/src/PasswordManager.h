#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <string>
#include <vector>
#include <stdexcept>

#include "Encryption.h"
#include "PasswordEntry.h"

class PasswordManager {
private:
    std::string masterPassword;
    std::vector<PasswordEntry> entries;
    Encryption encryption;
    std::vector<unsigned char> salt;
    std::string filename;
    bool needsSave = false;

    void loadFromFile(); // throws std::runtime_error
    void saveToFile();   // throws std::runtime_error
    std::string generatePasswordInternal(int length, bool includeSpecialChars);

public:
    PasswordManager(const std::string& masterPass, const std::string& storeFilename = "passwords.dat"); // throws
    ~PasswordManager();

    PasswordManager(const PasswordManager&) = delete;
    PasswordManager& operator=(const PasswordManager&) = delete;
    PasswordManager(PasswordManager&&) = delete;
    PasswordManager& operator=(PasswordManager&&) = delete;

    bool addEntry(const PasswordEntry& entry);
    PasswordEntry addEntryWithGeneratedPassword(const std::string& title, const std::string& username,
                                                const std::string& url = "", const std::string& notes = "",
                                                int passwordLength = 16, bool includeSpecialChars = true); // throws
    const std::vector<PasswordEntry>& getAllEntries() const;
    std::vector<PasswordEntry> findEntries(const std::string& searchTerm) const;
    bool updateEntry(size_t index, const PasswordEntry& updatedEntry);
    bool deleteEntry(size_t index);
    void changeMasterPassword(const std::string& newMasterPassword); // throws
    std::string generateRandomPassword(int length = 16, bool includeSpecialChars = true);
    void saveChanges(); // throws
};

#endif // PASSWORD_MANAGER_H