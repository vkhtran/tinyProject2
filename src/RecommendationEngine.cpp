#include "RecommendationEngine.h"

#include <algorithm>
#include <map>
#include <set>
#include <utility>

using namespace std;

RecommendationEngine::RecommendationEngine() = default;

set<string> RecommendationEngine::buildUserInterestSet(
    const Customer& customer,
    const vector<Product>& products
) const {
    set<string> interestSet;
    const auto& viewed = customer.getInteractionHistory().getViewedProducts();
    const auto& purchased = customer.getInteractionHistory().getPurchasedProducts();

    auto addProductFeatures = [&interestSet](const Product& product) {
        interestSet.insert(categoryToString(product.getCategory()));
        for (const auto& device : product.getCompatibleDevices()) {
            interestSet.insert(device);
        }
        for (const auto& tag : product.getTags()) {
            interestSet.insert(tag);
        }
    };

    for (const auto& entry : viewed) {
        for (const auto& product : products) {
            if (product.getId() == entry.first) {
                addProductFeatures(product);
                break;
            }
        }
    }

    for (const auto& entry : purchased) {
        for (const auto& product : products) {
            if (product.getId() == entry.first) {
                addProductFeatures(product);
                break;
            }
        }
    }

    return interestSet;
}

set<string> RecommendationEngine::buildProductFeatureSet(const Product& product) const {
    set<string> featureSet;
    featureSet.insert(categoryToString(product.getCategory()));

    for (const auto& device : product.getCompatibleDevices()) {
        featureSet.insert(device);
    }

    for (const auto& tag : product.getTags()) {
        featureSet.insert(tag);
    }

    return featureSet;
}

double RecommendationEngine::calculateJaccardSimilarity(
    const set<string>& left,
    const set<string>& right
) const {
    if (left.empty() && right.empty()) {
        return 0.0;
    }

    int intersectionCount = 0;
    for (const auto& value : left) {
        if (right.count(value) > 0) {
            ++intersectionCount;
        }
    }

    int unionCount = static_cast<int>(left.size() + right.size() - intersectionCount);
    if (unionCount == 0) {
        return 0.0;
    }

    return static_cast<double>(intersectionCount) / unionCount;
}

double RecommendationEngine::calculateScore(const Customer& customer, const Product& product) const {
    (void)customer;
    (void)product;
    return 0.0;
}

double RecommendationEngine::getCategoryBonus(
    const Customer& customer,
    const Product& product,
    const vector<Product>& products
) const {
    map<string, int> categoryScores;

    for (const auto& entry : customer.getInteractionHistory().getViewedProducts()) {
        for (const auto& item : products) {
            if (item.getId() == entry.first) {
                categoryScores[categoryToString(item.getCategory())] += entry.second;
                break;
            }
        }
    }

    for (const auto& entry : customer.getInteractionHistory().getPurchasedProducts()) {
        for (const auto& item : products) {
            if (item.getId() == entry.first) {
                categoryScores[categoryToString(item.getCategory())] += entry.second * 2;
                break;
            }
        }
    }

    string favoriteCategory;
    int bestScore = -1;
    for (const auto& entry : categoryScores) {
        if (entry.second > bestScore) {
            bestScore = entry.second;
            favoriteCategory = entry.first;
        }
    }

    if (!favoriteCategory.empty() && categoryToString(product.getCategory()) == favoriteCategory) {
        return 1.0;
    }

    return 0.0;
}

vector<Product> RecommendationEngine::recommendProducts(
    const Customer& customer,
    const vector<Product>& products,
    int topN
) const {
    vector<pair<Product, double>> scoredProducts;
    const InteractionHistory& history = customer.getInteractionHistory();
    set<string> userInterestSet = buildUserInterestSet(customer, products);

    for (const auto& product : products) {
        if (!product.isInStock()) {
            continue;
        }

        const double viewScore = history.getViewCount(product.getId()) * 0.2;
        const double purchaseScore = history.getPurchaseCount(product.getId()) * 0.8;
        const double categoryBonus = getCategoryBonus(customer, product, products);
        const double jaccardBonus =
            calculateJaccardSimilarity(userInterestSet, buildProductFeatureSet(product));

        const double finalScore = viewScore + purchaseScore + categoryBonus + jaccardBonus;
        scoredProducts.emplace_back(product, finalScore);
    }

    sort(
        scoredProducts.begin(),
        scoredProducts.end(),
        [](const auto& left, const auto& right) {
            return left.second > right.second;
        }
    );

    vector<Product> recommendations;
    for (int i = 0; i < static_cast<int>(scoredProducts.size()) && i < topN; ++i) {
        recommendations.push_back(scoredProducts[i].first);
    }

    return recommendations;
}
