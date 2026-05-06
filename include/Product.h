#pragma once

#include <string>
#include <vector>

#include "Category.h"

using namespace std;

class Product {
private:
    int id;
    string name;
    Category category;
    vector<string> compatibleDevices;
    vector<string> tags;
    double price;
    int stock;
    int viewCount;
    int purchaseCount;

public:
    Product();
    Product(
        int id,
        const string& name,
        const Category& category,
        const vector<string>& compatibleDevices,
        const vector<string>& tags,
        double price,
        int stock
    );

    int getId() const;
    const string& getName() const;
    Category getCategory() const;
    const vector<string>& getCompatibleDevices() const;
    const vector<string>& getTags() const;
    double getPrice() const;
    int getStock() const;
    int getViewCount() const;
    int getPurchaseCount() const;

    void setName(const string& newName);
    void setCategory(const Category& newCategory);
    void setCompatibleDevices(const vector<string>& devices);
    void setTags(const vector<string>& newTags);
    void setPrice(double newPrice);
    void setStock(int newStock);
    void setViewCount(int newViewCount);
    void setPurchaseCount(int newPurchaseCount);

    void increaseViewCount();
    void increasePurchaseCount(int quantity = 1);
    void decreaseStock(int quantity);
    bool isInStock() const;

    void display() const;
    void displayForCustomer() const;
    void displayForAdmin() const;
};
