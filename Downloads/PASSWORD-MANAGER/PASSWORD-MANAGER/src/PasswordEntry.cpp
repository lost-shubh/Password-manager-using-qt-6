#include "PasswordEntry.h"

#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

// Parameterized Constructor (Default constructor is defaulted in header)
PasswordEntry::PasswordEntry(const std::string& title_, const std::string& username_,
                           const std::string& password_, const std::string& url_,
                           const std::string& notes_)
    : title(title_), username(username_), password(password_), url(url_), notes(notes_)
{} // Empty body

// --- Serialization/Deserialization ---

std::string PasswordEntry::serialize() const {
    std::stringstream ss;
    ss << title << '|' << username << '|' << password << '|' << url << '|' << notes;
    return ss.str();
}

PasswordEntry PasswordEntry::deserialize(const std::string& data) {
    PasswordEntry entry; // Default construction initializes members to "" via header
    std::stringstream ss(data);
    std::string item;
    std::vector<std::string*> fields = {&entry.title, &entry.username, &entry.password, &entry.url, &entry.notes};
    size_t fieldCount = 0;
    while (fieldCount < fields.size() && std::getline(ss, item, '|')) {
        *(fields[fieldCount]) = item; // Assign read item to the corresponding member
        fieldCount++;
    }
    return entry;
}