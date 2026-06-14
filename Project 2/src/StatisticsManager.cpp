#include "StatisticsManager.h"

#include <algorithm>

using namespace std;

StatisticsManager::StatisticsManager() = default;

vector<Product> StatisticsManager::getMostViewedProducts(
    const vector<Product>& products,
    int topN
) const {
    vector<Product> result = products;
    sort(
        result.begin(),
        result.end(),
        [](const Product& left, const Product& right) {
            return left.getViewCount() > right.getViewCount();
        }
    );
    if (topN < static_cast<int>(result.size())) {
        result.resize(topN);
    }
    return result;
}

vector<Product> StatisticsManager::getBestSellingProducts(
    const vector<Product>& products,
    int topN
) const {
    vector<Product> result = products;
    sort(
        result.begin(),
        result.end(),
        [](const Product& left, const Product& right) {
            return left.getPurchaseCount() > right.getPurchaseCount();
        }
    );
    if (topN < static_cast<int>(result.size())) {
        result.resize(topN);
    }
    return result;
}

vector<Customer> StatisticsManager::getMostActiveUsers(
    const vector<Customer>& customers,
    int topN
) const {
    vector<Customer> result = customers;
    sort(
        result.begin(),
        result.end(),
        [](const Customer& left, const Customer& right) {
            return left.getActivityScore() > right.getActivityScore();
        }
    );
    if (topN < static_cast<int>(result.size())) {
        result.resize(topN);
    }
    return result;
}

double StatisticsManager::calculateRevenue(const vector<Order>& orders) const {
    double revenue = 0.0;
    for (const auto& order : orders) {
        revenue += order.getTotalAmount();
    }
    return revenue;
}
