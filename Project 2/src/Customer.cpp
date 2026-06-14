#include "Customer.h"

using namespace std;

Customer::Customer() : User("customer"), cart(), history(), orderHistory() {}

Customer::Customer(const string& username)
    : User(username), cart(), history(), orderHistory() {}

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
    return history.getTotalViews() + (history.getTotalPurchases() * 4);
}
