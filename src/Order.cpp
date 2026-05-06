#include "Order.h"

#include <iomanip>
#include <iostream>

using namespace std;

Order::Order() : orderId(0), customerUsername(""), items(), totalAmount(0.0) {}

Order::Order(int orderId, const string& customerUsername)
    : orderId(orderId), customerUsername(customerUsername), items(), totalAmount(0.0) {}

int Order::getOrderId() const {
    return orderId;
}

const string& Order::getCustomerUsername() const {
    return customerUsername;
}

double Order::getTotalAmount() const {
    return totalAmount;
}

const vector<OrderItem>& Order::getItems() const {
    return items;
}

void Order::addItem(const OrderItem& item) {
    items.push_back(item);
}

void Order::calculateTotal() {
    totalAmount = 0.0;
    for (const auto& item : items) {
        totalAmount += item.getSubtotal();
    }
}

void Order::display() const {
    cout << "Order ID: " << orderId << "\n"
         << "Customer Username: " << customerUsername << "\n";
    for (const auto& item : items) {
        item.display();
    }
    cout << "Total Amount: " << fixed << setprecision(0) << totalAmount << "\n";
}
