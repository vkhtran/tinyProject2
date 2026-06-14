#pragma once

#include <set>
#include <vector>

#include "Customer.h"
#include "Product.h"

struct RecommendationResult {
    Product product;
    double viewScore;
    double purchaseScore;
    double categoryBonus;
    double jaccardBonus;
    double finalScore;
};

class RecommendationEngine {
private:
    std::set<std::string> buildUserInterestSet(
        const Customer& customer,
        const std::vector<Product>& products
    ) const;

    std::set<std::string> buildProductFeatureSet(const Product& product) const;
    double calculateJaccardSimilarity(
        const std::set<std::string>& left,
        const std::set<std::string>& right
    ) const;

public:
    RecommendationEngine();

    double calculateScore(const Customer& customer, const Product& product) const;
    double getCategoryBonus(
        const Customer& customer,
        const Product& product,
        const std::vector<Product>& products
    ) const;
    std::vector<Product> recommendProducts(
        const Customer& customer,
        const std::vector<Product>& products,
        int topN
    ) const;
    std::vector<RecommendationResult> recommendProductsWithScores(
        const Customer& customer,
        const std::vector<Product>& products,
        int topN
    ) const;
};
