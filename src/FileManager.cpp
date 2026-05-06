#include "FileManager.h"

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;

namespace {
vector<string> splitString(const string& input, char delimiter) {
    vector<string> parts;
    string current;

    for (char ch : input) {
        if (ch == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    parts.push_back(current);

    return parts;
}

string joinStrings(const vector<string>& values, char delimiter) {
    stringstream stream;
    bool first = true;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i].empty()) {
            continue;
        }
        if (!first) {
            stream << delimiter;
        }
        stream << values[i];
        first = false;
    }
    return stream.str();
}

vector<string> deserializeStringList(const string& input) {
    vector<string> values;
    if (input.empty()) {
        return values;
    }

    for (const auto& part : splitString(input, ',')) {
        if (!part.empty()) {
            values.push_back(part);
        }
    }

    return values;
}

string serializeMap(const map<int, int>& values) {
    stringstream stream;
    bool first = true;
    for (const auto& entry : values) {
        if (!first) {
            stream << ",";
        }
        stream << entry.first << ":" << entry.second;
        first = false;
    }
    return stream.str();
}

map<int, int> deserializeMap(const string& input) {
    map<int, int> values;
    if (input.empty()) {
        return values;
    }

    vector<string> parts = splitString(input, ',');
    for (const auto& part : parts) {
        vector<string> pair = splitString(part, ':');
        if (pair.size() == 2) {
            values[stoi(pair[0])] = stoi(pair[1]);
        }
    }
    return values;
}

string serializeIntVector(const vector<int>& values) {
    stringstream stream;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            stream << ",";
        }
        stream << values[i];
    }
    return stream.str();
}

vector<int> deserializeIntVector(const string& input) {
    vector<int> values;
    if (input.empty()) {
        return values;
    }

    vector<string> parts = splitString(input, ',');
    for (const auto& part : parts) {
        if (!part.empty()) {
            values.push_back(stoi(part));
        }
    }
    return values;
}

void ensureParentDirectory(const string& filePath) {
    filesystem::path path(filePath);
    if (path.has_parent_path()) {
        filesystem::create_directories(path.parent_path());
    }
}
}

FileManager::FileManager() = default;

void FileManager::saveProducts(const vector<Product>& products, const string& filePath) const {
    ensureParentDirectory(filePath);
    ofstream outFile(filePath);
    for (const auto& product : products) {
        outFile << product.getId() << "|"
                << product.getName() << "|"
                << static_cast<int>(product.getCategory()) << "|"
                << categoryToString(product.getCategory()) << "|"
                << joinStrings(product.getCompatibleDevices(), ',') << "|"
                << joinStrings(product.getTags(), ',') << "|"
                << product.getPrice() << "|"
                << product.getStock() << "|"
                << product.getViewCount() << "|"
                << product.getPurchaseCount() << "\n";
    }
}

vector<Product> FileManager::loadProducts(const string& filePath) const {
    ifstream inFile(filePath);
    vector<Product> products;
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> parts = splitString(line, '|');
        if (parts.size() < 9) {
            continue;
        }

        Category category = intToCategory(stoi(parts[2]));
        vector<string> devices = deserializeStringList(parts[4]);
        vector<string> tags;
        double price;
        int stock;
        int viewCount;
        int purchaseCount;

        if (parts.size() >= 10) {
            tags = deserializeStringList(parts[5]);
            price = stod(parts[6]);
            stock = stoi(parts[7]);
            viewCount = stoi(parts[8]);
            purchaseCount = stoi(parts[9]);
        } else {
            price = stod(parts[5]);
            stock = stoi(parts[6]);
            viewCount = stoi(parts[7]);
            purchaseCount = stoi(parts[8]);
        }

        Product product(
            stoi(parts[0]),
            parts[1],
            category,
            devices,
            tags,
            price,
            stock
        );
        product.setViewCount(viewCount);
        product.setPurchaseCount(purchaseCount);
        products.push_back(product);
    }

    return products;
}

void FileManager::saveCustomers(const vector<Customer>& customers, const string& filePath) const {
    ensureParentDirectory(filePath);
    ofstream outFile(filePath);
    for (const auto& customer : customers) {
        outFile << customer.getUsername() << "|"
                << serializeMap(customer.getInteractionHistory().getViewedProducts()) << "|"
                << serializeMap(customer.getInteractionHistory().getPurchasedProducts()) << "|"
                << serializeIntVector(customer.getOrderHistory()) << "\n";
    }
}

vector<Customer> FileManager::loadCustomers(const string& filePath) const {
    ifstream inFile(filePath);
    vector<Customer> customers;
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> parts = splitString(line, '|');
        if (parts.size() < 3) {
            continue;
        }

        string username;
        map<int, int> viewed;
        map<int, int> purchased;
        vector<int> orderIds;

        if (parts.size() >= 5) {
            username = parts[1];
            viewed = deserializeMap(parts[2]);
            purchased = deserializeMap(parts[3]);
            orderIds = deserializeIntVector(parts[4]);
        } else {
            username = parts[0];
            viewed = deserializeMap(parts[1]);
            purchased = deserializeMap(parts[2]);
            if (parts.size() >= 4) {
                orderIds = deserializeIntVector(parts[3]);
            }
        }

        if (username.empty() && !parts.empty()) {
            username = parts[0];
        }

        Customer customer(username);
        for (const auto& entry : viewed) {
            for (int i = 0; i < entry.second; ++i) {
                customer.getInteractionHistory().addView(entry.first);
            }
        }

        for (const auto& entry : purchased) {
            customer.getInteractionHistory().addPurchase(entry.first, entry.second);
        }

        for (int orderId : orderIds) {
            customer.getOrderHistory().push_back(orderId);
        }

        customers.push_back(customer);
    }

    return customers;
}

void FileManager::saveAdmins(const vector<Admin>& admins, const string& filePath) const {
    ensureParentDirectory(filePath);
    ofstream outFile(filePath);
    for (const auto& admin : admins) {
        outFile << admin.getUsername() << "\n";
    }
}

vector<Admin> FileManager::loadAdmins(const string& filePath) const {
    ifstream inFile(filePath);
    vector<Admin> admins;
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> parts = splitString(line, '|');
        if (parts.empty()) {
            continue;
        }

        if (parts.size() >= 2 && !parts[1].empty()) {
            admins.push_back(Admin(parts[1]));
        } else if (!parts[0].empty()) {
            admins.push_back(Admin(parts[0]));
        }
    }

    return admins;
}

void FileManager::saveOrders(const vector<Order>& orders, const string& filePath) const {
    ensureParentDirectory(filePath);
    ofstream outFile(filePath);
    for (const auto& order : orders) {
        outFile << order.getOrderId() << "|"
                << order.getCustomerUsername() << "|"
                << order.getTotalAmount() << "|";

        const auto& items = order.getItems();
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& item = items[i];
            const auto& product = item.getProduct();
            if (i > 0) {
                outFile << ";";
            }
            outFile << product.getId() << "~"
                    << product.getName() << "~"
                    << static_cast<int>(product.getCategory()) << "~"
                    << categoryToString(product.getCategory()) << "~"
                    << item.getQuantity() << "~"
                    << item.getUnitPrice();
        }
        outFile << "\n";
    }
}

vector<Order> FileManager::loadOrders(const string& filePath) const {
    ifstream inFile(filePath);
    vector<Order> orders;
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> parts = splitString(line, '|');
        if (parts.size() < 4) {
            continue;
        }

        Order order(stoi(parts[0]), parts[1]);
        vector<string> itemParts = splitString(parts[3], ';');

        for (const auto& itemPart : itemParts) {
            if (itemPart.empty()) {
                continue;
            }

            vector<string> fields = splitString(itemPart, '~');
            if (fields.size() < 6) {
                continue;
            }

            Category category = intToCategory(stoi(fields[2]));
            Product product(
                stoi(fields[0]),
                fields[1],
                category,
                {},
                {},
                stod(fields[5]),
                0
            );
            order.addItem(OrderItem(product, stoi(fields[4]), stod(fields[5])));
        }

        order.calculateTotal();
        orders.push_back(order);
    }

    return orders;
}
