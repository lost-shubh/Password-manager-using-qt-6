#ifndef PASSWORD_ENTRY_H
#define PASSWORD_ENTRY_H

#include <string> // For std::string

class PasswordEntry {
public:
    std::string title = "";
    std::string username = "";
    std::string password = "";
    std::string url = "";
    std::string notes = "";

    PasswordEntry() = default;
    PasswordEntry(const std::string& title_, const std::string& username_,
                 const std::string& password_, const std::string& url_ = "",
                 const std::string& notes_ = "");

    std::string serialize() const;
    static PasswordEntry deserialize(const std::string& data);
};

#endif // PASSWORD_ENTRY_H