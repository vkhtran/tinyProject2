#pragma once

#include <map>

using namespace std;

class InteractionHistory {
private:
    map<int, int> viewedProducts;
    map<int, int> purchasedProducts;

public:
    InteractionHistory();

    void addView(int productId);
    void addPurchase(int productId, int quantity);

    int getViewCount(int productId) const;
    int getPurchaseCount(int productId) const;

    int getTotalViews() const;
    int getTotalPurchases() const;

    const map<int, int>& getViewedProducts() const;
    const map<int, int>& getPurchasedProducts() const;

    void clear();
};
