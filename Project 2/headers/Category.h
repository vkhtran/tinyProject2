#pragma once

#include <string>
#include <vector>

enum class Category {
    iPhone = 1,
    iPad = 2,
    AirPods = 3,
    Charging = 4,
    Protection = 5,
    Universal = 6
};

std::string categoryToString(Category category);
Category intToCategory(int categoryId);
std::vector<Category> getAllCategories();
