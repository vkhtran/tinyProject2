#include "ECommerceSystem.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

namespace {
const int DIVIDER_WIDTH = 60;
const string MENU_INDENT = "  ";
const string DATA_DIRECTORY = "data";
const string PRODUCTS_FILE = DATA_DIRECTORY + "/products.txt";
const string CUSTOMERS_FILE = DATA_DIRECTORY + "/customers.txt";
const string ADMINS_FILE = DATA_DIRECTORY + "/admins.txt";
const string ORDERS_FILE = DATA_DIRECTORY + "/orders.txt";
const string USERS_FILE = DATA_DIRECTORY + "/users.txt";

vector<string> splitCommaSeparatedValues(const string& input) {
    vector<string> values;
    stringstream stream(input);
    string item;

    while (getline(stream, item, ',')) {
        size_t start = item.find_first_not_of(' ');
        size_t end = item.find_last_not_of(' ');

        if (start != string::npos && end != string::npos) {
            values.push_back(item.substr(start, end - start + 1));
        }
    }

    return values;
}

void printDivider() {
    cout << string(DIVIDER_WIDTH, '=') << "\n";
}

void printMinorDivider() {
    cout << string(DIVIDER_WIDTH, '-') << "\n";
}

void printSectionTitle(const string& title) {
    printDivider();
    cout << "\033[36m" << title << "\033[0m\n";
    printDivider();
}

void clearScreen() {
    system("cls");
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printSuccess(const string& message) {
    cout << "\033[32m" << message << "\033[0m\n";
}

void printDanger(const string& message) {
    cout << "\033[31m" << message << "\033[0m\n";
}

bool isYesAnswer(const string& answer) {
    return answer == "yes" || answer == "YES" || answer == "y" || answer == "Y";
}

bool confirmAction(const string& message) {
    string confirm;
    cout << message << " (Y/N): ";
    cin >> confirm;
    return isYesAnswer(confirm);
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

string normalizeUsername(const string& input) {
    string normalized;
    for (char ch : input) {
        normalized.push_back(static_cast<char>(tolower(static_cast<unsigned char>(ch))));
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

void displayHeader(const Customer& customer) {
    cout << "[User: " << customer.getUsername()
         << " | Cart: " << customer.getCart().getItemCount() << " items]"
         << "\n";
    printMinorDivider();
}

void displayHeader(const Admin& admin) {
    cout << "[Admin: " << admin.getUsername() << "]\n";
    printMinorDivider();
}

void displayBreadcrumb(const string& breadcrumb) {
    cout << "\033[90m" << breadcrumb << "\033[0m\n";
    printMinorDivider();
}

void displayProductTable(const vector<const Product*>& productList) {
    cout << left
         << setw(5) << "ID"
         << setw(34) << "Product Name"
         << right
         << setw(12) << "Price"
         << setw(8) << "Sold"
         << setw(12) << "Stock"
         << "\n";

    cout << string(71, '-') << "\n";

    for (const auto* product : productList) {
        string stockDisplay = product->getStock() == 0
            ? "Sold out"
            : to_string(product->getStock());

        cout << left
             << setw(5) << product->getId()
             << setw(34) << product->getName().substr(0, 32)
             << right
             << setw(12) << fixed << setprecision(0) << product->getPrice()
             << setw(8) << product->getPurchaseCount()
             << setw(12) << stockDisplay
             << "\n";
    }

    cout << string(71, '-') << "\n";
}

bool promptQuantityWithStockLimit(const string& prompt, int maxQuantity, int& quantity) {
    string currentPrompt = prompt;
    while (true) {
        cout << currentPrompt;
        cin >> quantity;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            currentPrompt = "Invalid quantity. Enter quantity again or enter 0 to cancel: ";
            continue;
        }

        if (quantity == 0) {
            return false;
        }

        if (quantity < 0) {
            currentPrompt = "Quantity cannot be negative. Enter quantity again or enter 0 to cancel: ";
            continue;
        }

        if (quantity > maxQuantity) {
            currentPrompt = "Only " + to_string(maxQuantity)
                + " items available in stock. Enter quantity again or enter 0 to cancel: ";
            continue;
        }

        return true;
    }
}

}

ECommerceSystem::ECommerceSystem() : products(),
                                     customers(),
                                     admins(),
                                     orders(),
                                     authManager(),
                                     recommendationEngine(),
                                     statisticsManager(),
                                     fileManager(),
                                     nextProductId(1),
                                     nextOrderId(1) {}

bool ECommerceSystem::isUsernameTaken(const string& username) const {
    string normalizedUsername = normalizeUsername(username);
    for (const auto& customer : customers) {
        if (normalizeUsername(customer.getUsername()) == normalizedUsername) {
            return true;
        }
    }

    for (const auto& admin : admins) {
        if (normalizeUsername(admin.getUsername()) == normalizedUsername) {
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
            loginFlow();
            break;
        case 2:
            registerCustomer();
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

    vector<string> customerUsernames;
    for (const auto& customer : customers) {
        customerUsernames.push_back(customer.getUsername());
    }

    vector<string> adminUsernames;
    for (const auto& admin : admins) {
        adminUsernames.push_back(admin.getUsername());
    }

    authManager.seedDefaultCredentials(customerUsernames, adminUsernames);
}

void ECommerceSystem::loadData() {
    products = fileManager.loadProducts(PRODUCTS_FILE);
    customers = fileManager.loadCustomers(CUSTOMERS_FILE);
    admins = fileManager.loadAdmins(ADMINS_FILE);
    orders = fileManager.loadOrders(ORDERS_FILE);
    authManager.loadCredentials(USERS_FILE);

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
    authManager.saveCredentials(USERS_FILE);
}

void ECommerceSystem::showMainMenu() const {
    printSectionTitle("Main Menu");
    cout << "1. Login\n"
         << "2. Register\n"
         << "0. Exit\n";
}

void ECommerceSystem::loginFlow() {
    while (true) {
        printSectionTitle("Login");
        string username;
        string pin;

        cout << "Username, or 0 to back: ";
        cin >> username;

        if (username == "0") {
            return;
        }

        cout << "PIN (4 digits), or 0 to back: ";
        cin >> pin;

        if (pin == "0") {
            return;
        }

        if (!authManager.isPinValid(pin)) {
            cout << "\nInvalid PIN format. PIN must contain exactly 4 digits.\n";
        } else {
            AuthResult result = authManager.authenticate(username, pin);
            if (result.success && result.role == AccountRole::Customer) {
                Customer* customer = findCustomerByUsername(result.username);
                if (customer == nullptr) {
                    cout << "Customer data not found.\n";
                    return;
                }

                printSuccess("Login successful.");
                pauseScreen();
                customerMenu(*customer);
                return;
            }

            if (result.success && result.role == AccountRole::Admin) {
                Admin* admin = findAdminByUsername(result.username);
                if (admin == nullptr) {
                    cout << "Admin data not found.\n";
                    return;
                }

                printSuccess("Login successful.");
                pauseScreen();
                adminMenu(*admin);
                return;
            }

            cout << result.message << "\n";
        }

        while (true) {
            int retryChoice = -1;
            printMinorDivider();
            cout << "1. Try again\n"
                 << "0. Back\n"
                 << "Enter your choice: ";
            cin >> retryChoice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input.\n";
                continue;
            }

            if (retryChoice == 1) {
                break;
            }

            if (retryChoice == 0) {
                return;
            }

            cout << "Invalid choice.\n";
        }
    }
}

void ECommerceSystem::registerCustomer() {
    printSectionTitle("Register Customer");

    string username;
    while (true) {
        cout << "Enter username, or 0 to cancel: ";
        cin >> username;

        if (username == "0") {
            cout << "Registration cancelled.\n";
            return;
        }

        if (!authManager.isUsernameFormatValid(username)) {
            cout << "Invalid username. Use lowercase letters and digits only, "
                 << "and start with a lowercase letter.\n";
            continue;
        }

        if (isUsernameTaken(username) || authManager.credentialExists(username)) {
            cout << "Username already exists.\n";
            continue;
        }

        break;
    }

    string pin;
    while (true) {
        cout << "Enter PIN (4 digits), or 0 to cancel: ";
        cin >> pin;

        if (pin == "0") {
            cout << "Registration cancelled.\n";
            return;
        }

        if (!authManager.isPinValid(pin)) {
            cout << "Invalid PIN format. PIN must contain exactly 4 digits.\n";
            continue;
        }

        break;
    }

    if (!authManager.addCustomerCredential(username, pin)) {
        cout << "Registration failed. Please try again.\n";
        pauseScreen();
        return;
    }

    customers.push_back(Customer(username));
    saveData();

    printSuccess("Customer account registered successfully.");
    pauseScreen();
}

void ECommerceSystem::viewAllProducts(Customer& customer) {
    while (true) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > Shop Products > All Products");
        printSectionTitle("All Products");

        if (products.empty()) {
            cout << "No products available.\n";
            pauseScreen();
            return;
        }

        vector<const Product*> productList;
        for (const auto& product : products) {
            productList.push_back(&product);
        }

        displayProductTable(productList);
        if (!handleProductListAction(customer, productList)) {
            return;
        }
    }
}

void ECommerceSystem::customerMenu(Customer& customer) {
    int choice = -1;
    while (choice != 0) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu");
        printSectionTitle("Customer Main Menu");
        cout << MENU_INDENT << "1. Shop Products\n"
             << MENU_INDENT << "2. View Cart\n"
             << MENU_INDENT << "3. View Order History\n"
             << MENU_INDENT << "4. View Recommendations\n"
             << MENU_INDENT << "0. Logout\n\n"
             << MENU_INDENT << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        switch (choice) {
        case 1:
            shopProductsMenu(customer);
            break;
        case 2:
            viewCartMenu(customer);
            break;
        case 3:
            clearScreen();
            displayHeader(customer);
            displayBreadcrumb("Home > Customer Main Menu > Order History");
            viewOrderHistory(customer);
            pauseScreen();
            break;
        case 4: {
            clearScreen();
            displayHeader(customer);
            displayBreadcrumb("Home > Customer Main Menu > Recommendations");
            int topN;
            cout << "Enter number of recommendations: ";
            cin >> topN;
            if (cin.fail() || topN <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid number.\n";
                pauseScreen();
            } else {
                showRecommendations(customer, topN);
            }
            break;
        }
        case 0:
            cout << "Customer logged out.\n";
            pauseScreen();
            break;
        default:
            cout << "Invalid choice.\n";
            pauseScreen();
            break;
        }
    }
}

void ECommerceSystem::shopProductsMenu(Customer& customer) {
    int choice = -1;
    while (choice != 0) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > Shop Products");
        printSectionTitle("Shop Products");
        cout << MENU_INDENT << "1. View All Products\n"
             << MENU_INDENT << "2. Search Products\n"
             << MENU_INDENT << "3. View Best-selling Products\n"
             << MENU_INDENT << "0. Back\n\n"
             << MENU_INDENT << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        switch (choice) {
        case 1:
            viewAllProducts(customer);
            break;
        case 2:
            searchProducts(customer);
            break;
        case 3:
            showBestSellingProducts(customer, 5);
            break;
        case 0:
            break;
        default:
            cout << "Invalid choice.\n";
            pauseScreen();
            break;
        }
    }
}

void ECommerceSystem::viewCartMenu(Customer& customer) {
    int cartChoice = -1;
    while (cartChoice != 0) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > View Cart");
        displayCartWithItemNumbers(customer.getCart(), "Your Cart");

        if (customer.getCart().isEmpty()) {
            cout << MENU_INDENT << "0. Back\n\n"
                 << MENU_INDENT << "Enter your choice: ";
            cin >> cartChoice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input.\n";
                pauseScreen();
                continue;
            }

            if (cartChoice != 0) {
                cout << "Invalid choice.\n";
                pauseScreen();
            }
            continue;
        }

        cout << MENU_INDENT << "1. Checkout\n"
             << MENU_INDENT << "2. Update cart\n"
             << MENU_INDENT << "0. Back\n\n"
             << MENU_INDENT << "Enter your choice: ";
        cin >> cartChoice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        if (cartChoice == 1) {
            if (confirmAction("Proceed to checkout?")) {
                checkout(customer, false);
                pauseScreen();
                cartChoice = 0;
            } else {
                cout << "Checkout cancelled.\n";
                pauseScreen();
            }
        } else if (cartChoice == 2) {
            updateCartMenu(customer);
        } else if (cartChoice != 0) {
            cout << "Invalid choice.\n";
            pauseScreen();
        }
    }
}

void ECommerceSystem::adminMenu(Admin& admin) {
    int choice = -1;
    while (choice != 0) {
        clearScreen();
        displayHeader(admin);
        displayBreadcrumb("Admin Portal > Dashboard");
        showAdminDashboard(admin);

        printSectionTitle("Admin Main Menu");
        cout << MENU_INDENT << "1. Inventory Management\n"
             << MENU_INDENT << "2. Business Insights\n"
             << MENU_INDENT << "0. Logout\n\n"
             << MENU_INDENT << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        switch (choice) {
        case 1:
            adminInventoryMenu(admin);
            break;
        case 2:
            adminInsightsMenu(admin);
            break;
        case 0:
            cout << "Admin logged out.\n";
            break;
        default:
            cout << "Invalid choice.\n";
            pauseScreen();
            break;
        }
    }
}

void ECommerceSystem::showAdminDashboard(const Admin& admin) const {
    (void)admin;

    int outOfStockCount = 0;
    for (const auto& product : products) {
        if (product.getStock() == 0) {
            ++outOfStockCount;
        }
    }

    cout << "================ ADMIN DASHBOARD ================\n";
    cout << "| Total products: " << setw(5) << products.size()
         << " | Out of stock: ";
    if (outOfStockCount > 0) {
        cout << "\033[31m" << setw(5) << outOfStockCount << " !!\033[0m";
    } else {
        cout << "\033[32m" << setw(5) << outOfStockCount << "\033[0m";
    }
    cout << " |\n";

    cout << "| Total orders:   " << setw(5) << orders.size()
         << " | Total customers: " << setw(3) << customers.size()
         << " |\n";
    printMinorDivider();
}

void ECommerceSystem::showAdminInventoryTable() const {
    cout << left
         << setw(5) << "ID"
         << setw(34) << "Product Name"
         << setw(14) << "Category"
         << right
         << setw(12) << "Price"
         << setw(8) << "Sold"
         << setw(14) << "Stock"
         << setw(8) << "Views"
         << "\n";

    cout << string(95, '-') << "\n";

    for (const auto& product : products) {
        string stockText = product.getStock() == 0
            ? "Sold out"
            : to_string(product.getStock());
        string stockColor = "\033[32m";
        if (product.getStock() == 0) {
            stockColor = "\033[31m";
        } else if (product.getStock() < 5) {
            stockColor = "\033[33m";
        }

        cout << left
             << setw(5) << product.getId()
             << setw(34) << product.getName().substr(0, 32)
             << setw(14) << categoryToString(product.getCategory()).substr(0, 12)
             << right
             << setw(12) << fixed << setprecision(0) << product.getPrice()
             << setw(8) << product.getPurchaseCount()
             << stockColor << setw(14) << stockText << "\033[0m"
             << setw(8) << product.getViewCount()
             << "\n";
    }

    cout << string(95, '-') << "\n";
}

void ECommerceSystem::adminInventoryMenu(const Admin& admin) {
    string action;
    while (true) {
        clearScreen();
        displayHeader(admin);
        displayBreadcrumb("Admin Portal > Inventory Management");
        printSectionTitle("Inventory Management");
        showAdminInventoryTable();

        cout << MENU_INDENT << "A      - Add product\n"
             << MENU_INDENT << "ID     - Edit product, example: 5\n"
             << MENU_INDENT << "D + ID - Delete product, example: D5\n"
             << MENU_INDENT << "0      - Back\n\n"
             << MENU_INDENT << "Enter your choice: ";
        cin >> action;

        if (action == "0") {
            return;
        }

        if (action == "A" || action == "a") {
            addProduct();
            pauseScreen();
            continue;
        }

        if (action.length() >= 2 && (action[0] == 'D' || action[0] == 'd')) {
            int productId;
            stringstream stream(action.substr(1));
            stream >> productId;
            if (stream.fail() || !stream.eof()) {
                cout << "Invalid delete command.\n";
                pauseScreen();
                continue;
            }

            deleteProduct(productId);
            pauseScreen();
            continue;
        }

        int productId;
        stringstream stream(action);
        stream >> productId;
        if (stream.fail() || !stream.eof()) {
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        editProduct(productId);
        pauseScreen();
    }
}

void ECommerceSystem::adminInsightsMenu(const Admin& admin) {
    int choice = -1;
    while (choice != 0) {
        clearScreen();
        displayHeader(admin);
        displayBreadcrumb("Admin Portal > Business Insights");
        printSectionTitle("Business Insights");
        cout << MENU_INDENT << "1. View most viewed products\n"
             << MENU_INDENT << "2. View best-selling products\n"
             << MENU_INDENT << "3. View active users\n"
             << MENU_INDENT << "4. View customer recommendations\n"
             << MENU_INDENT << "0. Back\n\n"
             << MENU_INDENT << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        clearScreen();
        displayHeader(admin);

        switch (choice) {
        case 1:
            displayBreadcrumb("Admin Portal > Business Insights > Most Viewed Products");
            showMostViewedProducts(5);
            pauseScreen();
            break;
        case 2:
            displayBreadcrumb("Admin Portal > Business Insights > Best-selling Products");
            showBestSellingProducts(5);
            pauseScreen();
            break;
        case 3:
            displayBreadcrumb("Admin Portal > Business Insights > Active Users");
            showActiveUsers(5);
            pauseScreen();
            break;
        case 4:
            showCustomerRecommendationsForAdmin(admin);
            break;
        case 0:
            break;
        default:
            cout << "Invalid choice.\n";
            pauseScreen();
            break;
        }
    }
}

void ECommerceSystem::showCustomerRecommendationsForAdmin(const Admin& admin) {
    clearScreen();
    displayHeader(admin);
    displayBreadcrumb("Admin Portal > Business Insights > Customer Recommendations");
    printSectionTitle("Customer Recommendations");

    if (customers.empty()) {
        cout << "No customer data available.\n";
        pauseScreen();
        return;
    }

    cout << left
         << setw(20) << "Username"
         << setw(12) << "Role"
         << right
         << setw(10) << "Orders"
         << setw(16) << "Total Spent"
         << setw(16) << "Activity"
         << "\n";
    cout << string(74, '-') << "\n";

    for (const auto& customer : customers) {
        cout << left
             << setw(20) << customer.getUsername()
             << setw(12) << "customer"
             << right
             << setw(10) << customer.getOrderHistory().size()
             << setw(16) << fixed << setprecision(0)
             << calculateCustomerSpending(customer.getUsername())
             << setw(16) << customer.getActivityScore()
             << "\n";
    }
    cout << string(74, '-') << "\n";

    string username;
    cout << MENU_INDENT << "Enter customer username, or 0 to back: ";
    cin >> username;
    username = normalizeUsername(username);

    if (username == "0") {
        return;
    }

    Customer* customer = findCustomerByUsername(username);
    if (customer == nullptr) {
        cout << "Customer not found.\n";
        pauseScreen();
        return;
    }

    int topN;
    cout << MENU_INDENT << "Enter number of recommendations: ";
    cin >> topN;

    if (cin.fail() || topN <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid number.\n";
        pauseScreen();
        return;
    }

    vector<RecommendationResult> results =
        recommendationEngine.recommendProductsWithScores(*customer, products, topN);

    clearScreen();
    displayHeader(admin);
    displayBreadcrumb("Admin Portal > Business Insights > Customer Recommendations");
    printSectionTitle("Recommendation Scores");
    cout << "Customer: " << customer->getUsername() << "\n";
    cout << "Formula: final = view_score + purchase_score + category_bonus + jaccard_bonus\n";
    cout << "view_score = view_count * 0.2 | purchase_score = purchase_count * 0.8\n";
    printMinorDivider();

    if (results.empty()) {
        cout << "No recommendations available.\n";
        pauseScreen();
        return;
    }

    cout << left
         << setw(6) << "Rank"
         << setw(6) << "ID"
         << setw(28) << "Product Name"
         << right
         << setw(10) << "View"
         << setw(10) << "Buy"
         << setw(10) << "Cat"
         << setw(10) << "Jaccard"
         << setw(10) << "Final"
         << "\n";
    cout << string(90, '-') << "\n";

    for (size_t i = 0; i < results.size(); ++i) {
        cout << left
             << setw(6) << i + 1
             << setw(6) << results[i].product.getId()
             << setw(28) << results[i].product.getName().substr(0, 26)
             << right
             << fixed << setprecision(2)
             << setw(10) << results[i].viewScore
             << setw(10) << results[i].purchaseScore
             << setw(10) << results[i].categoryBonus
             << setw(10) << results[i].jaccardBonus
             << setw(10) << results[i].finalScore
             << "\n";
    }

    cout << string(90, '-') << "\n";
    pauseScreen();
}

void ECommerceSystem::searchProducts(Customer& customer) {
    int searchChoice = -1;
    while (searchChoice != 0) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > Shop Products > Search Products");
        printSectionTitle("Search Products");
        cout << "1. Search by name or keyword\n"
             << "2. Search by price range\n"
             << "0. Back\n"
             << "Enter your choice: ";
        cin >> searchChoice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            pauseScreen();
            continue;
        }

        vector<const Product*> matchedProducts;

        if (searchChoice == 1) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            string keyword;
            cout << "Enter product name or keyword: ";
            getline(cin, keyword);

            vector<string> keywordTokens = tokenizeSearchText(keyword);
            if (keywordTokens.empty()) {
                cout << "Keyword cannot be empty.\n";
                pauseScreen();
                continue;
            }

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
        } else if (searchChoice == 2) {
            double minPrice;
            double maxPrice;

            cout << "Enter minimum price: ";
            cin >> minPrice;
            cout << "Enter maximum price: ";
            cin >> maxPrice;

            if (cin.fail() || minPrice < 0 || maxPrice < 0 || minPrice > maxPrice) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid price range.\n";
                pauseScreen();
                continue;
            }

            for (const auto& product : products) {
                if (product.getPrice() >= minPrice && product.getPrice() <= maxPrice) {
                    matchedProducts.push_back(&product);
                }
            }
        } else if (searchChoice == 0) {
            return;
        } else {
            cout << "Invalid choice.\n";
            pauseScreen();
            continue;
        }

        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > Shop Products > Search Results");
        printSectionTitle("Search Results");
        if (matchedProducts.empty()) {
            cout << "No products matched your search.\n";
            pauseScreen();
            continue;
        }

        while (true) {
            clearScreen();
            displayHeader(customer);
            displayBreadcrumb("Home > Customer Main Menu > Shop Products > Search Results");
            printSectionTitle("Search Results");
            displayProductTable(matchedProducts);
            if (!handleProductListAction(customer, matchedProducts)) {
                break;
            }
        }
    }
}

void ECommerceSystem::showBestSellingProducts(Customer& customer, int topN) {
    while (true) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > Shop Products > Best-selling Products");
        printSectionTitle("Best-selling Products");

        vector<Product> bestSelling = statisticsManager.getBestSellingProducts(products, topN);
        if (bestSelling.empty()) {
            cout << "No sales data available.\n";
            pauseScreen();
            return;
        }

        vector<const Product*> productList;
        for (const auto& bestSellingProduct : bestSelling) {
            if (bestSellingProduct.getPurchaseCount() <= 0) {
                continue;
            }

            const Product* product = findProductById(bestSellingProduct.getId());
            if (product != nullptr) {
                productList.push_back(product);
            }
        }

        if (productList.empty()) {
            cout << "No sold products available.\n";
            pauseScreen();
            return;
        }

        displayProductTable(productList);
        if (!handleProductListAction(customer, productList)) {
            return;
        }
    }
}

bool ECommerceSystem::handleProductListAction(
    Customer& customer,
    const vector<const Product*>& productList
) {
    int productId;
    cout << "Enter product ID to view/buy, or 0 to back: ";
    cin >> productId;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid product ID.\n";
        pauseScreen();
        return true;
    }

    if (productId == 0) {
        return false;
    }

    bool existsInCurrentList = false;
    for (const auto* product : productList) {
        if (product->getId() == productId) {
            existsInCurrentList = true;
            break;
        }
    }

    if (!existsInCurrentList) {
        cout << "Product ID is not in the current list.\n";
        pauseScreen();
        return true;
    }

    clearScreen();
    displayHeader(customer);
    displayBreadcrumb("Home > Customer Main Menu > Shop Products > Product Detail");
    viewProductDetail(customer, productId);

    Product* product = findProductById(productId);
    if (product == nullptr) {
        pauseScreen();
        return true;
    }

    int currentQuantityInCart = 0;
    for (const auto& item : customer.getCart().getItems()) {
        if (item.getProduct().getId() == productId) {
            currentQuantityInCart = item.getQuantity();
            break;
        }
    }

    int maxCanAdd = product->getStock() - currentQuantityInCart;
    if (maxCanAdd <= 0) {
        cout << "This product cannot be added because there is no available stock left.\n";
        pauseScreen();
        return true;
    }

    int quantity;
    if (promptQuantityWithStockLimit("Enter quantity to add, or 0 to cancel: ", maxCanAdd, quantity)) {
        addToCart(customer, productId, quantity);
    } else {
        cout << "Add to cart cancelled.\n";
    }

    pauseScreen();
    return true;
}

void ECommerceSystem::updateCartMenu(Customer& customer) {
    bool backToCustomerMenu = false;
    while (!backToCustomerMenu) {
        clearScreen();
        displayHeader(customer);
        displayBreadcrumb("Home > Customer Main Menu > View Cart > Update Cart");
        displayCartWithItemNumbers(customer.getCart(), "Current Cart");

        if (customer.getCart().isEmpty()) {
            pauseScreen();
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
            if (confirmAction("Are you sure you want to clear the cart?")) {
                customer.getCart().clear();
                printSuccess("Cart cleared successfully.");
                pauseScreen();
                return;
            }
            cout << "Clear cart cancelled.\n";
            pauseScreen();
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
            clearScreen();
            displayHeader(customer);
            displayBreadcrumb("Home > Customer Main Menu > View Cart > Update Cart > Selected Item");
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
                if (!promptQuantityWithStockLimit(
                        "Enter new quantity, or 0 to remove product: ",
                        product->getStock(),
                        quantity
                    )) {
                    if (confirmAction("Remove this product from cart?")) {
                        customer.getCart().updateQuantity(productId, 0);
                        printSuccess("Product removed from cart.");
                        cout << "Updated total: " << fixed << setprecision(0)
                             << customer.getCart().calculateTotal() << "\n";
                    } else {
                        cout << "Remove product cancelled.\n";
                    }
                    pauseScreen();
                    action = 0;
                    continue;
                }

                customer.getCart().updateQuantity(productId, quantity);
                printSuccess("Cart updated successfully.");
                cout << "Updated total: " << fixed << setprecision(0)
                     << customer.getCart().calculateTotal() << "\n";
                pauseScreen();
                action = 0;
            } else if (action == 2) {
                if (confirmAction("Remove this product from cart?")) {
                    customer.getCart().removeProduct(productId);
                    printSuccess("Product removed from cart.");
                    cout << "Updated total: " << fixed << setprecision(0)
                         << customer.getCart().calculateTotal() << "\n";
                } else {
                    cout << "Remove product cancelled.\n";
                }
                pauseScreen();
                action = 0;
            } else if (action != 0) {
                cout << "Invalid choice.\n";
                pauseScreen();
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

    if (quantity <= 0) {
        cout << "Invalid quantity.\n";
        return;
    }

    int currentQuantityInCart = 0;
    for (const auto& item : customer.getCart().getItems()) {
        if (item.getProduct().getId() == productId) {
            currentQuantityInCart = item.getQuantity();
            break;
        }
    }

    int maxCanAdd = product->getStock() - currentQuantityInCart;
    if (quantity > maxCanAdd) {
        cout << "Only " << maxCanAdd << " items available in stock.\n";
        return;
    }

    customer.getCart().addProduct(*product, quantity);
    printSuccess("Added " + to_string(quantity) + " x " + product->getName()
        + " to cart successfully.");
}

void ECommerceSystem::checkout(Customer& customer, bool requireConfirmation) {
    if (customer.getCart().isEmpty()) {
        cout << "Cart is empty.\n";
        return;
    }

    if (requireConfirmation) {
        displayCartWithItemNumbers(customer.getCart(), "Checkout Cart");

        if (!confirmAction("Proceed to checkout?")) {
            cout << "Checkout cancelled.\n";
            return;
        }
    }

    Order order(generateOrderId(), customer.getUsername());
    for (const auto& item : customer.getCart().getItems()) {
        Product* product = findProductById(item.getProduct().getId());
        if (product == nullptr) {
            cout << "Checkout failed.\n\n"
                 << "Product: " << item.getProduct().getName() << "\n"
                 << "This product is no longer available.\n"
                 << "Please update your cart before checkout.\n";
            updateCartMenu(customer);
            return;
        }

        if (item.getQuantity() > product->getStock()) {
            cout << "Checkout failed.\n\n"
                 << "Product: " << product->getName() << "\n"
                 << "Quantity in cart: " << item.getQuantity() << "\n"
                 << "Available stock: " << product->getStock() << "\n"
                 << "Please update your cart before checkout.\n";
            updateCartMenu(customer);
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
    printSuccess("Checkout successful.");
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

void ECommerceSystem::showRecommendations(Customer& customer, int topN) {
    vector<Product> recommendations =
        recommendationEngine.recommendProducts(customer, products, topN);

    if (recommendations.empty()) {
        cout << "No recommendations available.\n";
        pauseScreen();
        return;
    }

    printSectionTitle("Top Recommendations");
    cout << "Showing top " << topN << " products\n";

    vector<const Product*> productList;
    for (const auto& recommendedProduct : recommendations) {
        const Product* product = findProductById(recommendedProduct.getId());
        if (product != nullptr) {
            productList.push_back(product);
        }
    }

    if (productList.empty()) {
        cout << "No recommendations available.\n";
        pauseScreen();
        return;
    }

    displayProductTable(productList);
    handleProductListAction(customer, productList);
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
        splitCommaSeparatedValues(devicesInput),
        splitCommaSeparatedValues(tagsInput),
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

    clearScreen();
    cout << "[Admin]\n";
    printMinorDivider();
    displayBreadcrumb("Admin Portal > Inventory Management > Edit Product");
    printSectionTitle("Editing Product");
    product->display();

    int choice = -1;
    printSectionTitle("Edit Product Menu");
    cout << MENU_INDENT << "1. Edit name\n"
         << MENU_INDENT << "2. Edit category\n"
         << MENU_INDENT << "3. Edit compatible devices\n"
         << MENU_INDENT << "4. Edit tags\n"
         << MENU_INDENT << "5. Edit price\n"
         << MENU_INDENT << "6. Edit stock\n"
         << MENU_INDENT << "0. Cancel\n\n"
         << MENU_INDENT << "Enter your choice: ";
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

    if (choice < 0 || choice > 6) {
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
        product->setCompatibleDevices(splitCommaSeparatedValues(devicesInput));
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
        product->setTags(splitCommaSeparatedValues(tagsInput));
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

    if (!updated) {
        cout << "No changes were saved.\n";
        return;
    }

    saveData();
    printSuccess("Product updated successfully.");
    printSectionTitle("Updated Product");
    product->displayForAdmin();
}

void ECommerceSystem::deleteProduct(int productId) {
    Product* product = findProductById(productId);
    if (product == nullptr) {
        cout << "Product not found.\n";
        return;
    }

    printDanger("\nProduct selected for deletion:");
    printMinorDivider();
    product->display();
    printMinorDivider();
    
    printDanger("Are you sure you want to delete '" + product->getName() + "'?");
    if (!confirmAction("Confirm delete")) {
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
    printSuccess("Product deleted successfully.");
}

void ECommerceSystem::showMostViewedProducts(int topN) const {
    vector<Product> mostViewed = statisticsManager.getMostViewedProducts(products, topN);
    printSectionTitle("Top Most Viewed Products");
    if (mostViewed.empty()) {
        cout << "No product data available.\n";
        return;
    }

    cout << left
         << setw(6) << "Rank"
         << setw(6) << "ID"
         << setw(34) << "Product Name"
         << right
         << setw(10) << "Views"
         << setw(8) << "Sold"
         << setw(12) << "Stock"
         << "\n";
    cout << string(76, '-') << "\n";

    for (size_t i = 0; i < mostViewed.size(); ++i) {
        string stockDisplay = mostViewed[i].getStock() == 0
            ? "Sold out"
            : to_string(mostViewed[i].getStock());

        cout << left
             << setw(6) << i + 1
             << setw(6) << mostViewed[i].getId()
             << setw(34) << mostViewed[i].getName().substr(0, 32)
             << right
             << setw(10) << mostViewed[i].getViewCount()
             << setw(8) << mostViewed[i].getPurchaseCount()
             << setw(12) << stockDisplay
             << "\n";
    }
    cout << string(76, '-') << "\n";
}

void ECommerceSystem::showBestSellingProducts(int topN) const {
    vector<Product> bestSelling = statisticsManager.getBestSellingProducts(products, topN);
    printSectionTitle("Top Best-Selling Products");
    if (bestSelling.empty()) {
        cout << "No sales data available.\n";
        return;
    }

    cout << left
         << setw(6) << "Rank"
         << setw(6) << "ID"
         << setw(34) << "Product Name"
         << right
         << setw(10) << "Sold"
         << setw(12) << "Price"
         << setw(12) << "Stock"
         << "\n";
    cout << string(80, '-') << "\n";

    for (size_t i = 0; i < bestSelling.size(); ++i) {
        if (bestSelling[i].getPurchaseCount() <= 0) {
            continue;
        }

        string stockDisplay = bestSelling[i].getStock() == 0
            ? "Sold out"
            : to_string(bestSelling[i].getStock());

        cout << left
             << setw(6) << i + 1
             << setw(6) << bestSelling[i].getId()
             << setw(34) << bestSelling[i].getName().substr(0, 32)
             << right
             << setw(10) << bestSelling[i].getPurchaseCount()
             << setw(12) << fixed << setprecision(0) << bestSelling[i].getPrice()
             << setw(12) << stockDisplay
             << "\n";
    }
    cout << string(80, '-') << "\n";
}

void ECommerceSystem::showActiveUsers(int topN) const {
    vector<Customer> activeUsers = statisticsManager.getMostActiveUsers(customers, topN);
    printSectionTitle("Top Active Users");
    if (activeUsers.empty()) {
        cout << "No user activity available.\n";
        return;
    }

    cout << left
         << setw(20) << "Username"
         << setw(12) << "Role"
         << right
         << setw(10) << "Orders"
         << setw(16) << "Total Spent"
         << setw(16) << "Activity"
         << "\n";
    cout << string(74, '-') << "\n";

    for (size_t i = 0; i < activeUsers.size(); ++i) {
        cout << left
             << setw(20) << activeUsers[i].getUsername()
             << setw(12) << "customer"
             << right
             << setw(10) << activeUsers[i].getOrderHistory().size()
             << setw(16) << fixed << setprecision(0)
             << calculateCustomerSpending(activeUsers[i].getUsername())
             << setw(16) << activeUsers[i].getActivityScore()
             << "\n";
    }

    cout << string(74, '-') << "\n";
}

double ECommerceSystem::calculateCustomerSpending(const string& username) const {
    double total = 0.0;

    for (const auto& order : orders) {
        if (order.getCustomerUsername() == username) {
            total += order.getTotalAmount();
        }
    }

    return total;
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
