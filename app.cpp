#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
// ====================== InputValidator ======================
bool isIntCorrect(const string &input) {
  if (input.empty())
    return false;
  for (char c : input) {
    if (!isdigit(static_cast<unsigned char>(c)))
      return false;
  }
  return true;
}

bool isStringCorrect(const string &input) {
  if (input.empty())
    return false;
  for (char c : input) {
    if (!isspace(static_cast<unsigned char>(c)))
      return true;
  }
  return false;
}

// ====================== User ======================
class User {
private:
  string username;
  string password;
  int rating;
  bool isBlocked;
  bool isAdmin; // Flag for administrator

public:
  User() : rating(0), isBlocked(false), isAdmin(false) {}
  User(const string &username, const string &password, int rating = 0,
       bool isBlocked = false, bool isAdmin = false)
      : username(username), password(password), rating(rating),
        isBlocked(isBlocked), isAdmin(isAdmin) {}

  // User registration function (with input validation)
  void registerUser() {
    string uname, pass;
    cout << "\nUser Registration:\n";
    cout << "Enter username: ";
    getline(cin, uname);
    while (!isStringCorrect(uname)) {
      cout << "Invalid input. Please try again: ";
      getline(cin, uname);
    }

    cout << "Enter password: ";
    getline(cin, pass);
    while (pass.empty()) {
      cout << "Password cannot be empty. Please try again: ";
      getline(cin, pass);
    }

    username = uname;
    password = pass;

    // If the username is "admin", mark the user as an administrator
    if (username == "admin") {
      isAdmin = true;
    }

    cout << "User " << username << " successfully registered!\n";
  }

  // Check if the entered password matches the stored one
  bool checkPassword(const string &pass) const { return password == pass; }

  const string &getUsername() const { return username; }
  bool getIsBlocked() const { return isBlocked; }
  bool getIsAdmin() const { return isAdmin; }

  // Administration functions
  void blockUser() { isBlocked = true; }
  void unblockUser() { isBlocked = false; }
};

// ====================== Car ======================
class Car {
private:
  string model;
  double price;
  int requiredExperience; // Required experience (e.g., in years)
  string tariff;          // Tariff plan

public:
  Car() : price(0.0), requiredExperience(0) {}
  Car(const string &model, double price, int requiredExperience,
      const string &tariff)
      : model(model), price(price), requiredExperience(requiredExperience),
        tariff(tariff) {}

  const string &getModel() const { return model; }
  double getPrice() const { return price; }
  int getRequiredExperience() const { return requiredExperience; }
  const string &getTariff() const { return tariff; }

  void display() const {
    cout << "Model: " << model << "\nPrice: " << price
         << "\nRequired Experience: " << requiredExperience
         << "\nTariff: " << tariff << "\n";
  }
};

// ====================== DataStorage (Singleton) ======================
class DataStorage {
public:
  static DataStorage &getInstance() {
    static DataStorage instance;
    return instance;
  }

  // User management
  void addUser(const User &user) { users.push_back(user); }

  User *findUserByName(const string &username) {
    for (auto &user : users) {
      if (user.getUsername() == username)
        return &user;
    }
    return nullptr;
  }

  vector<User> &getUsers() { return users; }

  // Car management
  void addCar(const Car &car) { cars.push_back(car); }

  vector<Car> &getCars() { return cars; }

private:
  DataStorage() = default;
  vector<User> users;
  vector<Car> cars;
};

// ====================== App ======================

class App {

public:
  App() {
    // Preload some cars for demonstration
    DataStorage::getInstance().addCar(Car("Toyota Camry", 50.0, 2, "Standard"));
    DataStorage::getInstance().addCar(Car("BMW 5 Series", 100.0, 5, "Premium"));
  }

  void run() {
    bool exitFlag = false;
    while (!exitFlag) {
      cout << "\nWelcome to the Car Rental System!\n";
      cout << "1. Register\n";
      cout << "2. Login\n";
      cout << "3. Exit\n";
      cout << "Choose an option: ";

      string choice;
      getline(cin, choice);

      if (!isIntCorrect(choice)) {
        cout << "Invalid input, please try again.\n";
        continue;
      }

      int intChoice = stoi(choice);
      switch (intChoice) {
      case 1: {
        // Register a new user
        User newUser;
        newUser.registerUser();
        DataStorage::getInstance().addUser(newUser);
        break;
      }
      case 2: {
        // Login to the system
        string uname, pass;
        cout << "Login:\n";
        cout << "Enter username: ";
        getline(cin, uname);
        cout << "Enter password: ";
        getline(cin, pass);

        DataStorage &ds = DataStorage::getInstance();
        User *user = ds.findUserByName(uname);
        if (user == nullptr) {
          cout << "User not found.\n";
          break;
        }
        if (!user->checkPassword(pass)) {
          cout << "Incorrect password.\n";
          break;
        }
        if (user->getIsBlocked()) {
          cout << "Your account is blocked.\n";
          break;
        }

        // If the logged in user is an administrator (username "admin")
        if (user->getIsAdmin()) {
          bool adminLogged = true;
          while (adminLogged) {
            cout << "\nAdmin Panel:\n";
            cout << "1. View all users\n";
            cout << "2. Block user\n";
            cout << "3. Unblock user\n";
            cout << "4. View rating\n";
            cout << "5. Exit\n";
            cout << "Choose an option: ";

            string adminChoice;
            getline(cin, adminChoice);
            if (!isIntCorrect(adminChoice)) {
              cout << "Invalid input, please try again.\n";
              continue;
            }
            int intAdminChoice = stoi(adminChoice);
            switch (intAdminChoice) {
            case 1: {
              cout << "\nList of Users:\n";
              for (auto &u : ds.getUsers()) {
                cout << "User: " << u.getUsername()
                     << " | Blocked: " << (u.getIsBlocked() ? "Yes" : "No")
                     << "\n";
              }
              break;
            }
            case 2: {
              cout << "Enter username to block: ";
              string blockName;
              getline(cin, blockName);
              User *userToBlock = ds.findUserByName(blockName);
              if (userToBlock != nullptr) {
                userToBlock->blockUser();
                cout << "User " << blockName << " has been blocked.\n";
              } else {
                cout << "User not found.\n";
              }
              break;
            }
            case 3: {
              cout << "Enter username to unblock: ";
              string unblockName;
              getline(cin, unblockName);
              User *userToUnblock = ds.findUserByName(unblockName);
              if (userToUnblock != nullptr) {
                userToUnblock->unblockUser();
                cout << "User " << unblockName << " has been unblocked.\n";
              } else {
                cout << "User not found.\n";
              }
              break;
            }
            case 4:
              cout << "user rating";
              break;
            case 5:
              adminLogged = false;
              break;
            default:
              cout << "Invalid choice, please try again.\n";
            }
          }
        } else {
          // Regular user menu
          bool userLogged = true;
          while (userLogged) {
            cout << "\nUser Menu:\n";
            cout << "1. View rented cars\n";
            cout << "2. Rent a car\n";
            cout << "3. Exit\n";
            cout << "Choose an option: ";

            string userChoice;
            getline(cin, userChoice);
            if (!isIntCorrect(userChoice)) {
              cout << "Invalid input, please try again.\n";
              continue;
            }
            int intUserChoice = stoi(userChoice);
            switch (intUserChoice) {
            case 1: {
              cout << "\nList of rented cars (temporary data, functionality "
                      "under development).\n";
              break;
            }
            case 2:
              // TODO rent car fuction;

              break;
            case 3:
              userLogged = false;
              break;
            default:
              cout << "Invalid choice, please try again.\n";
            }
          }
        }
        break;
      }
      case 3:
        exitFlag = true;
        break;
      default:
        cout << "Invalid choice, please try again.\n";
      }
    }
  }
};