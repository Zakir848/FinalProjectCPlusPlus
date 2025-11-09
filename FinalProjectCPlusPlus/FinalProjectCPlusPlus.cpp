#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <limits>
using namespace std;

// ==== ENUMS ====
enum OrderStatus {
    Received,
    Preparing,
    Cooking,
    Packed,
    Ready
};

enum Gender {
    Male,
    Female
};

// ==== INGREDIENT CLASS ====
class Ingredient { // düz yazılış!
    string name;
    double amount;
public:
    Ingredient() {}
    Ingredient(string name, double amount) {
        setName(name);
        setAmount(amount);
    }

    void setName(string name) {
        if (name.empty()) throw string("Ingredient name cannot be empty!");
        this->name = name;
    }

    void setAmount(double amount) {
        if (amount <= 0) throw string("Ingredient amount must be positive!");
        this->amount = amount;
    }

    string getName() const noexcept { return name; }
    double getAmount() const noexcept { return amount; }

    void increase(double value) { amount += value; }
    void decrease(double value) {
        if (value > amount)
            throw string("Not enough ingredient in stock: " + name);
        amount -= value;
    }

    void showIngredient() const {
        cout << "---------------------------------\n";
        cout << "Ingredient Name: " << name << endl;
        cout << "Ingredient Amount: " << amount << endl;
    }
};

// ==== DISH CLASS ====
class Dish {
    string name;
    string description;
    vector<Ingredient> ingredients;
    double price;
public:
    Dish() = default;

    Dish(const string name, const string& description, double price) {
        setName(name);
        setDescription(description);
        setPrice(price);
    }

    void setName(string name) {
        if (name.empty()) throw string("Dish name cannot be empty!");
        this->name = name;
    }

    void setDescription(string description) {
        if (description.empty()) throw string("Dish description cannot be empty!");
        this->description = description;
    }

    void setPrice(double price) {
        if (price <= 0) throw string("Dish price must be positive!");
        this->price = price;
    }

    string getName() const noexcept { return name; }
    string getDescription() const noexcept { return description; }
    double getPrice() const noexcept { return price; }
    vector<Ingredient> getIngredients() const noexcept { return ingredients; }

    void addIngredient(const Ingredient& ingredient) {
        ingredients.push_back(ingredient);
    }

    void show() const {
        cout << "Dish Name: " << name << "\n";
        cout << "Description: " << description << "\n";
        cout << "Price: " << price << "\n";
        cout << "Ingredients:\n";
        if (ingredients.empty()) {
            cout << "  (No ingredients)\n";
        }
        else {
            for (const auto& ing : ingredients) {
                cout << "  - " << ing.getName()
                    << " (Amount: " << ing.getAmount() << ")\n";
            }
        }
    }
};

// ==== STOCK CLASS ====
class Stock {
    vector<Ingredient> storage;

    static string toLower(string s) {
        for (auto& c : s) c = tolower(c);
        return s;
    }
public:
    Stock() {
        try { loadStorage(); }
        catch (...) {}
    }

    void addIngredient(const Ingredient& ingredient) {
        for (auto& stock : storage) {
            if (toLower(stock.getName()) == toLower(ingredient.getName())) {
                stock.increase(ingredient.getAmount());
                saveStorage();
                cout << "Ingredient amount increased: " << stock.getName() << endl;
                return;
            }
        }
        storage.push_back(ingredient);
        saveStorage();
        cout << "Ingredient added to stock: " << ingredient.getName() << endl;
    }

    void useIngredient(const Dish& dish) {
        for (const auto& ing : dish.getIngredients()) {
            bool found = false;
            for (auto& ingStock : storage) {
                if (toLower(ingStock.getName()) == toLower(ing.getName())) {
                    ingStock.decrease(ing.getAmount());
                    found = true;
                    break;
                }
            }
            if (!found)
                throw string("Ingredient not found in stock: " + ing.getName());
        }
        saveStorage();
        cout << "Stock updated for dish: " << dish.getName() << endl;
    }

    void loadStorage(string filePath = "StorageForIngredient.txt") {
        storage.clear();
        ifstream fs(filePath);
        if (!fs.is_open()) {
            ofstream create(filePath);
            create.close();
            return;
        }
        string row;
        while (getline(fs, row)) {
            if (row.empty()) continue;

            string name;
            double amount = 0;
            size_t pos = row.find('_');
            if (pos != string::npos) {
                name = row.substr(0, pos);
                amount = stod(row.substr(pos + 1));
                storage.push_back(Ingredient(name, amount));
            }
        }
    }

    void saveStorage(string filePath = "StorageForIngredient.txt") {
        ofstream fs(filePath);
        if (!fs.is_open()) throw string("File cannot be opened!");
        for (auto& s : storage)
            fs << s.getName() << "_" << s.getAmount() << "\n";
        fs.close();
    }

    void showStock() const {
        if (storage.empty()) throw string("Stock is empty!");
        cout << "Current Stock:\n";
        for (auto& ing : storage)
            ing.showIngredient();
    }
};

// ==== ORDER CLASS ====
class Order {
public:
    string userId;
    string dishName;
    OrderStatus status;

    Order(string userId, string dishName, int st = (int)Received)
        : userId(userId), dishName(dishName), status((OrderStatus)st) {
    }

    // Seriyalizasiya üçün
    string toString() const {
        return userId + "_" + dishName + "_" + to_string((int)status);
    }
    static Order fromString(const string& data) {
        stringstream ss(data);
        string userId, dishName, statusStr;
        getline(ss, userId, '_');
        getline(ss, dishName, '_');
        getline(ss, statusStr);
        int st = stoi(statusStr);
        return Order(userId, dishName, st);
    }
};

// ==== ORDER MANAGER ====
class OrderManager {
    vector<Order> orders;
    vector<Dish>* dishesRef = nullptr;
    Stock stock;

    string statusToString(OrderStatus st) const {
        switch (st) {
        case Received: return "Received";
        case Preparing: return "Preparing";
        case Cooking: return "Cooking";
        case Packed: return "Packed";
        case Ready: return "Ready";
        default: return "?";
        }
    }
public:
    OrderManager() {
        loadOrders();
    }
    ~OrderManager() {
        saveOrders();
    }

    void bindDishList(vector<Dish>* dishes) {
        dishesRef = dishes;
    }

    int getDishCount() const {
        return dishesRef ? dishesRef->size() : 0;
    }

    string getDishNameByIndex(int index) const {
        if (!dishesRef) throw string("Dish list is not loaded!");
        if (index < 0 || index >= dishesRef->size())
            throw string("Invalid dish index!");
        return (*dishesRef)[index].getName();
    }

    void showAllDishes() const {
        if (!dishesRef) { cout << "Dish list not loaded!\n"; return; }
        for (auto& d : *dishesRef) {
            d.show();
            cout << "------------------------\n";
        }
    }

    void showAllDishesWithIndex() const {
        if (!dishesRef) { cout << "Dish list not loaded!\n"; return; }
        cout << "\n===== MENU =====\n";
        for (size_t i = 0; i < dishesRef->size(); i++)
            cout << i << ") " << (*dishesRef)[i].getName()
            << "  Price: " << (*dishesRef)[i].getPrice() << endl;
        cout << "================\n";
    }

    void createOrder(const string& userId, const string& dishName) {
        orders.push_back(Order(userId, dishName, (int)Received));
        saveOrders();
    }

    void moveOrderForward(const string& userId) {
        for (auto& ord : orders) {
            if (ord.userId == userId) {
                // DƏYİŞİKLİK: Stock RECEIVED-də azaldılsın
                if (ord.status == Received) {
                    if (!dishesRef) {
                        cout << "Dish list is not loaded!\n";
                        return;
                    }
                    Dish* targetDish = nullptr;
                    for (auto& d : *dishesRef) {
                        if (d.getName() == ord.dishName) {
                            targetDish = &d;
                            break;
                        }
                    }
                    if (!targetDish) {
                        cout << "Dish does not exist in the menu!\n";
                        return;
                    }
                    try {
                        stock.useIngredient(*targetDish);
                    }
                    catch (const string& ex) {
                        cout << ex << endl;
                        cout << "Order cannot move to Preparing stage!\n";
                        return;
                    }
                }
                if (ord.status < Ready)
                    ord.status = (OrderStatus)(ord.status + 1);
                saveOrders();
                if (ord.status == Ready)
                    cout << "Order is ready for pickup!\n";
                else
                    cout << "Order progressed to status: " << statusToString(ord.status) << endl;
                return;
            }
        }
        cout << "Order not found!\n";
    }

    void showMyOrderStatus(const string& userId) const {
        for (auto& ord : orders) {
            if (ord.userId == userId) {
                cout << "Dish: " << ord.dishName << " | Status: " << statusToString(ord.status) << endl;
                return;
            }
        }
        cout << "You have no active orders.\n";
    }

    void showAllOrders() const {
        cout << "\nCurrent Orders:\n";
        bool any = false;
        for (const auto& ord : orders) {
            cout << "UserID: " << ord.userId << ", Dish: " << ord.dishName
                << ", Status: " << statusToString(ord.status) << endl;
            any = true;
        }
        if (!any)
            cout << "(No orders found)\n";
    }

    // -- Orders Faylda SAXLA/oxu --
    void saveOrders(const string& filePath = "Orders.txt") {
        ofstream fs(filePath);
        if (!fs.is_open()) return;
        for (const auto& order : orders)
            fs << order.toString() << endl;
        fs.close();
    }
    void loadOrders(const string& filePath = "Orders.txt") {
        orders.clear();
        ifstream fs(filePath);
        if (!fs.is_open()) return;
        string line;
        while (getline(fs, line)) {
            if (!line.empty())
                orders.push_back(Order::fromString(line));
        }
        fs.close();
    }
};

// ==== ADMIN CLASS ====
class Admin {
    Stock stock;
    vector<Dish> dishes;
    OrderManager& orderManager;
public:
    Admin(OrderManager& om) : orderManager(om) {
        loadAllData();
        orderManager.bindDishList(&dishes);
    }

    void AdminPanel() {
        int choice;
        do {
            cout << "\n=== ADMIN PANEL ===\n";
            cout << "1. Add new dish\n";
            cout << "2. Edit dish\n";
            cout << "3. Delete dish\n";
            cout << "4. Show all dishes\n";
            cout << "5. Add ingredient to stock\n";
            cout << "6. Show stock\n";
            cout << "7. Move order status forward\n";
            cout << "8. Show all orders\n";
            cout << "0. Exit\n";
            cout << "Choice: ";
            cin >> choice;

            if (choice == 7) {
                orderManager.showAllOrders();
                string userId;
                cout << "Enter User ID: ";
                cin >> userId;
                orderManager.moveOrderForward(userId);
                continue;
            }
            if (choice == 8) {
                orderManager.showAllOrders();
                continue;
            }
            switch (choice) {
            case 1: addDish(); break;
            case 2: updateDish(); break;
            case 3: deleteDish(); break;
            case 4: showAllDishes(); break;
            case 5: addIngredientToStock(); break;
            case 6: showStock(); break;
            case 0: cout << "Exiting admin panel...\n"; break;
            default: cout << "Invalid choice!\n"; break;
            }
        } while (choice != 0);
    }

    void addDish() {
        try {
            string name, description;
            double price;
            int ingCount;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter dish name: ";
            getline(cin, name);
            cout << "Enter dish description: ";
            getline(cin, description);
            cout << "Enter dish price: ";
            cin >> price;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            Dish d(name, description, price);

            cout << "How many ingredients?: ";
            cin >> ingCount;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            for (int i = 0; i < ingCount; i++) {
                string ingName; double ingAmount;
                cout << "Ingredient " << i + 1 << " name: ";
                getline(cin, ingName);
                cout << "Amount: ";
                cin >> ingAmount;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                d.addIngredient(Ingredient(ingName, ingAmount));
            }

            dishes.push_back(d);
            saveAllData();
            cout << "Dish successfully added!\n";
        }
        catch (string ex) { cout << ex << endl; }
    }

    void updateDish() {
        try {
            if (dishes.empty()) { cout << "No dishes available.\n"; return; }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string name;
            cout << "Enter dish name to edit: ";
            getline(cin, name);

            for (auto& d : dishes) {
                if (d.getName() == name) {
                    string newDesc; double newPrice;
                    cout << "New description: ";
                    getline(cin, newDesc);
                    cout << "New price: ";
                    cin >> newPrice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    d.setDescription(newDesc);
                    d.setPrice(newPrice);
                    saveAllData();
                    cout << "Dish updated!\n";
                    return;
                }
            }
            cout << "Dish not found!\n";
        }
        catch (string ex) { cout << ex << endl; }
    }

    void deleteDish() {
        if (dishes.empty()) { cout << "No dishes to delete.\n"; return; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string name;
        cout << "Enter dish name to delete: ";
        getline(cin, name);

        for (size_t i = 0; i < dishes.size(); i++) {
            if (dishes[i].getName() == name) {
                dishes.erase(dishes.begin() + i);
                saveAllData();
                cout << "Dish deleted!\n";
                return;
            }
        }
        cout << "Dish not found!\n";
    }

    void showAllDishes() {
        if (dishes.empty()) { cout << "Menu is empty.\n"; return; }
        for (auto& d : dishes) {
            cout << "-----------------------------------------------\n";
            d.show();
            cout << "-----------------------------------------------\n";
        }
    }

    void addIngredientToStock() {
        try {
            string name; double amount;
            cout << "Enter ingredient name: ";
            cin >> name;
            cout << "Enter amount: ";
            cin >> amount;
            stock.addIngredient(Ingredient(name, amount));
        }
        catch (string ex) { cout << ex << endl; }
    }

    void showStock() {
        try { stock.showStock(); }
        catch (string ex) { cout << ex << endl; }
    }

    void saveAllData(string filePath = "Dishes.txt") {
        ofstream fs(filePath);
        if (!fs.is_open()) throw string("File cannot be opened!");
        for (auto& d : dishes) {
            fs << d.getName() << "_" << d.getDescription() << "_" << d.getPrice();
            for (auto& ing : d.getIngredients())
                fs << "_" << ing.getName() << ":" << ing.getAmount();
            fs << "\n";
        }
        fs.close();
    }

    void loadAllData(string filePath = "Dishes.txt") {
        dishes.clear();
        ifstream fs(filePath);
        if (!fs.is_open()) {
            ofstream create(filePath);
            create.close();
            return;
        }
        string row;
        while (getline(fs, row)) {
            if (row.empty()) continue;
            stringstream ss(row);
            string name, description, price_str;
            getline(ss, name, '_');
            getline(ss, description, '_');
            getline(ss, price_str, '_');
            if (name.empty() || description.empty() || price_str.empty()) continue;
            double price = stod(price_str);
            Dish d(name, description, price);
            string ingPair;
            while (getline(ss, ingPair, '_')) {
                size_t pos = ingPair.find(':');
                if (pos == string::npos) continue;
                string ingName = ingPair.substr(0, pos);
                double ingAmount = stod(ingPair.substr(pos + 1));
                d.addIngredient(Ingredient(ingName, ingAmount));
            }
            dishes.push_back(d);
        }
    }
};

// ==== USER CLASS ====
class User {
    string id;
    string username;
    string password;
    string email;
    string name;
    string surname;
    string number;
    Gender gender;
    string card;
    tm dataofbirth{};
public:
    User() {}

    User(string id, string username, string password, string email, string name,
        string surname, string number, Gender gender, int day, int month, int year, string card = "") {

        setId(id);
        setUsername(username);
        setPassword(password);
        setEmail(email);
        setName(name);
        setSurname(surname);
        setDataOfBirth(day, month, year);
        setGender(gender);
        setNumber(number);
        setCard(card);
    }

    static bool isValidPhoneNumber(const string& number) {
        const regex pattern("^\\+994(50|51|55|70|77|10|99)[0-9]{7}$");
        return regex_match(number, pattern);
    }
    static bool isValidEmail(const string& email) {
        const regex pattern("^[A-Za-z0-9._%+-]+@(gmail|mail|outlook|yahoo)\\.(com|ru|az)$");
        return regex_match(email, pattern);
    }
    static bool isValidId(const string& id) {
        const regex pattern("^[A-Z0-9]{7}$");
        return regex_match(id, pattern);
    }

    void setId(string id) {
        if (isValidId(id)) this->id = id;
        else throw string("Invalid ID format!");
    }
    void setUsername(string username) {
        if (username.length() >= 8) this->username = username;
        else throw string("Username must be at least 8 characters!");
    }
    void setPassword(string password) {
        if (password.length() >= 8) this->password = password;
        else throw string("Password must be at least 8 characters!");
    }
    void setEmail(string email) {
        if (isValidEmail(email)) this->email = email;
        else throw string("Invalid email format!");
    }
    void setName(string name) {
        if (name.length() >= 2) this->name = name;
        else throw string("Name must be at least 2 characters!");
    }
    void setSurname(string surname) {
        if (surname.length() >= 4) this->surname = surname;
        else throw string("Surname must be at least 4 characters!");
    }
    void setCard(string card) { this->card = card; }
    void setDataOfBirth(int day, int month, int year) {
        if (year < 1900 || year > 2025) throw string("Invalid year!");
        if (month < 1 || month > 12) throw string("Invalid month!");
        int maxDays = 31;
        switch (month) {
        case 4: case 6: case 9: case 11: maxDays = 30; break;
        case 2: maxDays = (year % 4 == 0 ? 29 : 28); break;
        }
        if (day < 1 || day > maxDays)
            throw string("Invalid day for selected month!");
        dataofbirth.tm_mday = day;
        dataofbirth.tm_mon = month - 1;
        dataofbirth.tm_year = year - 1900;
        if (getAge() < 18) throw string("User must be at least 18!");
    }
    void setGender(Gender g) { gender = g; }
    void setNumber(string number) {
        if (isValidPhoneNumber(number))
            this->number = number;
        else
            throw string("Invalid phone number format!");
    }
    string getId() const noexcept { return id; }
    string getUserName() const noexcept { return username; }
    string getPassword() const noexcept { return password; }
    string getEmail() const noexcept { return email; }
    string getName() const noexcept { return name; }
    string getSurname() const noexcept { return surname; }
    string getNumber() const noexcept { return number; }
    string getCard() const noexcept { return card; }
    tm getDataOfBirth() const noexcept { return dataofbirth; }
    string getGender() const noexcept { return (gender == Male ? "Male" : "Female"); }
    int getAge() const noexcept {
        time_t now = time(nullptr);
        tm current;
        tm* ptm = localtime(&now);
        if (ptm) current = *ptm;
        int age = (current.tm_year + 1900) - (dataofbirth.tm_year + 1900);
        if ((current.tm_mon < dataofbirth.tm_mon) ||
            (current.tm_mon == dataofbirth.tm_mon && current.tm_mday < dataofbirth.tm_mday))
            age--;
        return age;
    }

    void ShowUser() const {
        cout << "\n----------- USER LOGIN INFO -----------\n";
        cout << "Username: " << username << endl;
        cout << "Password: " << password << endl;
        cout << "Email: " << email << endl;
        cout << "----------- USER PERSONAL INFO -----------\n";
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Surname: " << surname << endl;
        cout << "Birthday: " << dataofbirth.tm_mday << "/" << dataofbirth.tm_mon + 1 << "/" << dataofbirth.tm_year + 1900 << endl;
        cout << "Gender: " << getGender() << endl;
        cout << "Age: " << getAge() << endl;
    }

    friend istream& operator>>(istream& in, User& right) {
        string id, username, password, email, name, surname, number, gender_str;
        int day, month, year;
        cout << "ID: "; in >> id;
        cout << "Name: "; in >> name;
        cout << "Surname: "; in >> surname;
        cout << "Username: "; in >> username;
        cout << "Password: "; in >> password;
        cout << "Email: "; in >> email;
        cout << "Phone (+994...): "; in >> number;
        cout << "Gender (Male/Female): "; in >> gender_str;
        cout << "Birthday (Day Month Year): ";
        in >> day >> month >> year;
        Gender g;
        if (gender_str == "Male" || gender_str == "male") g = Male;
        else if (gender_str == "Female" || gender_str == "female") g = Female;
        else throw string("Invalid gender!");
        right = User(id, username, password, email, name, surname, number, g, day, month, year);
        return in;
    }
};

class UserManager {
    vector<User> users;
    OrderManager& orderManager;
    string currentUserId;
    Admin* adminPtr = nullptr;
public:
    UserManager(OrderManager& om) : orderManager(om) {
        loadUserData(users);
    }

    void setAdmin(Admin* admin) {
        adminPtr = admin;
    }

    void UserPanel() {
        int choice;
        do {
            cout << "\n=== USER PANEL ===\n";
            cout << "1. Create Order\n";
            cout << "2. View Order Status\n";
            cout << "3. View Profile\n";
            cout << "0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            if (choice == 1) {
                if (orderManager.getDishCount() == 0) { cout << "Menu is empty.\n"; continue; }
                orderManager.showAllDishesWithIndex();
                int idx;
                cout << "Enter dish index: ";
                cin >> idx;
                try {
                    string dishName = orderManager.getDishNameByIndex(idx);
                    orderManager.createOrder(currentUserId, dishName);
                    cout << "Order created!\n";
                }
                catch (string ex) { cout << ex << endl; }
            }
            else if (choice == 2) {
                orderManager.showMyOrderStatus(currentUserId);
            }
            else if (choice == 3) {
                for (auto& u : users)
                    if (u.getId() == currentUserId)
                        u.ShowUser();
            }
        } while (choice != 0);
    }

    void signUp(const User& user) {
        for (auto& u : users) {
            if (u.getId() == user.getId()) throw string("ID already exists!");
            if (u.getEmail() == user.getEmail()) throw string("Email already exists!");
            if (u.getNumber() == user.getNumber()) throw string("Phone number already exists!");
        }
        users.push_back(user);
        saveUserData(users);
        cout << "User registered!\n";
    }

    void signIn(const string& username, const string& password) {
        string uname = username, pass = password;
        for (auto& c : uname) c = tolower(c);
        for (auto& c : pass) c = tolower(c);
        if (uname == "admin" && pass == "admin") {
            cout << "Logged in as Admin.\n";
            if (adminPtr) adminPtr->AdminPanel();
            else cout << "Admin instance not attached!\n";
            return;
        }
        for (auto& u : users) {
            if (u.getUserName() == username && u.getPassword() == password) {
                currentUserId = u.getId();
                cout << "Login successful!\n";
                UserPanel();
                return;
            }
        }
        cout << "Wrong username or password!\n";
    }

    void saveUserData(const vector<User>& users) {
        ofstream fs("User.txt");
        if (!fs.is_open()) throw string("Cannot open User.txt!");
        for (auto& u : users) {
            fs << u.getId() << "_"
                << u.getUserName() << "_"
                << u.getPassword() << "_"
                << u.getEmail() << "_"
                << u.getName() << "_"
                << u.getSurname() << "_"
                << u.getNumber() << "_"
                << u.getGender() << "_"
                << u.getDataOfBirth().tm_mday << "/"
                << u.getDataOfBirth().tm_mon + 1 << "/"
                << u.getDataOfBirth().tm_year + 1900 << "\n";
        }
        fs.close();
    }

    void loadUserData(vector<User>& users) {
        ifstream fs("User.txt");
        if (!fs.is_open()) {
            ofstream create("User.txt");
            create.close();
            return;
        }
        string row;
        while (getline(fs, row)) {
            if (row.empty()) continue;
            stringstream ss(row);
            string id, username, password, email, name, surname, number, genderStr, dateStr;
            getline(ss, id, '_');
            getline(ss, username, '_');
            getline(ss, password, '_');
            getline(ss, email, '_');
            getline(ss, name, '_');
            getline(ss, surname, '_');
            getline(ss, number, '_');
            getline(ss, genderStr, '_');
            getline(ss, dateStr);
            int day, month, year; char slash;
            stringstream d(dateStr);
            d >> day >> slash >> month >> slash >> year;
            Gender g = (genderStr == "Male" ? Male : Female);
            try {
                users.push_back(User(id, username, password, email, name, surname, number, g, day, month, year));
            }
            catch (...) {
                cout << "Skipped corrupted user row.\n";
            }
        }
    }
};

// ==== MAIN ====
int main() {
    OrderManager orderManager;
    Admin admin(orderManager);
    UserManager userManager(orderManager);
    userManager.setAdmin(&admin);

    while (true) {
        string choiceStr;
        int choice;
        cout << "\n1. Sign In\n2. Sign Up\n0. Exit\nChoice: ";
        cin >> choiceStr;
        try { choice = stoi(choiceStr); }
        catch (...) { cout << "Invalid input!\n"; continue; }
        if (choice == 1) {
            string u, p;
            cout << "Username: ";
            cin >> u;
            cout << "Password: ";
            cin >> p;
            userManager.signIn(u, p);
        }
        else if (choice == 2) {
            try {
                User u;
                cin >> u;
                userManager.signUp(u);
            }
            catch (string ex) {
                cout << ex << endl;
            }
        }
        else if (choice == 0) {
            cout << "Exiting program...\n";
            break;
        }
        else {
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}