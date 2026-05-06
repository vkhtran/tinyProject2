#pragma once

#include <string>
#include <vector>

#include "Admin.h"
#include "Category.h"
#include "Customer.h"
#include "Order.h"
#include "Product.h"

using namespace std;

class FileManager {
public:
    FileManager();

    void saveProducts(const vector<Product>& products, const string& filePath) const;
    vector<Product> loadProducts(const string& filePath) const;

    void saveCustomers(const vector<Customer>& customers, const string& filePath) const;
    vector<Customer> loadCustomers(const string& filePath) const;

    void saveAdmins(const vector<Admin>& admins, const string& filePath) const;
    vector<Admin> loadAdmins(const string& filePath) const;

    void saveOrders(const vector<Order>& orders, const string& filePath) const;
    vector<Order> loadOrders(const string& filePath) const;
};
