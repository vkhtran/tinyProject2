#pragma once

#include <set>
#include <vector>

#include "Customer.h"
#include "Product.h"

using namespace std;

class RecommendationEngine {
private:
    set<string> buildUserInterestSet(
        const Customer& customer,
        const vector<Product>& products
    ) const;

    set<string> buildProductFeatureSet(const Product& product) const;
    double calculateJaccardSimilarity(
        const set<string>& left,
        const set<string>& right
    ) const;

public:
    RecommendationEngine();

    double calculateScore(const Customer& customer, const Product& product) const;
    double getCategoryBonus(
        const Customer& customer,
        const Product& product,
        const vector<Product>& products
    ) const;
    vector<Product> recommendProducts(
        const Customer& customer,
        const vector<Product>& products,
        int topN
    ) const;
};
