#include "DataStorage.h"

DataStorage& DataStorage::getInstance() {
    static DataStorage instance;
    return instance;
}

void DataStorage::addUser(const User& user) {
    users.push_back(user);
}

User* DataStorage::findUserByName(const std::string& username) {
    for (auto& user : users) {
        if (user.getUsername() == username)
            return &user;
    }
    return nullptr;
}

std::vector<User>& DataStorage::getUsers() {
    return users;
}

void DataStorage::addCar(const Car& car) {
    cars.push_back(car);
}

std::vector<Car>& DataStorage::getCars() {
    return cars;
}
