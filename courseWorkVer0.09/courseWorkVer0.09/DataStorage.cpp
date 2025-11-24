#include "DataStorage.h"
#include <iostream>

DataStorage::DataStorage() {
    if (sqlite3_open("car_rental.db", &db)) {
        std::cerr << "Не удалось открыть БД: " << sqlite3_errmsg(db) << "\n";
        exit(1);
    }
    createTables();
}

void DataStorage::createTables() {
    const char* userTableSQL =
        "CREATE TABLE IF NOT EXISTS Users ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "hashedPassword TEXT NOT NULL, "
        "age INTEGER, experience REAL, rating INTEGER, "
        "isBlocked INTEGER, isAdmin INTEGER);";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, userTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка SQL (Users): " << errMsg << "\n";
        sqlite3_free(errMsg);
    }

    const char* carTableSQL =
        "CREATE TABLE IF NOT EXISTS Cars ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "type TEXT NOT NULL, model TEXT NOT NULL, "
        "basePrice REAL, minAge INTEGER);";
    if (sqlite3_exec(db, carTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка SQL (Cars): " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
    const char* rentalTableSQL =
        "CREATE TABLE IF NOT EXISTS Rentals ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userID INTEGER NOT NULL, "
        "carID INTEGER NOT NULL, "
        "startDate TEXT NOT NULL, "
        "endDate   TEXT NOT NULL, "
        "FOREIGN KEY(userID) REFERENCES Users(ID), "
        "FOREIGN KEY(carID)  REFERENCES Cars(ID)"
        ");";
    if (sqlite3_exec(db, rentalTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка SQL (Rentals): " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
}
bool DataStorage::addRental(int userID, int carID,
    const std::string& startDate,
    const std::string& endDate) {
    const char* sql =
        "INSERT INTO Rentals (userID,carID,startDate,endDate) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    sqlite3_bind_int(stmt, 1, userID);
    sqlite3_bind_int(stmt, 2, carID);
    sqlite3_bind_text(stmt, 3, startDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, endDate.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}


int DataStorage::getCarIDByModel(const std::string& model) {
    const char* sql = "SELECT ID FROM Cars WHERE model = ? LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса getCarIDByModel: "
            << sqlite3_errmsg(db) << "\n";
        return -1;
    }
    sqlite3_bind_text(stmt, 1, model.c_str(), -1, SQLITE_TRANSIENT);

    int carID = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        carID = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return carID;
}

DataStorage& DataStorage::getInstance() {
    static DataStorage instance;
    return instance;
}

DataStorage::~DataStorage() {
    sqlite3_close(db);
}

bool DataStorage::addUser(const User& user) {
    const char* sql =
        "INSERT INTO Users (username, hashedPassword, age, experience, rating, "
        "isBlocked, isAdmin) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (addUser): " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.hashedPassword.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, user.age);
    sqlite3_bind_double(stmt, 4, user.experience);
    sqlite3_bind_int(stmt, 5, user.rating);
    sqlite3_bind_int(stmt, 6, user.isBlocked ? 1 : 0);
    sqlite3_bind_int(stmt, 7, user.isAdmin ? 1 : 0);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

User* DataStorage::findUserByName(const std::string& username) {
    const char* sql =
        "SELECT username, hashedPassword, age, experience, rating, isBlocked, "
        "isAdmin FROM Users WHERE username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (findUserByName): " << sqlite3_errmsg(db) << "\n";
        return nullptr;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    User* user = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string hashed = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int age = sqlite3_column_int(stmt, 2);
        double exp = sqlite3_column_double(stmt, 3);
        int rating = sqlite3_column_int(stmt, 4);
        bool blocked = sqlite3_column_int(stmt, 5);
        bool admin = sqlite3_column_int(stmt, 6);
        user = new User(uname, "", age, exp, rating, blocked, admin);
        user->hashedPassword = hashed;
    }
    sqlite3_finalize(stmt);
    return user;
}

void DataStorage::updateUser(const User& user) {
    const char* sql =
        "UPDATE Users SET hashedPassword=?, age=?, experience=?, rating=?, "
        "isBlocked=?, isAdmin=? WHERE username=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (updateUser): " << sqlite3_errmsg(db) << "\n";
        return;
    }
    sqlite3_bind_text(stmt, 1, user.hashedPassword.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, user.age);
    sqlite3_bind_double(stmt, 3, user.experience);
    sqlite3_bind_int(stmt, 4, user.rating);
    sqlite3_bind_int(stmt, 5, user.isBlocked ? 1 : 0);
    sqlite3_bind_int(stmt, 6, user.isAdmin ? 1 : 0);
    sqlite3_bind_text(stmt, 7, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<User> DataStorage::getUsers() {
    std::vector<User> users;
    const char* sql =
        "SELECT username, hashedPassword, age, experience, rating, isBlocked, isAdmin FROM Users;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (getUsers): " << sqlite3_errmsg(db) << "\n";
        return users;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string hashed = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int age = sqlite3_column_int(stmt, 2);
        double exp = sqlite3_column_double(stmt, 3);
        int rating = sqlite3_column_int(stmt, 4);
        bool blocked = sqlite3_column_int(stmt, 5);
        bool admin = sqlite3_column_int(stmt, 6);
        User u(uname, "", age, exp, rating, blocked, admin);
        u.hashedPassword = hashed;
        users.push_back(u);
    }
    sqlite3_finalize(stmt);
    return users;
}

void DataStorage::addCar(const std::shared_ptr<Car>& car) {
    const char* sql =
        "INSERT INTO Cars (type, model, basePrice, minAge) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (addCar): " << sqlite3_errmsg(db) << "\n";
        return;
    }
    std::string type = car->getType();
    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, car->getModel().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, car->getBasePrice());
    sqlite3_bind_int(stmt, 4, car->getMinAge());
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<std::shared_ptr<Car>> DataStorage::getCars() {
    std::vector<std::shared_ptr<Car>> cars;
    const char* sql = "SELECT type, model, basePrice, minAge FROM Cars;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (getCars): " << sqlite3_errmsg(db) << "\n";
        return cars;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int minAge = sqlite3_column_int(stmt, 3);
        std::shared_ptr<Car> car;
        if (type == "Эконом")      car = std::make_shared<EconomyCar>(model, price);
        else if (type == "Комфорт") car = std::make_shared<ComfortCar>(model, price);
        else if (type == "Бизнес")  car = std::make_shared<BusinessCar>(model, price);
        else if (type == "Премиум") car = std::make_shared<PremiumCar>(model, price);
        if (car) cars.push_back(car);
    }
    sqlite3_finalize(stmt);
    return cars;
}

void DataStorage::removeCar(const std::string& model) {
    const char* sql = "DELETE FROM Cars WHERE model = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки (removeCar): " << sqlite3_errmsg(db) << "\n";
        return;
    }
    sqlite3_bind_text(stmt, 1, model.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}
