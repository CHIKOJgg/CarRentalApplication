#pragma once

#include "User.h"
#include "Car.h"
#include <memory>
#include <vector>
#include <string>

class App {
public:
    App();
    void run();

private:
    void registerUser();
    void loginUser();
    void deleteUser(); // если реализуешь
    void adminMenu(User* admin);
    void viewUsers();
    void filterByExperience();
    void filterByAge();
    void sortInAlphabetOrder();
    void blockUnblockUser();
    void manageCars();
    void viewCars();
    void addCar();
    void removeCar();
    void userMenu(User* user);
    void displayAvailableCars(const User& user);
    void rentCar(const User& user);
    int calculateDays(const std::string& start, const std::string& end) const;
    double calculateTotalPrice(const std::string& carType, double basePrice, int days) const;
};
