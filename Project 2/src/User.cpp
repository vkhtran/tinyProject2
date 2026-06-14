#include "User.h"

using namespace std;

User::User() : username("guest") {}

User::User(const string& username) : username(username) {}

const string& User::getUsername() const {
    return username;
}

void User::setUsername(const string& newUsername) {
    username = newUsername;
}
