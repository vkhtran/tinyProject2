#pragma once

#include <vector>

#include "Customer.h"
#include "Order.h"
#include "Product.h"

class StatisticsManager {
public:
    StatisticsManager();

    std::vector<Product> getMostViewedProducts(
        const std::vector<Product>& products,
        int topN
    ) const;

    std::vector<Product> getBestSellingProducts(
        const std::vector<Product>& products,
        int topN
    ) const;

    std::vector<Customer> getMostActiveUsers(
        const std::vector<Customer>& customers,
        int topN
    ) const;

    double calculateRevenue(const std::vector<Order>& orders) const;
};
