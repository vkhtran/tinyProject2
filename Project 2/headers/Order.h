#pragma once

#include <vector>

#include "OrderItem.h"

class Order {
private:
    int orderId;
    std::string customerUsername;
    std::vector<OrderItem> items;
    double totalAmount;

public:
    Order();
    Order(int orderId, const std::string& customerUsername);

    int getOrderId() const;
    const std::string& getCustomerUsername() const;
    double getTotalAmount() const;
    const std::vector<OrderItem>& getItems() const;

    void addItem(const OrderItem& item);
    void calculateTotal();

    void display() const;
};
