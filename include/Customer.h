#pragma once

#include <vector>

#include "InteractionHistory.h"
#include "ShoppingCart.h"
#include "User.h"

using namespace std;

class Customer : public User {
private:
    ShoppingCart cart;
    InteractionHistory history;
    vector<int> orderHistory;

public:
    Customer();
    Customer(const string& username);

    ShoppingCart& getCart();
    const ShoppingCart& getCart() const;

    InteractionHistory& getInteractionHistory();
    const InteractionHistory& getInteractionHistory() const;

    vector<int>& getOrderHistory();
    const vector<int>& getOrderHistory() const;

    void addOrderId(int orderId);
    int getActivityScore() const;

    void displayMenu() const override;
};
