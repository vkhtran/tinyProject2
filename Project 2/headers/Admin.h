#pragma once

#include "User.h"

class Admin : public User {
public:
    Admin();
    Admin(const std::string& username);
};
