# Project 2 — Smart E-Commerce System

## How to run

```bash
cd "Project 2"
g++ -std=c++17 -Iheaders main.cpp src/*.cpp -o project2
./project2
```

Requires a C++17 compiler. Run from inside the `Project 2` folder so the program can find the `data/` files.

## Files

**Source**

| File | Purpose |
| --- | --- |
| `main.cpp` | Entry point. Creates an `ECommerceSystem` and calls `run()`. |
| `src/ECommerceSystem.cpp` | Central controller. Manages the menu loop, all user actions, and coordinates the other modules. |
| `src/AuthManager.cpp` | Validates login credentials and handles customer registration. |
| `src/User.cpp` | Base class holding the username shared by `Customer` and `Admin`. |
| `src/Customer.cpp` | Customer account with a shopping cart, interaction history, and order history. |
| `src/Admin.cpp` | Admin account with inventory and insights access. |
| `src/Product.cpp` | Product data (name, category, price, stock, view and purchase counts). |
| `src/Category.cpp` | Enum and helper functions for the six product categories. |
| `src/ShoppingCart.cpp` | Holds cart items; supports add, update, remove, and checkout. |
| `src/CartItem.cpp` | A single product and quantity pair inside the cart. |
| `src/Order.cpp` | A completed order with its items and total amount. |
| `src/OrderItem.cpp` | A single product snapshot inside a completed order. |
| `src/InteractionHistory.cpp` | Tracks per-customer view and purchase counts per product. |
| `src/RecommendationEngine.cpp` | Scores and ranks in-stock products for a customer using view, purchase, category, and Jaccard signals. |
| `src/StatisticsManager.cpp` | Computes most viewed products, best sellers, most active users, and revenue. |
| `src/FileManager.cpp` | Reads and writes all data to the `data/` text files. |

**Headers** (`headers/`) — declarations for each class above.

**Data**

| File | Purpose |
| --- | --- |
| `data/users.txt` | Login credentials (username, PIN, role). |
| `data/admins.txt` | Admin usernames. |
| `data/customers.txt` | Customer profiles including interaction history and order IDs. |
| `data/products.txt` | Product catalog with stock, view, and purchase counts. |
| `data/orders.txt` | Completed order records. |
