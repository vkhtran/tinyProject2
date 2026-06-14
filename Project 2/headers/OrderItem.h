#pragma once

#include "Product.h"

class OrderItem {
private:
    Product product;
    int quantity;
    double unitPrice;

public:
    OrderItem();
    OrderItem(const Product& product, int quantity, double unitPrice);

    const Product& getProduct() const;
    int getQuantity() const;
    double getUnitPrice() const;
    double getSubtotal() const;

    void display() const;
};
