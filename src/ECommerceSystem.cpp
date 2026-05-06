#include "ECommerceSystem.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

namespace {
const string DATA_DIRECTORY = "data";
const string PRODUCTS_FILE = DATA_DIRECTORY + "/products.txt";
const string CUSTOMERS_FILE = DATA_DIRECTORY + "/customers.txt";
const string ADMINS_FILE = DATA_DIRECTORY + "/admins.txt";
const string ORDERS_FILE = DATA_DIRECTORY + "/orders.txt";

vector<string> splitCompatibleDevices(const string& input) {
    vector<string> devices;
    stringstream stream(input);
    string item;

    while (getline(stream, item, ',')) {
        size_t start = item.find_first_not_of(' ');
        size_t end = item.find_last_not_of(' ');

        if (start != string::npos && end != string::npos) {
            devices.push_back(item.substr(start, end - start + 1));
        }
    }

    return devices;
}

void printDivider() {
    cout << "----------------------------------------\n";
}

void printSectionTitle(const string& title) {
    printDivider();
    cout << title << "\n";
    printDivider();
}

void waitForEnter() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string normalizeText(const string& input) {
    string normalized;
    bool previousWasSpace = false;

    for (char ch : input) {
        unsigned char uch = static_cast<unsigned char>(ch);
        if (isspace(uch) || ispunct(uch)) {
            if (!normalized.empty() && !previousWasSpace) {
                normalized.push_back(' ');
                previousWasSpace = true;
            }
        } else {
            normalized.push_back(static_cast<char>(tolower(uch)));
            previousWasSpace = false;
        }
    }

    if (!normalized.empty() && normalized.back() == ' ') {
        normalized.pop_back();
    }

    return normalized;
}

vector<string> tokenizeSearchText(const string& input) {
    vector<string> tokens;
    stringstream stream(normalizeText(input));
    string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

string buildProductSearchText(const Product& product) {
    string searchText = product.getName() + " " + categoryToString(product.getCategory());

    for (const auto& tag : product.getTags()) {
        searchText += " " + tag;
    }

    for (const auto& device : product.getCompatibleDevices()) {
        searchText += " " + device;
    }

    return normalizeText(searchText);
}

void displayCartWithItemNumbers(const ShoppingCart& cart, const string& title) {
    printSectionTitle(title);

    const vector<CartItem>& items = cart.getItems();
    if (items.empty()) {
        cout << "Cart is empty.\n";
        return;
    }

    for (size_t i = 0; i < items.size(); ++i) {
        const Product& product = items[i].getProduct();
        cout << i + 1 << ". "
             << product.getName()
             << " | Qty: " << items[i].getQuantity()
             << " | Price: " << fixed << setprecision(0) << product.getPrice()
             << " | Subtotal: " << fixed << setprecision(0) << items[i].getSubtotal()
             << "\n";
    }

    cout << "Total: " << fixed << setprecision(0) << cart.calculateTotal() << "\n";
}
}

ECommerceSystem::ECommerceSystem() : products(),
                                     customers(),
                                     admins(),
                                     orders(),
                                     recommendationEngine(),
                                     statisticsManager(),
                                     fileManager(),
                                     nextProductId(1),
                                     nextOrderId(1) {}

bool ECommerceSystem::isUsernameTaken(const string& username) const {
    for (const auto& customer : customers) {
        if (customer.getUsername() == username) {
            return true;
        }
    }

    for (const auto& admin : admins) {
        if (admin.getUsername() == username) {
            return true;
        }
    }

    return false;
}

void ECommerceSystem::run() {
    loadData();
    seedData();
    saveData();
    cout << "Smart E-Commerce System started.\n";

    int choice = -1;
    while (choice != 0) {
        showMainMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
        case 1:
            if (customers.empty()) {
                cout << "No customer account available.\n";
            } else {
                printDivider();
                customerMenu(customers[0]);
            }
            break;
        case 2:
            if (admins.empty()) {
                cout << "No admin account available.\n";
            } else {
                printDivider();
                adminMenu(admins[0]);
            }
            break;
        case 0:
            saveData();
            cout << "Exiting system.\n";
            break;
        default:
            cout << "Invalid choice.\n";
            break;
        }
    }
}

void ECommerceSystem::seedData() {
    if (products.empty()) {
        products.push_back(Product(
            generateProductId(),
            "iPhone 15 Clear Case",
            Category::iPhone,
            {"iPhone 15"},
            {"case", "clear", "protection"},
            250000,
            20
        ));
        products.push_back(Product(
            generateProductId(),
            "iPhone 15 Pro Silicone Case",
            Category::iPhone,
            {"iPhone 15 Pro"},
            {"case", "silicone", "protection"},
            320000,
            15
        ));
        products.push_back(Product(
            generateProductId(),
            "iPad Air Folio Case",
            Category::iPad,
            {"iPad Air"},
            {"case", "folio", "protection"},
            450000,
            10
        ));
        products.push_back(Product(
            generateProductId(),
            "iPad Paperlike Screen Protector",
            Category::Protection,
            {"iPad Air", "iPad Pro"},
            {"paperlike", "screen", "protection", "writing"},
            290000,
            18
        ));
        products.push_back(Product(
            generateProductId(),
            "Apple Pencil Tips Pack",
            Category::iPad,
            {"iPad", "Apple Pencil"},
            {"pencil", "drawing", "writing"},
            190000,
            25
        ));
        products.push_back(Product(
            generateProductId(),
            "AirPods Pro Protective Case",
            Category::AirPods,
            {"AirPods Pro"},
            {"case", "audio", "protection"},
            210000,
            12
        ));
        products.push_back(Product(
            generateProductId(),
            "20W USB-C Charger",
            Category::Charging,
            {"iPhone", "iPad", "AirPods"},
            {"charger", "usb-c", "fast-charge"},
            490000,
            14
        ));
        products.push_back(Product(
            generateProductId(),
            "USB-C to USB-C Cable 1m",
            Category::Charging,
            {"iPhone", "iPad", "AirPods"},
            {"cable", "usb-c", "charging"},
            180000,
            30
        ));
        products.push_back(Product(
            generateProductId(),
            "MagSafe Power Bank",
            Category::Charging,
            {"iPhone"},
            {"power-bank", "magsafe", "charging"},
            890000,
            8
        ));
        products.push_back(Product(
            generateProductId(),
            "Tempered Glass for iPhone 15",
            Category::Protection,
            {"iPhone 15"},
            {"tempered-glass", "screen", "protection"},
            150000,
            22
        ));
        products.push_back(Product(
            generateProductId(),
            "Tempered Glass for iPad Air",
            Category::Protection,
            {"iPad Air"},
            {"tempered-glass", "screen", "protection"},
            240000,
            16
        ));
        products.push_back(Product(
            generateProductId(),
            "Laptop Sleeve 13 inch",
            Category::Universal,
            {"Laptop", "iPad"},
            {"sleeve", "bag", "protection"},
            390000,
            9
        ));

        products[0].increaseViewCount();
        products[0].increaseViewCount();
        products[1].increaseViewCount();
        products[3].increaseViewCount();
        products[3].increaseViewCount();
        products[3].increaseViewCount();
        products[6].increaseViewCount();
        products[6].increaseViewCount();
        products[8].increaseViewCount();

        products[0].increasePurchaseCount(2);
        products[3].increasePurchaseCount(1);
        products[6].increasePurchaseCount(3);
        products[8].increasePurchaseCount(2);
    }
    else {
        auto applyTagsById = [this](int productId, const vector<string>& newTags) {
            Product* product = findProductById(productId);
            if (product != nullptr && product->getTags().empty()) {
                product->setTags(newTags);
            }
        };

        applyTagsById(1, {"case", "clear", "protection"});
        applyTagsById(2, {"case", "silicone", "protection"});
        applyTagsById(3, {"case", "folio", "protection"});
        applyTagsById(4, {"paperlike", "screen", "protection", "writing"});
        applyTagsById(5, {"pencil", "drawing", "writing"});
        applyTagsById(6, {"case", "audio", "protection"});
        applyTagsById(7, {"charger", "usb-c", "fast-charge"});
        applyTagsById(8, {"cable", "usb-c", "charging"});
        applyTagsById(9, {"power-bank", "magsafe", "charging"});
        applyTagsById(10, {"tempered-glass", "screen", "protection"});
        applyTagsById(11, {"tempered-glass", "screen", "protection"});
        applyTagsById(12, {"sleeve", "bag", "protection"});
    }

    if (customers.empty()) {
        if (!isUsernameTaken("user1")) {
            customers.push_back(Customer("user1"));
        }
        if (!isUsernameTaken("user2")) {
            customers.push_back(Customer("user2"));
        }

        if (!customers.empty()) {
            customers[0].getInteractionHistory().addView(products[0].getId());
            customers[0].getInteractionHistory().addView(products[0].getId());
            customers[0].getInteractionHistory().addView(products[6].getId());
            customers[0].getInteractionHistory().addPurchase(products[6].getId(), 1);
        }

        if (customers.size() > 1) {
            customers[1].getInteractionHistory().addView(products[3].getId());
            customers[1].getInteractionHistory().addView(products[4].getId());
            customers[1].getInteractionHistory().addPurchase(products[3].getId(), 1);
        }
    }

    if (admins.empty()) {
        if (!isUsernameTaken("admin1")) {
            admins.push_back(Admin("admin1"));
        }
    }
}

void ECommerceSystem::loadData() {
    products = fileManager.loadProducts(PRODUCTS_FILE);
    customers = fileManager.loadCustomers(CUSTOMERS_FILE);
    admins = fileManager.loadAdmins(ADMINS_FILE);
    orders = fileManager.loadOrders(ORDERS_FILE);

    vector<Customer> uniqueCustomers;
    for (const auto& customer : customers) {
        if (customer.getUsername().empty()) {
            continue;
        }

        Customer normalizedCustomer = customer;
        if (normalizedCustomer.getUsername() == "1") {
            normalizedCustomer.setUsername("user1");
        } else if (normalizedCustomer.getUsername() == "2") {
            normalizedCustomer.setUsername("user2");
        }

        bool exists = false;
        for (const auto& saved : uniqueCustomers) {
            if (saved.getUsername() == normalizedCustomer.getUsername()) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            uniqueCustomers.push_back(normalizedCustomer);
        }
    }
    customers = uniqueCustomers;

    vector<Admin> uniqueAdmins;
    for (const auto& admin : admins) {
        if (admin.getUsername().empty()) {
            continue;
        }

        bool exists = false;
        for (const auto& saved : uniqueAdmins) {
            if (saved.getUsername() == admin.getUsername()) {
                exists = true;
                break;
            }
        }
        bool usedByCustomer = false;
        for (const auto& customer : customers) {
            if (customer.getUsername() == admin.getUsername()) {
                usedByCustomer = true;
                break;
            }
        }

        if (!exists && !usedByCustomer) {
            uniqueAdmins.push_back(admin);
        }
    }
    admins = uniqueAdmins;

    nextProductId = 1;
    for (const auto& product : products) {
        if (product.getId() >= nextProductId) {
            nextProductId = product.getId() + 1;
        }
    }

    nextOrderId = 1;
    for (const auto& order : orders) {
        if (order.getOrderId() >= nextOrderId) {
            nextOrderId = order.getOrderId() + 1;
        }
    }
}

void ECommerceSystem::saveData() const {
    fileManager.saveProducts(products, PRODUCTS_FILE);
    fileManager.saveCustomers(customers, CUSTOMERS_FILE);
    fileManager.saveAdmins(admins, ADMINS_FILE);
    fileManager.saveOrders(orders, ORDERS_FILE);
}

void ECommerceSystem::showMainMenu() const {
    printSectionTitle("Main Menu");
    cout << "1. Customer\n"
         << "2. Admin\n"
         << "0. Exit\n";
}

void ECommerceSystem::viewAllProducts() const {
    if (products.empty()) {
        cout << "No products available.\n";
        return;
    }

    printSectionTitle("Product List");
    for (const auto& product : products) {
        product.display();
        cout << "------------------\n";
    }
}

void ECommerceSystem::customerMenu(Customer& customer) {
    int choice = -1;
    while (choice != 0) {
        customer.displayMenu();
        cout << "Hello, " << customer.getUsername() << ". Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            browseProducts(customer);
            printDivider();
            break;
        }
        case 2: {
            searchProducts();
            printDivider();
            break;
        }
        case 3: {
            printSectionTitle("Add To Cart");
            int productId;
            cout << "Enter product ID to add: ";
            cin >> productId;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid product ID.\n";
                printDivider();
                break;
            }

            Product* product = findProductById(productId);
            if (product == nullptr) {
                cout << "Product not found.\n";
                printDivider();
                break;
            }

            int quantity;
            cout << "Enter quantity: ";
            cin >> quantity;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid quantity.\n";
            } else {
                addToCart(customer, productId, quantity);
            }
            printDivider();
            break;
        }
        case 4: {
            int cartChoice = -1;
            while (cartChoice != 0) {
                displayCartWithItemNumbers(customer.getCart(), "Your Cart");

                if (customer.getCart().isEmpty()) {
                    cout << "0. Back\n"
                         << "Enter your choice: ";
                    cin >> cartChoice;

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid input.\n";
                        continue;
                    }

                    if (cartChoice != 0) {
                        cout << "Invalid choice.\n";
                    }
                    continue;
                }

                cout << "1. Checkout\n"
                     << "2. Update cart\n"
                     << "0. Back\n"
                     << "Enter your choice: ";
                cin >> cartChoice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input.\n";
                    continue;
                }

                if (cartChoice == 1) {
                    checkout(customer, false);
                    cartChoice = 0;
                } else if (cartChoice == 2) {
                    updateCartMenu(customer);
                } else if (cartChoice != 0) {
                    cout << "Invalid choice.\n";
                }
            }
            printDivider();
            break;
        }
        case 5: {
            updateCartMenu(customer);
            printDivider();
            break;
        }
        case 6: {
            checkout(customer, true);
            printDivider();
            break;
        }
        case 7: {
            viewOrderHistory(customer);
            printDivider();
            break;
        }
        case 8: {
            int topN;
            cout << "Enter number of recommendations: ";
            cin >> topN;
            if (cin.fail() || topN <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid number.\n";
            } else {
                showRecommendations(customer, topN);
            }
            printDivider();
            break;
        }
        case 0:
            cout << "Customer logged out.\n";
            printDivider();
            break;
        default:
            cout << "Invalid choice.\n";
            printDivider();
            break;
        }
    }
}

void ECommerceSystem::adminMenu(Admin& admin) {
    int choice = -1;
    while (choice != 0) {
        admin.displayMenu();
        cout << "Hello, " << admin.getUsername() << ". Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
        case 1:
            addProduct();
            printDivider();
            break;
        case 2: {
            int productId;
            cout << "Enter product ID to edit: ";
            cin >> productId;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid product ID.\n";
            } else {
                editProduct(productId);
            }
            printDivider();
            break;
        }
        case 3: {
            int productId;
            cout << "Enter product ID to delete: ";
            cin >> productId;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid product ID.\n";
            } else {
                deleteProduct(productId);
            }
            printDivider();
            break;
        }
        case 4:
            viewAllProducts();
            printDivider();
            break;
        case 5:
            showMostViewedProducts(5);
            printDivider();
            break;
        case 6:
            showBestSellingProducts(5);
            printDivider();
            break;
        case 7:
            showActiveUsers(5);
            printDivider();
            break;
        case 0:
            cout << "Admin logged out.\n";
            printDivider();
            break;
        default:
            cout << "Invalid choice.\n";
            printDivider();
            break;
        }
    }
}

void ECommerceSystem::searchProducts() const {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string keyword;
    cout << "Enter product name or keyword: ";
    getline(cin, keyword);

    if (keyword.empty()) {
        cout << "Keyword cannot be empty.\n";
        return;
    }

    vector<string> keywordTokens = tokenizeSearchText(keyword);
    if (keywordTokens.empty()) {
        cout << "Keyword cannot be empty.\n";
        return;
    }

    vector<const Product*> matchedProducts;
    for (const auto& product : products) {
        string productSearchText = buildProductSearchText(product);
        bool matchedAllTokens = true;

        for (const auto& token : keywordTokens) {
            if (productSearchText.find(token) == string::npos) {
                matchedAllTokens = false;
                break;
            }
        }

        if (matchedAllTokens) {
            matchedProducts.push_back(&product);
        }
    }

    if (matchedProducts.empty()) {
        cout << "No products matched your search.\n";
        return;
    }

    printSectionTitle("Search Results");
    for (const auto* product : matchedProducts) {
        product->displayForCustomer();
        cout << "------------------\n";
    }
}

void ECommerceSystem::browseProducts(Customer& customer) {
    int choice = -1;
    while (choice != 0) {
        printSectionTitle("Product List");
        for (const auto& product : products) {
            product.displayForCustomer();
            cout << "------------------\n";
        }

        cout << "1. View product detail\n";
        cout << "2. Add product to cart\n";
        cout << "0. Back\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        if (choice == 1) {
            int productId;
            cout << "Enter product ID to view: ";
            cin >> productId;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid product ID.\n";
                continue;
            }
            viewProductDetail(customer, productId);

            int detailChoice = -1;
            while (detailChoice != 0) {
                cout << "1. Add product to cart\n"
                     << "0. Back\n"
                     << "Enter your choice: ";
                cin >> detailChoice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input.\n";
                    continue;
                }

                if (detailChoice == 1) {
                    Product* product = findProductById(productId);
                    if (product == nullptr) {
                        cout << "Product not found.\n";
                        break;
                    }

                    int quantity;
                    cout << "Enter quantity: ";
                    cin >> quantity;

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid quantity.\n";
                        continue;
                    }

                    addToCart(customer, productId, quantity);
                    waitForEnter();
                    detailChoice = 0;
                } else if (detailChoice != 0) {
                    cout << "Invalid choice.\n";
                }
            }
        } else if (choice == 2) {
            int productId;
            cout << "Enter product ID to add: ";
            cin >> productId;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid product ID.\n";
                continue;
            }

            if (productId == 0) {
                cout << "Add to cart cancelled.\n";
                continue;
            }

            Product* product = findProductById(productId);
            if (product == nullptr) {
                cout << "Product not found.\n";
                continue;
            }

            int quantity;
            cout << "Enter quantity: ";
            cin >> quantity;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid quantity.\n";
                continue;
            }

            addToCart(customer, productId, quantity);
            waitForEnter();
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    }
}

void ECommerceSystem::updateCartMenu(Customer& customer) {
    bool backToCustomerMenu = false;
    while (!backToCustomerMenu) {
        displayCartWithItemNumbers(customer.getCart(), "Current Cart");

        if (customer.getCart().isEmpty()) {
            return;
        }

        string selection;
        cout << "C. Clear cart\n"
             << "0. Back\n"
             << "Enter item number to manage: ";
        cin >> selection;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        if (selection == "0") {
            break;
        }

        if (selection == "c" || selection == "C") {
            string confirm;
            cout << "Are you sure you want to clear the cart? (yes/no): ";
            cin >> confirm;
            if (confirm == "yes" || confirm == "YES" || confirm == "y" || confirm == "Y") {
                customer.getCart().clear();
                cout << "Cart cleared successfully.\n";
                return;
            }
            cout << "Clear cart cancelled.\n";
            continue;
        }

        int selectedItemNumber;
        stringstream selectionStream(selection);
        selectionStream >> selectedItemNumber;
        if (selectionStream.fail() || !selectionStream.eof()) {
            cout << "Invalid input.\n";
            continue;
        }

        if (selectedItemNumber == 0) {
            break;
        }

        if (selectedItemNumber < 0 ||
            selectedItemNumber > static_cast<int>(customer.getCart().getItems().size())) {
            cout << "Item number is invalid.\n";
            continue;
        }

        const CartItem& selectedItem = customer.getCart().getItems()[selectedItemNumber - 1];
        const int productId = selectedItem.getProduct().getId();
        Product* product = findProductById(productId);
        if (product == nullptr) {
            cout << "This product no longer exists in the product list.\n";
            continue;
        }

        int action = -1;
        while (action != 0) {
            printSectionTitle("Selected Cart Item");
            cout << selectedItemNumber << ". "
                 << product->getName()
                 << " | Current quantity: " << selectedItem.getQuantity()
                 << " | Stock: " << product->getStock()
                 << " | Subtotal: " << fixed << setprecision(0) << selectedItem.getSubtotal()
                 << "\n";

            cout << "\nUpdate Item Menu\n"
                 << "1. Set quantity\n"
                 << "2. Remove product\n"
                 << "0. Back\n"
                 << "Enter your choice: ";
            cin >> action;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input.\n";
                continue;
            }

            if (action == 1) {
                int quantity;
                cout << "Enter new quantity: ";
                cin >> quantity;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid quantity.\n";
                    continue;
                }

                if (quantity < 0) {
                    cout << "Quantity cannot be negative.\n";
                    continue;
                }

                if (quantity > product->getStock()) {
                    cout << "Quantity exceeds stock.\n";
                    continue;
                }

                customer.getCart().updateQuantity(productId, quantity);
                if (quantity == 0) {
                    cout << "Product removed from cart.\n";
                } else {
                    cout << "Cart updated successfully.\n";
                }
                cout << "Updated total: " << fixed << setprecision(0)
                     << customer.getCart().calculateTotal() << "\n";
                action = 0;
            } else if (action == 2) {
                customer.getCart().removeProduct(productId);
                cout << "Product removed from cart.\n";
                cout << "Updated total: " << fixed << setprecision(0)
                     << customer.getCart().calculateTotal() << "\n";
                action = 0;
            } else if (action != 0) {
                cout << "Invalid choice.\n";
            }
        }
    }
}

void ECommerceSystem::viewProductDetail(Customer& customer, int productId) {
    Product* product = findProductById(productId);
    if (product == nullptr) {
        cout << "Product not found.\n";
        return;
    }

    product->increaseViewCount();
    customer.getInteractionHistory().addView(productId);
    printSectionTitle("Product Detail");
    product->displayForCustomer();
}

void ECommerceSystem::addToCart(Customer& customer, int productId, int quantity) {
    Product* product = findProductById(productId);
    if (product == nullptr) {
        cout << "Product not found.\n";
        return;
    }

    if (quantity <= 0 || quantity > product->getStock()) {
        cout << "Invalid quantity.\n";
        return;
    }

    customer.getCart().addProduct(*product, quantity);
    cout << "Added " << quantity << " x " << product->getName()
         << " to cart successfully.\n";
}

void ECommerceSystem::removeFromCart(Customer& customer) {
    if (customer.getCart().isEmpty()) {
        cout << "Cart is empty.\n";
        return;
    }

    int productId;
    cout << "Enter product ID to remove from cart: ";
    cin >> productId;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid product ID.\n";
        return;
    }

    if (productId <= 0) {
        cout << "Product ID is invalid.\n";
        return;
    }

    bool foundInCart = false;
    for (const auto& item : customer.getCart().getItems()) {
        if (item.getProduct().getId() == productId) {
            foundInCart = true;
            break;
        }
    }

    if (!foundInCart) {
        cout << "Product is not in the cart yet.\n";
        return;
    }

    customer.getCart().removeProduct(productId);
    cout << "Cart updated. Current total: " << fixed << setprecision(0)
         << customer.getCart().calculateTotal() << "\n";
}

void ECommerceSystem::checkout(Customer& customer, bool requireConfirmation) {
    if (customer.getCart().isEmpty()) {
        cout << "Cart is empty.\n";
        return;
    }

    if (requireConfirmation) {
        displayCartWithItemNumbers(customer.getCart(), "Checkout Cart");

        string confirm;
        cout << "Proceed to checkout? (yes/no): ";
        cin >> confirm;

        if (confirm != "yes" && confirm != "YES" && confirm != "y" && confirm != "Y") {
            cout << "Checkout cancelled.\n";
            return;
        }
    }

    Order order(generateOrderId(), customer.getUsername());
    for (const auto& item : customer.getCart().getItems()) {
        Product* product = findProductById(item.getProduct().getId());
        if (product == nullptr || item.getQuantity() > product->getStock()) {
            cout << "Checkout failed because stock is invalid.\n";
            return;
        }
    }

    for (const auto& item : customer.getCart().getItems()) {
        Product* product = findProductById(item.getProduct().getId());
        product->decreaseStock(item.getQuantity());
        product->increasePurchaseCount(item.getQuantity());
        customer.getInteractionHistory().addPurchase(product->getId(), item.getQuantity());
        order.addItem(OrderItem(*product, item.getQuantity(), product->getPrice()));
    }

    order.calculateTotal();
    orders.push_back(order);
    customer.addOrderId(order.getOrderId());
    customer.getCart().clear();
    saveData();

    printSectionTitle("Order Summary");
    order.display();
    printDivider();
    cout << "Checkout successful.\n";
}

void ECommerceSystem::viewOrderHistory(const Customer& customer) const {
    printSectionTitle("Order History");
    int orderCount = 0;
    for (const auto& order : orders) {
        if (order.getCustomerUsername() == customer.getUsername()) {
            ++orderCount;
        }
    }

    cout << "Total orders purchased: " << orderCount << "\n";
    bool hasOrders = false;
    for (const auto& order : orders) {
        if (order.getCustomerUsername() == customer.getUsername()) {
            order.display();
            cout << "------------------\n";
            hasOrders = true;
        }
    }

    if (!hasOrders) {
        cout << "No order history found.\n";
    }
}

void ECommerceSystem::showRecommendations(const Customer& customer, int topN) const {
    vector<Product> recommendations =
        recommendationEngine.recommendProducts(customer, products, topN);

    if (recommendations.empty()) {
        cout << "No recommendations available.\n";
        return;
    }

    printSectionTitle("Top Recommendations");
    cout << "Showing top " << topN << " products\n";
    for (const auto& product : recommendations) {
        product.displayForCustomer();
        cout << "------------------\n";
    }
}

void ECommerceSystem::addProduct() {
    string name;
    int categoryId;
    string devicesInput;
    string tagsInput;
    double price;
    int stock;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter product name: ";
    getline(cin, name);

    cout << "Available categories:\n";
    for (const auto& category : getAllCategories()) {
        cout << static_cast<int>(category) << ". " << categoryToString(category) << "\n";
    }

    cout << "Enter category ID: ";
    cin >> categoryId;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid category ID.\n";
        return;
    }

    if (categoryId < 1 || categoryId > 6) {
        cout << "Category not found.\n";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter compatible devices (comma-separated): ";
    getline(cin, devicesInput);

    cout << "Enter tags (comma-separated): ";
    getline(cin, tagsInput);

    cout << "Enter price: ";
    cin >> price;
    if (cin.fail() || price < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid price.\n";
        return;
    }

    cout << "Enter stock: ";
    cin >> stock;
    if (cin.fail() || stock < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid stock.\n";
        return;
    }

    products.push_back(Product(
        generateProductId(),
        name,
        intToCategory(categoryId),
        splitCompatibleDevices(devicesInput),
        splitCompatibleDevices(tagsInput),
        price,
        stock
    ));

    saveData();
    cout << "Product added successfully.\n";
}

void ECommerceSystem::editProduct(int productId) {
    Product* product = findProductById(productId);
    if (product == nullptr) {
        cout << "Product not found.\n";
        return;
    }

    printSectionTitle("Editing Product");
    product->display();

    int choice = -1;
    printSectionTitle("Edit Product Menu");
    cout << "1. Edit name\n"
         << "2. Edit category\n"
         << "3. Edit compatible devices\n"
         << "4. Edit tags\n"
         << "5. Edit price\n"
         << "6. Edit stock\n"
         << "7. Edit all fields\n"
         << "0. Cancel\n"
         << "Enter your choice: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid choice.\n";
        return;
    }

    if (choice == 0) {
        cout << "Edit cancelled.\n";
        return;
    }

    if (choice < 0 || choice > 7) {
        cout << "Invalid choice.\n";
        return;
    }

    bool updated = false;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        string name;
        cout << "Enter new product name, or 0 to cancel: ";
        getline(cin, name);
        if (name == "0") {
            cout << "Edit name cancelled.\n";
            return;
        }
        if (name.empty()) {
            cout << "Product name cannot be empty.\n";
            return;
        }
        product->setName(name);
        updated = true;
    }

    if (choice == 2) {
        int categoryId;
        cout << "Available categories:\n";
        for (const auto& category : getAllCategories()) {
            cout << static_cast<int>(category) << ". " << categoryToString(category) << "\n";
        }
        cout << "Enter new category ID, or 0 to cancel: ";
        cin >> categoryId;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid category ID.\n";
            return;
        }
        if (categoryId == 0) {
            cout << "Edit category cancelled.\n";
            return;
        }
        if (categoryId < 1 || categoryId > 6) {
            cout << "Category not found.\n";
            return;
        }
        product->setCategory(intToCategory(categoryId));
        updated = true;
    }

    if (choice == 3) {
        string devicesInput;
        cout << "Enter new compatible devices (comma-separated), or 0 to cancel: ";
        getline(cin, devicesInput);
        if (devicesInput == "0") {
            cout << "Edit compatible devices cancelled.\n";
            return;
        }
        if (devicesInput.empty()) {
            cout << "Compatible devices cannot be empty.\n";
            return;
        }
        product->setCompatibleDevices(splitCompatibleDevices(devicesInput));
        updated = true;
    }

    if (choice == 4) {
        string tagsInput;
        cout << "Enter new tags (comma-separated), or 0 to cancel: ";
        getline(cin, tagsInput);
        if (tagsInput == "0") {
            cout << "Edit tags cancelled.\n";
            return;
        }
        if (tagsInput.empty()) {
            cout << "Tags cannot be empty.\n";
            return;
        }
        product->setTags(splitCompatibleDevices(tagsInput));
        updated = true;
    }

    if (choice == 5) {
        double price;
        cout << "Enter new price, or -1 to cancel: ";
        cin >> price;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid price.\n";
            return;
        }
        if (price == -1) {
            cout << "Edit price cancelled.\n";
            return;
        }
        if (price < 0) {
            cout << "Invalid price.\n";
            return;
        }
        product->setPrice(price);
        updated = true;
    }

    if (choice == 6) {
        int stock;
        cout << "Enter new stock, or -1 to cancel: ";
        cin >> stock;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid stock.\n";
            return;
        }
        if (stock == -1) {
            cout << "Edit stock cancelled.\n";
            return;
        }
        if (stock < 0) {
            cout << "Invalid stock.\n";
            return;
        }
        product->setStock(stock);
        updated = true;
    }

    if (choice == 7) {
        string name;
        int categoryId;
        string devicesInput;
        string tagsInput;
        double price;
        int stock;

        cout << "Enter new product name, or 0 to cancel: ";
        getline(cin, name);
        if (name == "0") {
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (name.empty()) {
            cout << "Product name cannot be empty.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }

        cout << "Available categories:\n";
        for (const auto& category : getAllCategories()) {
            cout << static_cast<int>(category) << ". " << categoryToString(category) << "\n";
        }
        cout << "Enter new category ID, or 0 to cancel: ";
        cin >> categoryId;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid category ID.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (categoryId == 0) {
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (categoryId < 1 || categoryId > 6) {
            cout << "Category not found.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter new compatible devices (comma-separated), or 0 to cancel: ";
        getline(cin, devicesInput);
        if (devicesInput == "0") {
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (devicesInput.empty()) {
            cout << "Compatible devices cannot be empty.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }

        cout << "Enter new tags (comma-separated), or 0 to cancel: ";
        getline(cin, tagsInput);
        if (tagsInput == "0") {
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (tagsInput.empty()) {
            cout << "Tags cannot be empty.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }

        cout << "Enter new price, or -1 to cancel: ";
        cin >> price;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid price.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (price == -1) {
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (price < 0) {
            cout << "Invalid price.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }

        cout << "Enter new stock, or -1 to cancel: ";
        cin >> stock;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid stock.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (stock == -1) {
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }
        if (stock < 0) {
            cout << "Invalid stock.\n";
            cout << "Edit all cancelled. No changes were saved.\n";
            return;
        }

        product->setName(name);
        product->setCategory(intToCategory(categoryId));
        product->setCompatibleDevices(splitCompatibleDevices(devicesInput));
        product->setTags(splitCompatibleDevices(tagsInput));
        product->setPrice(price);
        product->setStock(stock);
        updated = true;
    }

    if (!updated) {
        cout << "No changes were saved.\n";
        return;
    }

    saveData();
    cout << "Product updated successfully.\n";
    product->displayForAdmin();
}

void ECommerceSystem::deleteProduct(int productId) {
    Product* product = findProductById(productId);
    if (product == nullptr) {
        cout << "Product not found.\n";
        return;
    }

    cout << "Product selected for deletion:\n";
    product->display();

    string confirm;
    cout << "Are you sure you want to delete this product? (yes/no): ";
    cin >> confirm;

    if (confirm != "yes" && confirm != "YES" && confirm != "y" && confirm != "Y") {
        cout << "Delete product cancelled.\n";
        return;
    }

    auto it = remove_if(
        products.begin(),
        products.end(),
        [productId](const Product& currentProduct) {
            return currentProduct.getId() == productId;
        }
    );

    products.erase(it, products.end());
    saveData();
    cout << "Product deleted successfully.\n";
}

void ECommerceSystem::showStatistics(int topN) const {
    vector<Product> mostViewed = statisticsManager.getMostViewedProducts(products, topN);
    vector<Product> bestSelling = statisticsManager.getBestSellingProducts(products, topN);
    vector<Customer> activeUsers = statisticsManager.getMostActiveUsers(customers, topN);

    printSectionTitle("System Statistics");

    printSectionTitle("Most Viewed Products");
    cout << "Showing top " << topN << " products\n";
    if (mostViewed.empty()) {
        cout << "No product data available.\n";
    } else {
        for (size_t i = 0; i < mostViewed.size(); ++i) {
            cout << i + 1 << ". "
                 << mostViewed[i].getName()
                 << " | Views: " << mostViewed[i].getViewCount()
                 << " | Stock: " << mostViewed[i].getStock()
                 << "\n";
        }
    }

    printSectionTitle("Best-Selling Products");
    cout << "Showing top " << topN << " products\n";
    if (bestSelling.empty()) {
        cout << "No sales data available.\n";
    } else {
        for (size_t i = 0; i < bestSelling.size(); ++i) {
            cout << i + 1 << ". "
                 << bestSelling[i].getName()
                 << " | Purchases: " << bestSelling[i].getPurchaseCount()
                 << " | Price: " << fixed << setprecision(0) << bestSelling[i].getPrice()
                 << "\n";
        }
    }

    printSectionTitle("Active Users");
    cout << "Showing top " << topN << " users\n";
    if (activeUsers.empty()) {
        cout << "No user activity available.\n";
    } else {
        for (size_t i = 0; i < activeUsers.size(); ++i) {
            cout << i + 1 << ". "
                 << activeUsers[i].getUsername()
                 << " | Activity Score: " << activeUsers[i].getActivityScore()
                 << " | Orders: " << activeUsers[i].getOrderHistory().size()
                 << "\n";
        }
    }

    cout << "\nTotal Orders: " << orders.size() << "\n";
    cout << "Total Revenue: " << fixed << setprecision(0)
         << statisticsManager.calculateRevenue(orders) << "\n";
    cout << "=============================\n";
}

void ECommerceSystem::showMostViewedProducts(int topN) const {
    vector<Product> mostViewed = statisticsManager.getMostViewedProducts(products, topN);
    cout << "\nTop " << topN << " Most Viewed Products\n";
    if (mostViewed.empty()) {
        cout << "No product data available.\n";
        return;
    }

    for (size_t i = 0; i < mostViewed.size(); ++i) {
        cout << i + 1 << ". "
             << "ID: " << mostViewed[i].getId() << " | "
             << mostViewed[i].getName()
             << " | Views: " << mostViewed[i].getViewCount()
             << " | Stock: " << mostViewed[i].getStock()
             << "\n";
    }
}

void ECommerceSystem::showBestSellingProducts(int topN) const {
    vector<Product> bestSelling = statisticsManager.getBestSellingProducts(products, topN);
    cout << "\nTop " << topN << " Best-Selling Products\n";
    if (bestSelling.empty()) {
        cout << "No sales data available.\n";
        return;
    }

    for (size_t i = 0; i < bestSelling.size(); ++i) {
        cout << i + 1 << ". "
             << "ID: " << bestSelling[i].getId() << " | "
             << bestSelling[i].getName()
             << " | Purchases: " << bestSelling[i].getPurchaseCount()
             << " | Price: " << fixed << setprecision(0) << bestSelling[i].getPrice()
             << "\n";
    }
}

void ECommerceSystem::showActiveUsers(int topN) const {
    vector<Customer> activeUsers = statisticsManager.getMostActiveUsers(customers, topN);
    cout << "\nTop " << topN << " Active Users\n";
    if (activeUsers.empty()) {
        cout << "No user activity available.\n";
        return;
    }

    for (size_t i = 0; i < activeUsers.size(); ++i) {
        cout << i + 1 << ". "
             << activeUsers[i].getUsername()
             << " | Activity Score: " << activeUsers[i].getActivityScore()
             << " | Orders: " << activeUsers[i].getOrderHistory().size()
             << "\n";
    }
}

Product* ECommerceSystem::findProductById(int productId) {
    for (auto& product : products) {
        if (product.getId() == productId) {
            return &product;
        }
    }
    return nullptr;
}

const Product* ECommerceSystem::findProductById(int productId) const {
    for (const auto& product : products) {
        if (product.getId() == productId) {
            return &product;
        }
    }
    return nullptr;
}

Customer* ECommerceSystem::findCustomerByUsername(const string& username) {
    for (auto& customer : customers) {
        if (customer.getUsername() == username) {
            return &customer;
        }
    }
    return nullptr;
}

Admin* ECommerceSystem::findAdminByUsername(const string& username) {
    for (auto& admin : admins) {
        if (admin.getUsername() == username) {
            return &admin;
        }
    }
    return nullptr;
}

int ECommerceSystem::generateProductId() {
    return nextProductId++;
}

int ECommerceSystem::generateOrderId() {
    return nextOrderId++;
}
