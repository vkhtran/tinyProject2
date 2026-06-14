#pragma once

#include <string>

class User {
protected:
    std::string username;

public:
    User();
    User(const std::string& username);
    virtual ~User() = default;

    const std::string& getUsername() const;

    void setUsername(const std::string& newUsername);
};
