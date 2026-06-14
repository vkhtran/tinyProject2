#include "AuthManager.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;

AuthManager::AuthManager() : credentials() {}

AccountRole AuthManager::stringToRole(const string& roleText) const {
    if (roleText == "customer") {
        return AccountRole::Customer;
    }

    if (roleText == "admin") {
        return AccountRole::Admin;
    }

    return AccountRole::None;
}

string AuthManager::roleToString(AccountRole role) const {
    if (role == AccountRole::Customer) {
        return "customer";
    }

    if (role == AccountRole::Admin) {
        return "admin";
    }

    return "none";
}

string AuthManager::defaultPinForCustomer(const string& username) const {
    if (username == "user2") {
        return "2222";
    }

    return "1111";
}

void AuthManager::loadCredentials(const string& filePath) {
    credentials.clear();

    ifstream inFile(filePath);
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream stream(line);
        string username;
        string pin;
        string roleText;

        getline(stream, username, '|');
        getline(stream, pin, '|');
        getline(stream, roleText, '|');

        AccountRole role = stringToRole(roleText);
        if (!isUsernameFormatValid(username) || !isPinValid(pin) || role == AccountRole::None
            || credentialExists(username)) {
            continue;
        }

        credentials.push_back({username, pin, role});
    }
}

void AuthManager::saveCredentials(const string& filePath) const {
    filesystem::path path(filePath);
    if (path.has_parent_path()) {
        filesystem::create_directories(path.parent_path());
    }

    ofstream outFile(filePath);
    for (const auto& credential : credentials) {
        outFile << credential.username << "|"
                << credential.pin << "|"
                << roleToString(credential.role) << "\n";
    }
}

bool AuthManager::isPinValid(const string& pin) const {
    if (pin.length() != 4) {
        return false;
    }

    for (char ch : pin) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return true;
}

bool AuthManager::isUsernameFormatValid(const string& username) const {
    if (username.empty() || username == "0") {
        return false;
    }

    if (username[0] < 'a' || username[0] > 'z') {
        return false;
    }

    for (char ch : username) {
        bool isLowercaseLetter = ch >= 'a' && ch <= 'z';
        bool isDigit = ch >= '0' && ch <= '9';
        if (!isLowercaseLetter && !isDigit) {
            return false;
        }
    }

    return true;
}

bool AuthManager::credentialExists(const string& username) const {
    for (const auto& credential : credentials) {
        if (credential.username == username) {
            return true;
        }
    }

    return false;
}

AuthResult AuthManager::authenticate(const string& username, const string& pin) const {
    for (const auto& credential : credentials) {
        if (credential.username != username) {
            continue;
        }

        if (credential.pin != pin) {
            return {false, username, AccountRole::None, "Incorrect PIN."};
        }

        return {true, username, credential.role, "Login successful."};
    }

    return {false, username, AccountRole::None, "Username not found."};
}

bool AuthManager::addCustomerCredential(const string& username, const string& pin) {
    if (!isUsernameFormatValid(username) || !isPinValid(pin) || credentialExists(username)) {
        return false;
    }

    credentials.push_back({username, pin, AccountRole::Customer});
    return true;
}

void AuthManager::seedDefaultCredentials(
    const vector<string>& customerUsernames,
    const vector<string>& adminUsernames
) {
    for (const auto& username : customerUsernames) {
        if (!credentialExists(username)) {
            credentials.push_back({username, defaultPinForCustomer(username), AccountRole::Customer});
        }
    }

    for (const auto& username : adminUsernames) {
        if (!credentialExists(username)) {
            credentials.push_back({username, "0000", AccountRole::Admin});
        }
    }
}
