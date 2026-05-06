#pragma once

#include <string>
#include <vector>

using namespace std;

enum class Category {
    iPhone = 1,
    iPad = 2,
    AirPods = 3,
    Charging = 4,
    Protection = 5,
    Universal = 6
};

string categoryToString(Category category);
Category intToCategory(int categoryId);
vector<Category> getAllCategories();
