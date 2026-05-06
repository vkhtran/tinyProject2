#include "Product.h"

#include <iomanip>
#include <iostream>

using namespace std;

Product::Product()
    : id(0),
      name("Unnamed Product"),
      category(),
      compatibleDevices(),
      tags(),
      price(0.0),
      stock(0),
      viewCount(0),
      purchaseCount(0) {}

Product::Product(
    int id,
    const string& name,
    const Category& category,
    const vector<string>& compatibleDevices,
    const vector<string>& tags,
    double price,
    int stock
)
    : id(id),
      name(name),
      category(category),
      compatibleDevices(compatibleDevices),
      tags(tags),
      price(price),
      stock(stock),
      viewCount(0),
      purchaseCount(0) {}

int Product::getId() const {
    return id;
}

const string& Product::getName() const {
    return name;
}

Category Product::getCategory() const {
    return category;
}

const vector<string>& Product::getCompatibleDevices() const {
    return compatibleDevices;
}

const vector<string>& Product::getTags() const {
    return tags;
}

double Product::getPrice() const {
    return price;
}

int Product::getStock() const {
    return stock;
}

int Product::getViewCount() const {
    return viewCount;
}

int Product::getPurchaseCount() const {
    return purchaseCount;
}

void Product::setName(const string& newName) {
    name = newName;
}

void Product::setCategory(const Category& newCategory) {
    category = newCategory;
}

void Product::setCompatibleDevices(const vector<string>& devices) {
    compatibleDevices = devices;
}

void Product::setTags(const vector<string>& newTags) {
    tags = newTags;
}

void Product::setPrice(double newPrice) {
    price = newPrice;
}

void Product::setStock(int newStock) {
    stock = newStock;
}

void Product::setViewCount(int newViewCount) {
    viewCount = newViewCount;
}

void Product::setPurchaseCount(int newPurchaseCount) {
    purchaseCount = newPurchaseCount;
}

void Product::increaseViewCount() {
    ++viewCount;
}

void Product::increasePurchaseCount(int quantity) {
    if (quantity > 0) {
        purchaseCount += quantity;
    }
}

void Product::decreaseStock(int quantity) {
    if (quantity > 0 && quantity <= stock) {
        stock -= quantity;
    }
}

bool Product::isInStock() const {
    return stock > 0;
}

void Product::displayForCustomer() const {
    cout << "ID: " << id << "\n"
         << "Name: " << name << "\n"
         << "Category: " << categoryToString(category) << "\n"
         << "Price: " << fixed << setprecision(0) << price << "\n"
         << "Stock: " << stock << "\n"
         << "Sold: " << purchaseCount << "\n"
         << "Compatible Devices: ";
    for (size_t i = 0; i < compatibleDevices.size(); ++i) {
        if (i > 0) {
            cout << ", ";
        }
        cout << compatibleDevices[i];
    }
    cout << "\n"
         << "Tags: ";
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) {
            cout << ", ";
        }
        cout << tags[i];
    }
    cout << "\n";
}

void Product::displayForAdmin() const {
    displayForCustomer();
    cout << "Views: " << viewCount << "\n"
         << "Purchases: " << purchaseCount << "\n";
}

void Product::display() const {
    displayForAdmin();
}
