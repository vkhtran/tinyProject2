#include "CartItem.h"

#include <iomanip>
#include <iostream>

using namespace std;

CartItem::CartItem() : product(), quantity(0) {}

CartItem::CartItem(const Product& product, int quantity) : product(product), quantity(quantity) {}

const Product& CartItem::getProduct() const {
    return product;
}

int CartItem::getQuantity() const {
    return quantity;
}

double CartItem::getSubtotal() const {
    return product.getPrice() * quantity;
}

void CartItem::setQuantity(int newQuantity) {
    quantity = newQuantity;
}

void CartItem::increaseQuantity(int amount) {
    if (amount > 0) {
        quantity += amount;
    }
}

void CartItem::display() const {
    cout << product.getName() << " x " << quantity
         << " = " << fixed << setprecision(0) << getSubtotal() << "\n";
}
