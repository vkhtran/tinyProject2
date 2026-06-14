#pragma once

#include "Product.h"

class CartItem {
private:
    Product product;
    int quantity;

public:
    CartItem();
    CartItem(const Product& product, int quantity);

    const Product& getProduct() const;
    int getQuantity() const;
    double getSubtotal() const;

    void setQuantity(int newQuantity);
    void increaseQuantity(int amount);

    void display() const;
};
