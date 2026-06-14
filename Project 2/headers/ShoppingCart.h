#pragma once

#include <vector>

#include "CartItem.h"

class ShoppingCart {
private:
    std::vector<CartItem> items;

public:
    ShoppingCart();

    void addProduct(const Product& product, int quantity);
    void removeProduct(int productId);
    void updateQuantity(int productId, int quantity);
    void clear();

    bool isEmpty() const;
    double calculateTotal() const;
    int getItemCount() const;

    std::vector<CartItem>& getItems();
    const std::vector<CartItem>& getItems() const;

    void displayCart() const;
};
