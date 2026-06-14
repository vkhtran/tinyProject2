#pragma once

#include <vector>

#include "InteractionHistory.h"
#include "ShoppingCart.h"
#include "User.h"

class Customer : public User {
private:
    ShoppingCart cart;
    InteractionHistory history;
    std::vector<int> orderHistory;

public:
    Customer();
    Customer(const std::string& username);

    ShoppingCart& getCart();
    const ShoppingCart& getCart() const;

    InteractionHistory& getInteractionHistory();
    const InteractionHistory& getInteractionHistory() const;

    std::vector<int>& getOrderHistory();
    const std::vector<int>& getOrderHistory() const;

    void addOrderId(int orderId);
    int getActivityScore() const;
};
