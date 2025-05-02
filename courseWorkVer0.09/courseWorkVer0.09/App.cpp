#include "App.h"
#include "DataStorage.h"
#include "Utilities.h"
#include "InputChecker.h"
#include "termcolor.hpp"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <sstream>
#include"input_val.h"


using namespace std;

int showMenu(const std::string& title, const std::string options[],
    int optionCount) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO originalCursorInfo;
    GetConsoleCursorInfo(hConsole, &originalCursorInfo);

    CONSOLE_CURSOR_INFO cursorInfo = originalCursorInfo;
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    system("cls");
    std::cout << title << std::endl;
    while (_kbhit()) _getch();
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SHORT menuStartY = csbi.dwCursorPosition.Y;
    for (int i = 0; i < optionCount; ++i) {
        if (i == 0) {
            std::cout << termcolor::red << "--> " << termcolor::reset;
        }
        else {
            std::cout << "    ";
        }
        std::cout << options[i] << std::endl;
    }



    int choice = 0;
    int previousChoice = 0;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int consoleWidth = csbi.dwSize.X;
    //  TODO paint console output
    while (true) {
        if (choice != previousChoice) {

            SetConsoleCursorPosition(
                hConsole, { 0, static_cast<SHORT>(menuStartY + previousChoice) });
            std::cout << std::string(consoleWidth, ' ') << termcolor::grey << "\r    "
                << termcolor::reset << options[previousChoice];

            SetConsoleCursorPosition(
                hConsole, { 0, static_cast<SHORT>(menuStartY + choice) });
            std::cout << std::string(consoleWidth, ' ') << termcolor::red << "\r--> "
                << termcolor::reset << options[choice];

            previousChoice = choice;
        }

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72 && choice > 0)
                choice--;
            else if (key == 80 && choice < optionCount - 1)
                choice++;
        }
        else if (key == 13) { // Enter
            SetConsoleCursorInfo(hConsole, &originalCursorInfo);
            while (_kbhit()) _getch();
            return choice;
        }
    }
}


App::App() {
    auto& db = DataStorage::getInstance();
    auto cars = db.getCars();
    if (cars.empty()) {
        db.addCar(make_shared<EconomyCar>("Toyota Yaris", 30));
        db.addCar(make_shared<ComfortCar>("Honda Accord", 50));
        db.addCar(make_shared<BusinessCar>("BMW 5 Series", 80));
        db.addCar(make_shared<PremiumCar>("Mercedes S-Class", 120));
    }

    if (db.findUserByName("admin") == nullptr) {
        db.addUser(User("admin", "admin", 30, 10, 0, false, true));
    }
}


void App::run() {
    bool exitFlag = false;
    while (!exitFlag) {
        const string menu[] = { "�����������", "����", "�����" };
        int choice = showMenu("������� ������ �����������", menu, 3);
        switch (choice) {
        case 0: registerUser(); break;
        case 1: loginUser(); break;
        case 2: exitFlag = true; break;
        }
    }
}


void App::registerUser() {
    cin.ignore();
    string username, password, inputAge, inputExp;
    int age = 0;
    int exp = -1;

    cout << "\n=== ����������� ===\n";
  
    username = getValidatedInput<string>("������� ��� ������������:", isValidString, convertToString);

    if (DataStorage::getInstance().findUserByName(username)) {
        cout << "������������ � ����� ������ ��� ����������!\n";
        system("pause");
        return;
    }
    password = getValidatedInput<string>("������� ������ (������� 6 ��������):", isValidPassword, convertToString);
    
    age = getValidatedInput<int>("������� �������", isValidAdultAge, convertToInt);
     
   
    while (true) {
        exp = getValidatedInput<int>("������� ���� �������� (� �����)", isValidInt, convertToInt);
        if (age - exp >= 18) break;
        std::cerr << "���� ������ ���� ����������(�������: ��� ������� - 18)\n";
    }
            
    User user(username, password, age, exp);
    if (DataStorage::getInstance().addUser(user)) {
        cout << "������������ ������� ���������������!" << endl;
    }
    else {
        cout << "������ �����������." << endl;
    }

    system("pause");
}


void App::loginUser() {
    cin.ignore();
    string username, password;

    cout << "\n=== ���� ===\n";
    username = getValidatedInput<string>("������� ��� ������������:", isValidString, convertToString);

    password = getValidatedInput<string>("������� ������:", isValidString, convertToString);
    

    User* user = DataStorage::getInstance().findUserByName(username);
    if (user == nullptr || !user->checkPassword(password)) {
        cout << "�������� ��� ������������ ��� ������." << endl;
        system("pause");
        if (user)
            delete user;
        return;
    }

    if (user->isBlocked) {
        cout << "��� ������� ������������." << endl;
        system("pause");
        delete user;
        return;
    }

    if (user->isAdmin) {
        adminMenu(user);
    }
    else {
        userMenu(user);
    }

    delete user;
}
void App::userMenu(User* user) {
    bool exitUser = false;
    while (!exitUser) {
        const string userOptions[] = { "���������� ��������� ����������", "���������� ����������",
                                      "����� �� �������" };
        int choice = showMenu("������ ������������", userOptions, 3);
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
void App::adminMenu(User* admin) {
    bool exitAdmin = false;
    while (!exitAdmin) {
        const string adminOptions[] = { "�������� �������������", "�����������/�������������� ������������",
                                       "���������� ������������", "����� �� �������" };
        int choice = showMenu("������ ��������������", adminOptions, 4);
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
void App::addCar() {
    cin.ignore();
    const string carTypes[] = { "������", "�������", "������", "�������" };
    int typeChoice = showMenu("�������� ��� ���������� ��� ����������", carTypes, 4);
    string model;
    double price;
    model= getValidatedInput<string>("������� ������ ����������:", isValidString, convertToString);
    price = getValidatedInput<int>("������ ������� ���� :", isValidInt , convertToInt);
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
    cout << "���������� ������� ��������!" << endl;
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
}

void App::viewCars() {
    cout << "\n=== ��������� ���������� ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    int index = 0;
    for (auto& car : cars) {
        cout << index++ << ". ";
        car->displayInfo();
        cout << dashesManip << "\n";
    }
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
}
void App::removeCar() {
    viewCars();
    string model;
    model = getValidatedInput<string>("������� �������� ������ ����������, ������� ����� �������:", isValidString, convertToString);
    DataStorage::getInstance().removeCar(model);
    cout << "���������� ������� ������ (���� ������ ������������)." << endl;
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
}
void App::displayAvailableCars(const User& user) {
    cout << "\n=== Available Cars for You ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    vector<shared_ptr<Car>> availableCars;
    for (auto& car : cars) {
        if (user.age < car->getMinAge())
            continue;
        if (user.experience < 1.0 && car->getType() != "Economy")
            continue;
        availableCars.push_back(car);
    }
    if (availableCars.empty()) {
        cout << "��� �����, ������� �� �������� ��������� ��������/�����."
            << endl;
    }
    else {
        int idx = 0;
        for (auto& car : availableCars) {
            cout << idx++ << ". ";
            car->displayInfo();
            cout << "���� ��� ���: " << car->getPrice(user) << "\n";
            cout << "---------------------\n";
        }
    }
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
}
void App::rentCar(const User& user) {
    cout << "\n=== ���������� ������ ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    vector<shared_ptr<Car>> availableCars;
    for (auto& car : cars) {
        if (user.age < car->getMinAge())
            continue;
        if (user.experience < 1.0 && car->getType() != "������")
            continue;
        availableCars.push_back(car);
    }
    if (availableCars.empty()) {
        cout << "��� �����, ������� �� �������� ��������� ��������/�����."
            << endl;
        cout << "������� ����� �������, ����� ����������..." << endl;
        cin.get();
        return;
    }
    vector<string> carOptions;
    for (auto& car : availableCars) {
        stringstream ss;
        ss << car->getType() << " - " << car->getModel()
            << " - ����: " << car->getPrice(user);
        carOptions.push_back(ss.str());
    }
    int choice = showMenu("������� ���������� ��� ������", carOptions.data(),
        carOptions.size());
    cout << "\n�� ����������: " << availableCars[choice]->getType() << " - "
        << availableCars[choice]->getModel() << "\n";
    cout << "��������� ������: " << availableCars[choice]->getPrice(user)
        << "\n";
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
}
void App::viewUsers() {
    cout << "\n=== ������ �������������===\n";
    vector<User> users = DataStorage::getInstance().getUsers();
    for (auto& u : users) {
        cout << "��� ������������: " << u.username << " | �������: " << u.age
            << " | ����: " << u.experience << " ���"
            << " | ������������ ��: " << (u.isBlocked ? "��" : "���")
            << (u.isAdmin ? " | Admin" : "") << endl;
    }
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
}
void App::blockUnblockUser() {
    cin.ignore();
    string uname;
    uname = getValidatedInput<string>("������� ��� ������������ ��� ����������/ �������������:", isValidString, convertToString);
    User* user = DataStorage::getInstance().findUserByName(uname);
    if (user == nullptr) {
        cout << "������������ �� ������." << endl;
        return;
    }
    if (user->isAdmin) {
        cout << "���������� �������� ������� ������ ��������������." << endl;
        delete user;
        return;
    }
    user->isBlocked = !user->isBlocked;
    DataStorage::getInstance().updateUser(*user);
    cout << "������������ " << user->username
        << (user->isBlocked ? " ��� ������������."
            : " ��� �������������.")
        << endl;
    cout << "������� ����� �������, ����� ����������..." << endl;
    cin.get();
    delete user;
}
void App::manageCars() {
    bool exitCars = false;
    while (!exitCars) {
        const string carOptions[] = { "�������� �����������", "�������� ����������", "������� ����������",
                                     "�����" };
        int choice = showMenu("������ ��������� ������ �� ����", carOptions, 4);
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
