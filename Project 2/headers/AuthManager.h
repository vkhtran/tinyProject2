#pragma once

#include <string>
#include <vector>

enum class AccountRole {
    None,
    Customer,
    Admin
};

struct AuthResult {
    bool success;
    std::string username;
    AccountRole role;
    std::string message;
};

struct AuthCredential {
    std::string username;
    std::string pin;
    AccountRole role;
};

class AuthManager {
private:
    std::vector<AuthCredential> credentials;

    AccountRole stringToRole(const std::string& roleText) const;
    std::string roleToString(AccountRole role) const;
    std::string defaultPinForCustomer(const std::string& username) const;

public:
    AuthManager();

    void loadCredentials(const std::string& filePath);
    void saveCredentials(const std::string& filePath) const;

    bool isPinValid(const std::string& pin) const;
    bool isUsernameFormatValid(const std::string& username) const;
    bool credentialExists(const std::string& username) const;

    AuthResult authenticate(const std::string& username, const std::string& pin) const;
    bool addCustomerCredential(const std::string& username, const std::string& pin);
    void seedDefaultCredentials(
        const std::vector<std::string>& customerUsernames,
        const std::vector<std::string>& adminUsernames
    );
};
