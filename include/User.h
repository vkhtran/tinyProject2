#pragma once

#include <string>

using namespace std;

class User {
protected:
    string username;
    string role;

public:
    User();
    User(const string& username, const string& role);
    virtual ~User() = default;

    const string& getUsername() const;
    const string& getRole() const;

    void setUsername(const string& newUsername);
    void setRole(const string& newRole);

    virtual void displayMenu() const = 0;
};
