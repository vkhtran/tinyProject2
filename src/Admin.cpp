#include "Admin.h"

#include <iostream>

using namespace std;

Admin::Admin() : User("admin", "admin") {}

Admin::Admin(const string& username) : User(username, "admin") {}

void Admin::displayMenu() const {
    cout << "Admin Menu\n"
         << "1. Add product\n"
         << "2. Edit product\n"
         << "3. Delete product\n"
         << "4. View all products\n"
         << "5. View most viewed products\n"
         << "6. View best selling products\n"
         << "7. View active users\n"
         << "0. Logout\n";
}
