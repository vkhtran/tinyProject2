#pragma once

#include <string>
#include <vector>

#include "Admin.h"
#include "Category.h"
#include "Customer.h"
#include "FileManager.h"
#include "Order.h"
#include "Product.h"
#include "RecommendationEngine.h"
#include "StatisticsManager.h"

using namespace std;

class ECommerceSystem {
private:
    vector<Product> products;
    vector<Customer> customers;
    vector<Admin> admins;
    vector<Order> orders;

    RecommendationEngine recommendationEngine;
    StatisticsManager statisticsManager;
    FileManager fileManager;

    int nextProductId;
    int nextOrderId;

public:
    ECommerceSystem();

    void run();
    void seedData();
    void loadData();
    void saveData() const;

    void showMainMenu() const;
    void customerMenu(Customer& customer);
    void adminMenu(Admin& admin);

    void viewAllProducts() const;
    void browseProducts(Customer& customer);
    void searchProducts() const;
    void viewProductDetail(Customer& customer, int productId);
    void addToCart(Customer& customer, int productId, int quantity);
    void updateCartMenu(Customer& customer);
    void removeFromCart(Customer& customer);
    void checkout(Customer& customer, bool requireConfirmation = false);
    void viewOrderHistory(const Customer& customer) const;
    void showRecommendations(const Customer& customer, int topN) const;

    void addProduct();
    void editProduct(int productId);
    void deleteProduct(int productId);
    void showStatistics(int topN) const;
    void showMostViewedProducts(int topN) const;
    void showBestSellingProducts(int topN) const;
    void showActiveUsers(int topN) const;

    Product* findProductById(int productId);
    const Product* findProductById(int productId) const;

    Customer* findCustomerByUsername(const string& username);
    Admin* findAdminByUsername(const string& username);
    bool isUsernameTaken(const string& username) const;

    int generateProductId();
    int generateOrderId();
};
