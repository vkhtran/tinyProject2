#pragma once

#include "User.h"

using namespace std;

class Admin : public User {
public:
    Admin();
    Admin(const string& username);

    void displayMenu() const override;
};
