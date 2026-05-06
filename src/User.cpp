#include "User.h"

using namespace std;

User::User() : username("guest"), role("unknown") {}

User::User(const string& username, const string& role)
    : username(username), role(role) {}

const string& User::getUsername() const {
    return username;
}

const string& User::getRole() const {
    return role;
}

void User::setUsername(const string& newUsername) {
    username = newUsername;
}

void User::setRole(const string& newRole) {
    role = newRole;
}
