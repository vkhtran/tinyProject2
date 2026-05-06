#include "Category.h"

using namespace std;

string categoryToString(Category category) {
    switch (category) {
    case Category::iPhone:
        return "iPhone";
    case Category::iPad:
        return "iPad";
    case Category::AirPods:
        return "AirPods";
    case Category::Charging:
        return "Charging";
    case Category::Protection:
        return "Protection";
    case Category::Universal:
        return "Universal";
    default:
        return "Unknown";
    }
}

Category intToCategory(int categoryId) {
    switch (categoryId) {
    case 1:
        return Category::iPhone;
    case 2:
        return Category::iPad;
    case 3:
        return Category::AirPods;
    case 4:
        return Category::Charging;
    case 5:
        return Category::Protection;
    case 6:
        return Category::Universal;
    default:
        return Category::Universal;
    }
}

vector<Category> getAllCategories() {
    return {
        Category::iPhone,
        Category::iPad,
        Category::AirPods,
        Category::Charging,
        Category::Protection,
        Category::Universal
    };
}
