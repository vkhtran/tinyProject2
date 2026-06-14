#include "OrderItem.h"

#include <iomanip>
#include <iostream>

using namespace std;

OrderItem::OrderItem() : product(), quantity(0), unitPrice(0.0) {}

OrderItem::OrderItem(const Product& product, int quantity, double unitPrice)
    : product(product), quantity(quantity), unitPrice(unitPrice) {}

const Product& OrderItem::getProduct() const {
    return product;
}

int OrderItem::getQuantity() const {
    return quantity;
}

double OrderItem::getUnitPrice() const {
    return unitPrice;
}

double OrderItem::getSubtotal() const {
    return unitPrice * quantity;
}

void OrderItem::display() const {
    cout << product.getName() << " x " << quantity
         << " @ " << fixed << setprecision(0) << unitPrice
         << " = " << fixed << setprecision(0) << getSubtotal() << "\n";
}
