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
    void deleteUser(); // ���� ����������
    void adminMenu(User* admin);
    void viewUsers();
    void blockUnblockUser();
    void manageCars();
    void viewCars();
    void addCar();
    void removeCar();
    void userMenu(User* user);
    void displayAvailableCars(const User& user);
    void rentCar(const User& user);
};
#pragma once
