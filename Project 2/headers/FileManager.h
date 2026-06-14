#pragma once

#include <string>
#include <vector>

#include "Admin.h"
#include "Category.h"
#include "Customer.h"
#include "Order.h"
#include "Product.h"

class FileManager {
public:
    FileManager();

    void saveProducts(const std::vector<Product>& products, const std::string& filePath) const;
    std::vector<Product> loadProducts(const std::string& filePath) const;

    void saveCustomers(const std::vector<Customer>& customers, const std::string& filePath) const;
    std::vector<Customer> loadCustomers(const std::string& filePath) const;

    void saveAdmins(const std::vector<Admin>& admins, const std::string& filePath) const;
    std::vector<Admin> loadAdmins(const std::string& filePath) const;

    void saveOrders(const std::vector<Order>& orders, const std::string& filePath) const;
    std::vector<Order> loadOrders(const std::string& filePath) const;
};
