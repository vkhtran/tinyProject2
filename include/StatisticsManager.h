#pragma once

#include <vector>

#include "Customer.h"
#include "Order.h"
#include "Product.h"

using namespace std;

class StatisticsManager {
public:
    StatisticsManager();

    vector<Product> getMostViewedProducts(
        const vector<Product>& products,
        int topN
    ) const;

    vector<Product> getBestSellingProducts(
        const vector<Product>& products,
        int topN
    ) const;

    vector<Customer> getMostActiveUsers(
        const vector<Customer>& customers,
        int topN
    ) const;

    double calculateRevenue(const vector<Order>& orders) const;
};
