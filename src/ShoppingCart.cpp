#include "ShoppingCart.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace std;

ShoppingCart::ShoppingCart() : items() {}

void ShoppingCart::addProduct(const Product& product, int quantity) {
    if (quantity <= 0) {
        return;
    }

    for (auto& item : items) {
        if (item.getProduct().getId() == product.getId()) {
            item.increaseQuantity(quantity);
            return;
        }
    }

    items.emplace_back(product, quantity);
}

void ShoppingCart::removeProduct(int productId) {
    items.erase(
        remove_if(
            items.begin(),
            items.end(),
            [productId](const CartItem& item) {
                return item.getProduct().getId() == productId;
            }
        ),
        items.end()
    );
}

void ShoppingCart::updateQuantity(int productId, int quantity) {
    for (auto& item : items) {
        if (item.getProduct().getId() == productId) {
            if (quantity <= 0) {
                removeProduct(productId);
            } else {
                item.setQuantity(quantity);
            }
            return;
        }
    }
}

void ShoppingCart::clear() {
    items.clear();
}

bool ShoppingCart::isEmpty() const {
    return items.empty();
}

double ShoppingCart::calculateTotal() const {
    double total = 0.0;
    for (const auto& item : items) {
        total += item.getSubtotal();
    }
    return total;
}

int ShoppingCart::getItemCount() const {
    int count = 0;
    for (const auto& item : items) {
        count += item.getQuantity();
    }
    return count;
}

vector<CartItem>& ShoppingCart::getItems() {
    return items;
}

const vector<CartItem>& ShoppingCart::getItems() const {
    return items;
}

void ShoppingCart::displayCart() const {
    if (items.empty()) {
        cout << "Cart is empty.\n";
        return;
    }

    for (const auto& item : items) {
        item.display();
    }

    cout << "Total: " << fixed << setprecision(0) << calculateTotal() << "\n";
}
