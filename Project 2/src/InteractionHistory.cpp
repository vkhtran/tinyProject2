#include "InteractionHistory.h"

using namespace std;

InteractionHistory::InteractionHistory() : viewedProducts(), purchasedProducts() {}

void InteractionHistory::addView(int productId) {
    ++viewedProducts[productId];
}

void InteractionHistory::addPurchase(int productId, int quantity) {
    if (quantity > 0) {
        purchasedProducts[productId] += quantity;
    }
}

int InteractionHistory::getViewCount(int productId) const {
    auto it = viewedProducts.find(productId);
    return it == viewedProducts.end() ? 0 : it->second;
}

int InteractionHistory::getPurchaseCount(int productId) const {
    auto it = purchasedProducts.find(productId);
    return it == purchasedProducts.end() ? 0 : it->second;
}

int InteractionHistory::getTotalViews() const {
    int total = 0;
    for (const auto& entry : viewedProducts) {
        total += entry.second;
    }
    return total;
}

int InteractionHistory::getTotalPurchases() const {
    int total = 0;
    for (const auto& entry : purchasedProducts) {
        total += entry.second;
    }
    return total;
}

const map<int, int>& InteractionHistory::getViewedProducts() const {
    return viewedProducts;
}

const map<int, int>& InteractionHistory::getPurchasedProducts() const {
    return purchasedProducts;
}

void InteractionHistory::clear() {
    viewedProducts.clear();
    purchasedProducts.clear();
}
