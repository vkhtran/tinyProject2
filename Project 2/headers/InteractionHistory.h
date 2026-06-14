#pragma once

#include <map>

class InteractionHistory {
private:
    std::map<int, int> viewedProducts;
    std::map<int, int> purchasedProducts;

public:
    InteractionHistory();

    void addView(int productId);
    void addPurchase(int productId, int quantity);

    int getViewCount(int productId) const;
    int getPurchaseCount(int productId) const;

    int getTotalViews() const;
    int getTotalPurchases() const;

    const std::map<int, int>& getViewedProducts() const;
    const std::map<int, int>& getPurchasedProducts() const;

    void clear();
};
