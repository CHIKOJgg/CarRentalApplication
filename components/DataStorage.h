#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "User.h"
#include "Car.h"
#include <vector>

class DataStorage {
public:
    // Реализация шаблона "синглтон"
    static DataStorage& getInstance();
    
    // Управление пользователями
    void addUser(const User& user);
    User* findUserByName(const std::string& username);
    std::vector<User>& getUsers();
    
    // Управление автомобилями
    void addCar(const Car& car);
    std::vector<Car>& getCars();
    
private:
    DataStorage() = default;
    std::vector<User> users;
    std::vector<Car> cars;
};

#endif // DATA_STORAGE_H
