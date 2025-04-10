#include "sqlite3.h"
#include <conio.h>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Функция для показа меню со стрелками
int showMenu(const std::string &title, const std::string options[],
             int optionCount) {
  int choice = 0;
  while (true) {
    system("cls");
    cout << title << endl;
    for (int i = 0; i < optionCount; ++i) {
      if (i == choice) {
        cout << "--> " << options[i] << endl;
      } else {
        cout << "    " << options[i] << endl;
      }
    }
    int key = _getch();
    if (key == 224) {
      key = _getch();
      if (key == 72 && choice > 0) {
        choice--;
      } else if (key == 80 && choice < optionCount - 1) {
        choice++;
      }
    } else if (key == 13) { // Enter
      return choice;
    }
  }
}

// Функция хеширования пароля с солью
string hashPassword(const string &password) {
  const string salt = "random_salt"; // можно заменить на что-то более случайное
  string salted = password + salt;
  size_t hash_val = std::hash<string>{}(salted);
  stringstream ss;
  ss << hex << hash_val;
  return ss.str();
}

// ====================== Пользователь ======================
class User {
public:
  string username;
  string hashedPassword;
  int age;
  double experience; // в годах
  int rating;
  bool isBlocked;
  bool isAdmin;

  User()
      : age(0), experience(0.0), rating(0), isBlocked(false), isAdmin(false) {}

  User(const string &uname, const string &pass, int age, double exp,
       int rating = 0, bool blocked = false, bool isAdmin = false)
      : username(uname), hashedPassword(hashPassword(pass)), age(age),
        experience(exp), rating(rating), isBlocked(blocked), isAdmin(isAdmin) {}

  bool checkPassword(const string &pass) const {
    return hashedPassword == hashPassword(pass);
  }
};

// ====================== Автомобили ======================
// Абстрактный базовый класс автомобиля
class Car {
protected:
  string model;
  double basePrice;
  int minAge;

public:
  Car(const string &model, double basePrice, int minAge)
      : model(model), basePrice(basePrice), minAge(minAge) {}
  virtual ~Car() {}
  virtual string getType() const = 0;
  // По умолчанию цена равна базовой цене
  virtual double getPrice(const User &user) const { return basePrice; }
  virtual void displayInfo() const {
    cout << "Type: " << getType() << "\nModel: " << model
         << "\nBase Price: " << basePrice << "\nMinimum Age: " << minAge
         << "\n";
  }
  int getMinAge() const { return minAge; }
  string getModel() const { return model; }
  double getBasePrice() const { return basePrice; }
};

class EconomyCar : public Car {
public:
  EconomyCar(const string &model, double basePrice)
      : Car(model, basePrice, 18) {}
  virtual string getType() const override { return "Economy"; }
  virtual double getPrice(const User &user) const override {
    if (user.experience < 1.0)
      return basePrice * 1.1;
    return basePrice;
  }
};

class ComfortCar : public Car {
public:
  ComfortCar(const string &model, double basePrice)
      : Car(model, basePrice, 21) {}
  virtual string getType() const override { return "Comfort"; }
};

class BusinessCar : public Car {
public:
  BusinessCar(const string &model, double basePrice)
      : Car(model, basePrice, 21) {}
  virtual string getType() const override { return "Business"; }
};

class PremiumCar : public Car {
public:
  PremiumCar(const string &model, double basePrice)
      : Car(model, basePrice, 25) {}
  virtual string getType() const override { return "Premium"; }
};

// ====================== DataStorage (с использованием SQLite)
// ======================
class DataStorage {
private:
  sqlite3 *db;
  DataStorage() {
    int rc = sqlite3_open("car_rental.db", &db);
    if (rc) {
      cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
      exit(1);
    }
    createTables();
  }
  void createTables() {
    const char *userTableSQL = "CREATE TABLE IF NOT EXISTS Users ("
                               "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "username TEXT UNIQUE NOT NULL, "
                               "hashedPassword TEXT NOT NULL, "
                               "age INTEGER, "
                               "experience REAL, "
                               "rating INTEGER, "
                               "isBlocked INTEGER, "
                               "isAdmin INTEGER);";
    char *errMsg = 0;
    int rc = sqlite3_exec(db, userTableSQL, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      cerr << "SQL error (Users): " << errMsg << endl;
      sqlite3_free(errMsg);
    }
    const char *carTableSQL = "CREATE TABLE IF NOT EXISTS Cars ("
                              "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "type TEXT NOT NULL, "
                              "model TEXT NOT NULL, "
                              "basePrice REAL, "
                              "minAge INTEGER);";
    rc = sqlite3_exec(db, carTableSQL, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
      cerr << "SQL error (Cars): " << errMsg << endl;
      sqlite3_free(errMsg);
    }
  }

public:
  static DataStorage &getInstance() {
    static DataStorage instance;
    return instance;
  }
  ~DataStorage() { sqlite3_close(db); }
  // Пользователи
  void addUser(const User &user) {
    const char *sql =
        "INSERT INTO Users (username, hashedPassword, age, experience, rating, "
        "isBlocked, isAdmin) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing statement (addUser): " << sqlite3_errmsg(db)
           << endl;
      return;
    }
    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.hashedPassword.c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, user.age);
    sqlite3_bind_double(stmt, 4, user.experience);
    sqlite3_bind_int(stmt, 5, user.rating);
    sqlite3_bind_int(stmt, 6, user.isBlocked ? 1 : 0);
    sqlite3_bind_int(stmt, 7, user.isAdmin ? 1 : 0);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
      cerr << "Error inserting user: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
  }
  // Поиск пользователя по имени
  User *findUserByName(const string &username) {
    const char *sql =
        "SELECT username, hashedPassword, age, experience, rating, isBlocked, "
        "isAdmin FROM Users WHERE username = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing statement (findUserByName): "
           << sqlite3_errmsg(db) << endl;
      return nullptr;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    User *user = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      string uname =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
      string hashedPass =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      int age = sqlite3_column_int(stmt, 2);
      double experience = sqlite3_column_double(stmt, 3);
      int rating = sqlite3_column_int(stmt, 4);
      bool isBlocked = sqlite3_column_int(stmt, 5) != 0;
      bool isAdmin = sqlite3_column_int(stmt, 6) != 0;
      user = new User(uname, "", age, experience, rating, isBlocked, isAdmin);
      user->hashedPassword = hashedPass;
    }
    sqlite3_finalize(stmt);
    return user;
  }
  void updateUser(const User &user) {
    const char *sql =
        "UPDATE Users SET hashedPassword = ?, age = ?, experience = ?, rating "
        "= ?, isBlocked = ?, isAdmin = ? WHERE username = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing update statement: " << sqlite3_errmsg(db)
           << endl;
      return;
    }
    sqlite3_bind_text(stmt, 1, user.hashedPassword.c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, user.age);
    sqlite3_bind_double(stmt, 3, user.experience);
    sqlite3_bind_int(stmt, 4, user.rating);
    sqlite3_bind_int(stmt, 5, user.isBlocked ? 1 : 0);
    sqlite3_bind_int(stmt, 6, user.isAdmin ? 1 : 0);
    sqlite3_bind_text(stmt, 7, user.username.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
      cerr << "Error updating user: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
  }
  vector<User> getUsers() {
    vector<User> users;
    const char *sql = "SELECT username, hashedPassword, age, experience, "
                      "rating, isBlocked, isAdmin FROM Users;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing statement (getUsers): " << sqlite3_errmsg(db)
           << endl;
      return users;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      string uname =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
      string hashedPass =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      int age = sqlite3_column_int(stmt, 2);
      double experience = sqlite3_column_double(stmt, 3);
      int rating = sqlite3_column_int(stmt, 4);
      bool isBlocked = sqlite3_column_int(stmt, 5) != 0;
      bool isAdmin = sqlite3_column_int(stmt, 6) != 0;
      User u(uname, "", age, experience, rating, isBlocked, isAdmin);
      u.hashedPassword = hashedPass;
      users.push_back(u);
    }
    sqlite3_finalize(stmt);
    return users;
  }
  // Автомобили
  void addCar(const shared_ptr<Car> &car) {
    const char *sql = "INSERT INTO Cars (type, model, basePrice, minAge) "
                      "VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing statement (addCar): " << sqlite3_errmsg(db)
           << endl;
      return;
    }
    string type = car->getType();
    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car->getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, car->getBasePrice());
    sqlite3_bind_int(stmt, 4, car->getMinAge());
    if (sqlite3_step(stmt) != SQLITE_DONE) {
      cerr << "Error inserting car: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
  }
  vector<shared_ptr<Car>> getCars() {
    vector<shared_ptr<Car>> cars;
    const char *sql = "SELECT type, model, basePrice, minAge FROM Cars;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing statement (getCars): " << sqlite3_errmsg(db)
           << endl;
      return cars;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      string type =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
      string model =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      double basePrice = sqlite3_column_double(stmt, 2);
      int minAge = sqlite3_column_int(stmt, 3);
      shared_ptr<Car> car;
      if (type == "Economy") {
        car = make_shared<EconomyCar>(model, basePrice);
      } else if (type == "Comfort") {
        car = make_shared<ComfortCar>(model, basePrice);
      } else if (type == "Business") {
        car = make_shared<BusinessCar>(model, basePrice);
      } else if (type == "Premium") {
        car = make_shared<PremiumCar>(model, basePrice);
      }
      if (car) {
        cars.push_back(car);
      }
    }
    sqlite3_finalize(stmt);
    return cars;
  }
  void removeCar(const string &model) {
    const char *sql = "DELETE FROM Cars WHERE model = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
      cerr << "Error preparing statement (removeCar): " << sqlite3_errmsg(db)
           << endl;
      return;
    }
    sqlite3_bind_text(stmt, 1, model.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
      cerr << "Error deleting car: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
  }
};

// ====================== Основное приложение ======================
class App {
public:
  App() {
    // Если в базе ещё нет автомобилей, добавляем их
    vector<shared_ptr<Car>> existingCars = DataStorage::getInstance().getCars();
    if (existingCars.empty()) {
      DataStorage::getInstance().addCar(
          make_shared<EconomyCar>("Toyota Yaris", 30.0));
      DataStorage::getInstance().addCar(
          make_shared<ComfortCar>("Honda Accord", 50.0));
      DataStorage::getInstance().addCar(
          make_shared<BusinessCar>("BMW 5 Series", 80.0));
      DataStorage::getInstance().addCar(
          make_shared<PremiumCar>("Mercedes S-Class", 120.0));
    }
    // Создаем учетную запись администратора, если её ещё нет
    User *admin = DataStorage::getInstance().findUserByName("admin");
    if (admin == nullptr) {
      User adminUser("admin", "admin", 30, 5.0, 0, false, true);
      DataStorage::getInstance().addUser(adminUser);
    } else {
      delete admin;
    }
  }

  void run() {
    bool exitFlag = false;
    while (!exitFlag) {
      const string mainMenuOptions[] = {"Register", "Login", "Exit"};
      int choice = showMenu("Car Rental System", mainMenuOptions, 3);
      switch (choice) {
      case 0:
        registerUser();
        break;
      case 1:
        loginUser();
        break;
      case 2:
        exitFlag = true;
        break;
      }
    }
  }

  void registerUser() {
    cin.ignore();
    string username, password;
    int age;
    double experience;
    cout << "\n=== User Registration ===\n";
    cout << "Enter username: ";
    getline(cin, username);
    if (DataStorage::getInstance().findUserByName(username) != nullptr) {
      cout << "Username already exists.\n";
      return;
    }
    cout << "Enter password: ";
    getline(cin, password);
    cout << "Enter age: ";
    cin >> age;
    cin.ignore();
    cout << "Enter driving experience (in years): ";
    cin >> experience;
    cin.ignore();
    if (age < 18) {
      cout << "Sorry, you must be at least 18 years old to register." << endl;
      return;
    }
    User newUser(username, password, age, experience);
    DataStorage::getInstance().addUser(newUser);
    cout << "Registration successful!" << endl;
  }

  void loginUser() {
    cin.ignore();
    string username, password;
    cout << "\n=== Login ===\n";
    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);
    User *user = DataStorage::getInstance().findUserByName(username);
    if (user == nullptr || !user->checkPassword(password)) {
      cout << "Invalid username or password." << endl;
      if (user)
        delete user;
      return;
    }
    if (user->isBlocked) {
      cout << "Your account is blocked." << endl;
      delete user;
      return;
    }
    if (user->isAdmin) {
      adminMenu(user);
    } else {
      userMenu(user);
    }
    delete user;
  }

  void adminMenu(User *admin) {
    bool exitAdmin = false;
    while (!exitAdmin) {
      const string adminOptions[] = {"View Users", "Block/Unblock User",
                                     "Manage Cars", "Logout"};
      int choice = showMenu("Admin Panel", adminOptions, 4);
      switch (choice) {
      case 0:
        viewUsers();
        break;
      case 1:
        blockUnblockUser();
        break;
      case 2:
        manageCars();
        break;
      case 3:
        exitAdmin = true;
        break;
      }
    }
  }

  void viewUsers() {
    cout << "\n=== List of Users ===\n";
    vector<User> users = DataStorage::getInstance().getUsers();
    for (auto &u : users) {
      cout << "Username: " << u.username << " | Age: " << u.age
           << " | Experience: " << u.experience << " years"
           << " | Blocked: " << (u.isBlocked ? "Yes" : "No")
           << (u.isAdmin ? " | Admin" : "") << endl;
    }
    cout << "Press any key to continue..." << endl;
    cin.get();
  }

  void blockUnblockUser() {
    cin.ignore();
    cout << "\nEnter username to block/unblock: ";
    string uname;
    getline(cin, uname);
    User *user = DataStorage::getInstance().findUserByName(uname);
    if (user == nullptr) {
      cout << "User not found." << endl;
      return;
    }
    if (user->isAdmin) {
      cout << "Cannot modify admin account." << endl;
      delete user;
      return;
    }
    user->isBlocked = !user->isBlocked;
    DataStorage::getInstance().updateUser(*user);
    cout << "User " << user->username
         << (user->isBlocked ? " has been blocked." : " has been unblocked.")
         << endl;
    cout << "Press any key to continue..." << endl;
    cin.get();
    delete user;
  }

  void manageCars() {
    bool exitCars = false;
    while (!exitCars) {
      const string carOptions[] = {"View Cars", "Add Car", "Remove Car",
                                   "Back"};
      int choice = showMenu("Manage Cars", carOptions, 4);
      switch (choice) {
      case 0:
        viewCars();
        break;
      case 1:
        addCar();
        break;
      case 2:
        removeCar();
        break;
      case 3:
        exitCars = true;
        break;
      }
    }
  }

  void viewCars() {
    cout << "\n=== Available Cars ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    int index = 0;
    for (auto &car : cars) {
      cout << index++ << ". ";
      car->displayInfo();
      cout << "---------------------\n";
    }
    cout << "Press any key to continue..." << endl;
    cin.get();
  }

  void addCar() {
    cin.ignore();
    const string carTypes[] = {"Economy", "Comfort", "Business", "Premium"};
    int typeChoice = showMenu("Select Car Type to Add", carTypes, 4);
    string model;
    double price;
    cout << "Enter car model: ";
    getline(cin, model);
    cout << "Enter base price: ";
    cin >> price;
    cin.ignore();
    shared_ptr<Car> newCar;
    switch (typeChoice) {
    case 0:
      newCar = make_shared<EconomyCar>(model, price);
      break;
    case 1:
      newCar = make_shared<ComfortCar>(model, price);
      break;
    case 2:
      newCar = make_shared<BusinessCar>(model, price);
      break;
    case 3:
      newCar = make_shared<PremiumCar>(model, price);
      break;
    }
    DataStorage::getInstance().addCar(newCar);
    cout << "Car added successfully!" << endl;
    cout << "Press any key to continue..." << endl;
    cin.get();
  }

  void removeCar() {
    viewCars();
    cout << "Enter the model name of the car to remove: ";
    string model;
    getline(cin, model);
    DataStorage::getInstance().removeCar(model);
    cout << "Car removed successfully (if model existed)." << endl;
    cout << "Press any key to continue..." << endl;
    cin.get();
  }

  void userMenu(User *user) {
    bool exitUser = false;
    while (!exitUser) {
      const string userOptions[] = {"View Available Cars", "Rent a Car",
                                    "Logout"};
      int choice = showMenu("User Menu", userOptions, 3);
      switch (choice) {
      case 0:
        displayAvailableCars(*user);
        break;
      case 1:
        rentCar(*user);
        break;
      case 2:
        exitUser = true;
        break;
      }
    }
  }

  void displayAvailableCars(const User &user) {
    cout << "\n=== Available Cars for You ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    vector<shared_ptr<Car>> availableCars;
    for (auto &car : cars) {
      if (user.age < car->getMinAge())
        continue;
      if (user.experience < 1.0 && car->getType() != "Economy")
        continue;
      availableCars.push_back(car);
    }
    if (availableCars.empty()) {
      cout << "No cars available based on your age/experience criteria."
           << endl;
    } else {
      int idx = 0;
      for (auto &car : availableCars) {
        cout << idx++ << ". ";
        car->displayInfo();
        cout << "Rental Price for you: " << car->getPrice(user) << "\n";
        cout << "---------------------\n";
      }
    }
    cout << "Press any key to continue..." << endl;
    cin.get();
  }

  void rentCar(const User &user) {
    cout << "\n=== Rent a Car ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    vector<shared_ptr<Car>> availableCars;
    for (auto &car : cars) {
      if (user.age < car->getMinAge())
        continue;
      if (user.experience < 1.0 && car->getType() != "Economy")
        continue;
      availableCars.push_back(car);
    }
    if (availableCars.empty()) {
      cout << "No cars available for rental based on your criteria." << endl;
      cout << "Press any key to continue..." << endl;
      cin.get();
      return;
    }
    vector<string> carOptions;
    for (auto &car : availableCars) {
      stringstream ss;
      ss << car->getType() << " - " << car->getModel()
         << " - Price: " << car->getPrice(user);
      carOptions.push_back(ss.str());
    }
    int choice =
        showMenu("Select a car to rent", carOptions.data(), carOptions.size());
    cout << "You have rented: " << availableCars[choice]->getType() << " - "
         << availableCars[choice]->getModel() << "\n";
    cout << "Rental price: " << availableCars[choice]->getPrice(user) << "\n";
    cout << "Press any key to continue..." << endl;
    cin.get();
  }
};

int main() {
  App app;
  app.run();
  return 0;
}
