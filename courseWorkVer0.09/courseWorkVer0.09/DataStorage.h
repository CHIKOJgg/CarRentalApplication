#pragma once
#include <vector>
#include <memory>
#include <string>
#include "User.h"
#include "Car.h"
#include <sqlite3.h>

class DataStorage {
private:
    sqlite3* db;
    DataStorage();
    void createTables();
public:
    static DataStorage& getInstance();
    ~DataStorage();

    bool addUser(const User& user);
    User* findUserByName(const std::string& username);
    void updateUser(const User& user);
    std::vector<User> getUsers();

    void addCar(const std::shared_ptr<Car>& car);
    std::vector<std::shared_ptr<Car>> getCars();
    void removeCar(const std::string& model);
    bool addRental(int userID, int carID,
        const std::string& startDate,
        const std::string& endDate);
    int getCarIDByModel(const std::string& model);
};
