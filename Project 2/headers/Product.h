#pragma once

#include <string>
#include <vector>

#include "Category.h"

class Product {
private:
    int id;
    std::string name;
    Category category;
    std::vector<std::string> compatibleDevices;
    std::vector<std::string> tags;
    double price;
    int stock;
    int viewCount;
    int purchaseCount;

public:
    Product();
    Product(
        int id,
        const std::string& name,
        const Category& category,
        const std::vector<std::string>& compatibleDevices,
        const std::vector<std::string>& tags,
        double price,
        int stock
    );

    int getId() const;
    const std::string& getName() const;
    Category getCategory() const;
    const std::vector<std::string>& getCompatibleDevices() const;
    const std::vector<std::string>& getTags() const;
    double getPrice() const;
    int getStock() const;
    int getViewCount() const;
    int getPurchaseCount() const;

    void setName(const std::string& newName);
    void setCategory(const Category& newCategory);
    void setCompatibleDevices(const std::vector<std::string>& devices);
    void setTags(const std::vector<std::string>& newTags);
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
