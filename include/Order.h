#pragma once

#include <vector>

#include "OrderItem.h"

using namespace std;

class Order {
private:
    int orderId;
    string customerUsername;
    vector<OrderItem> items;
    double totalAmount;

public:
    Order();
    Order(int orderId, const string& customerUsername);

    int getOrderId() const;
    const string& getCustomerUsername() const;
    double getTotalAmount() const;
    const vector<OrderItem>& getItems() const;

    void addItem(const OrderItem& item);
    void calculateTotal();

    void display() const;
};
