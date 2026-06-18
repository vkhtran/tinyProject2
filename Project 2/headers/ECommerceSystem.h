#pragma once

#include <string>
#include <vector>

#include "Admin.h"
#include "AuthManager.h"
#include "Category.h"
#include "Customer.h"
#include "FileManager.h"
#include "Order.h"
#include "Product.h"
#include "RecommendationEngine.h"
#include "StatisticsManager.h"

class ECommerceSystem {
private:
    std::vector<Product> products;
    std::vector<Customer> customers;
    std::vector<Admin> admins;
    std::vector<Order> orders;

    AuthManager authManager;
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
    void loginFlow();
    void registerCustomer();
    void customerMenu(Customer& customer);
    void shopProductsMenu(Customer& customer);
    void viewCartMenu(Customer& customer);
    void adminMenu(Admin& admin);
    void adminInventoryMenu(const Admin& admin);
    void adminInsightsMenu(const Admin& admin);
    void showAdminDashboard(const Admin& admin) const;
    void showAdminInventoryTable() const;
    void showCustomerRecommendationsForAdmin(const Admin& admin);
    double calculateCustomerSpending(const std::string& username) const;

    void viewAllProducts(Customer& customer);
    void searchProducts(Customer& customer);
    void showBestSellingProducts(Customer& customer, int topN);
    bool handleProductListAction(Customer& customer, const std::vector<const Product*>& productList);
    void viewProductDetail(Customer& customer, int productId);
    void addToCart(Customer& customer, int productId, int quantity);
    void updateCartMenu(Customer& customer);
    void checkout(Customer& customer, bool requireConfirmation = false);
    void viewOrderHistory(const Customer& customer) const;
    void showRecommendations(Customer& customer, int topN);

    void addProduct();
    void editProduct(int productId);
    void deleteProduct(int productId);
    void showMostViewedProducts(int topN) const;
    void showBestSellingProducts(int topN) const;
    void showActiveUsers(int topN) const;

    Product* findProductById(int productId);
    const Product* findProductById(int productId) const;

    Customer* findCustomerByUsername(const std::string& username);
    Admin* findAdminByUsername(const std::string& username);
    bool isUsernameTaken(const std::string& username) const;

    int generateProductId();
    int generateOrderId();
};
