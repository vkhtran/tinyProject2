#include "Customer.h"

#include <iostream>

using namespace std;

Customer::Customer() : User("customer", "customer"), cart(), history(), orderHistory() {}

Customer::Customer(const string& username)
    : User(username, "customer"), cart(), history(), orderHistory() {}

ShoppingCart& Customer::getCart() {
    return cart;
}

const ShoppingCart& Customer::getCart() const {
    return cart;
}

InteractionHistory& Customer::getInteractionHistory() {
    return history;
}

const InteractionHistory& Customer::getInteractionHistory() const {
    return history;
}

vector<int>& Customer::getOrderHistory() {
    return orderHistory;
}

const vector<int>& Customer::getOrderHistory() const {
    return orderHistory;
}

void Customer::addOrderId(int orderId) {
    orderHistory.push_back(orderId);
}

int Customer::getActivityScore() const {
    return history.getTotalViews() + (history.getTotalPurchases() * 2);
}

void Customer::displayMenu() const {
    cout << "Customer Menu\n"
         << "1. View products\n"
         << "2. Search product\n"
         << "3. Add to cart\n"
         << "4. View cart\n"
         << "5. Update cart\n"
         << "6. Checkout\n"
         << "7. View order history\n"
         << "8. View recommendations\n"
         << "0. Logout\n";
}
