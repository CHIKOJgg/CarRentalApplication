#define _CRT_SECURE_NO_WARNINGS
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
#include <algorithm>


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
        const string menu[] = { "Регистрация", "Вход", "Выход" };
        int choice = showMenu("Система аренды автомобилей", menu, 3);
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

    cout << "\n=== Регистрация ===\n";

    username = getValidatedInput<string>("Введите имя пользователя:", isValidString, convertToString);

    if (DataStorage::getInstance().findUserByName(username)) {
        cout << "Пользователь с таким именем уже существует!\n";
        system("pause");
        return;
    }
    password = getValidatedInput<string>("Введите пароль (минимум 6 символов):", isValidPassword, convertToString);

    age = getValidatedInput<int>("Введите возраст", isValidAdultAge, convertToInt);


    while (true) {
        exp = getValidatedInput<int>("Введите стаж вождения (в годах)", isValidInt, convertToInt);
        if (age - exp >= 18) break;
        std::cerr << "Стаж должен быть корректным(минимум: ваш возраст - 18)\n";
    }

    User user(username, password, age, exp);
    if (DataStorage::getInstance().addUser(user)) {
        cout << "Пользователь успешно зарегистрирован!" << endl;

    }
    else {
        cout << "Ошибка регистрации. Соединение с базой данных разорвано" << endl;
    }

    system("pause");
}


void App::loginUser() {
    cin.ignore();
    string username, password;

    cout << "\n=== вход ===\n";
    username = getValidatedInput<string>("Введите имя пользователя:", isValidString, convertToString);

    password = getValidatedInput<string>("Введите пароль:", isValidString, convertToString);


    User* user = DataStorage::getInstance().findUserByName(username);
    if (user == nullptr || !user->checkPassword(password)) {
        cout << "Неверное имя пользователя или пароль." << endl;
        system("pause");
        if (user)
            delete user;
        return;
    }

    if (user->isBlocked) {
        cout << "Ваш аккаунт заблокирован." << endl;
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
        const string userOptions[] = { "Посмотреть доступные автомобили", "арендовать автомобиль",
                                      "выйти из системы"};
        int choice = showMenu("Панель пользователя", userOptions, 3);
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
        const string adminOptions[] = { "Просмотр пользователей", "Блокировать/Разблокировать пользователя",
                                       "Управление автомобилями","Отфильтровать пользователей по возрасту", "Отфильтровать пользователей по стажу","отсортировать пользоватеелй в алфавитном порядке", "выйти из системы"};
        int choice = showMenu("Панель администратора", adminOptions, 7);
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
           filterByAge();
            break;
        case 4:
           filterByExperience();
            break;
        case 5:
            sortInAlphabetOrder();
            break;
        case 6:
            exitAdmin = true;
            break;

        }
    }
}
void App::addCar() {
    cin.ignore();
    const string carTypes[] = { "Эконом", "Комфорт", "Бизнес", "Премиум" };
    int typeChoice = showMenu("Выберите тип автомобиля для добавления", carTypes, 4);
    string model;
    double price;
    model = getValidatedInput<string>("Введите модель автомобиля:", isValidString, convertToString);
    price = getValidatedInput<int>("Ввести базовую цену :", isValidInt, convertToInt);
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
    cout << "Автомобиль успешно добавлен!" << endl;
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
}
 
void App::viewCars() {
    cout << "\n=== Доступные автомобили ===\n";
    vector<shared_ptr<Car>> cars = DataStorage::getInstance().getCars();
    int index = 0;
    for (auto& car : cars) {
        cout << index++ << ". ";
        car->displayInfo();
        cout << dashesManip << "\n";
    }
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
}
void App::removeCar() {
    viewCars();
    string model;
    model = getValidatedInput<string>("Введите название модели автомобиля, который нужно удалить:", isValidString, convertToString);
    DataStorage::getInstance().removeCar(model);
    cout << "Автомобиль успешно удален (если модель существовала)." << endl;
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
}
void App::displayAvailableCars(const User& user) {
    cout << "\n=== Доступные для вас машины ===\n";
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
        cout << "Нет машин, которые бы отвечали критериям возраста/опыта."
            << endl;
    }
    else {
        int idx = 0;
        for (auto& car : availableCars) {
            cout << idx++ << ". ";
            car->displayInfo();
            cout << "Цена за день: " << car->getBasePrice() << "\n";
            cout << "Пример за 3 дня: "
            << calculateTotalPrice(car->getType(), car->getBasePrice(), 3) << "\n";
            cout << "---------------------\n";
        }
    }
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
}

string getCurrentDate() {
    time_t now = time(nullptr);
    tm* tm = localtime(&now);
    stringstream ss;
    ss << put_time(tm, "%Y-%m-%d");
    return ss.str();
}
void App::rentCar(const User& user) {
    cout << "\n=== Аренда автомобиля ===\n";

    auto allCars = DataStorage::getInstance().getCars();
    vector<shared_ptr<Car>> available;
    for (auto& car : allCars) {
        if (user.age < car->getMinAge()) continue;
        if (user.experience < 1.0 && car->getType() != "Эконом") continue;
        available.push_back(car);
    }
    if (available.empty()) {
        cout << "Нет машин, подходящих под ваши параметры.\n";
        cout << "Нажмите любую клавишу, чтобы вернуться...\n";
        cin.get();
        return;
    }
    vector<string> options;
    for (auto& car : available) {
        stringstream ss;
        ss << car->getType() << " - " << car->getModel()
            << " - Цена: " << car->getPrice(user);
        options.push_back(ss.str());
    }
    int choice = showMenu("Выберите автомобиль для аренды", options.data(), options.size());



    string currentDate = getCurrentDate();
    string startDate;
    bool validStart = false;
    do {
        cout << "\n\nВведите дату начала аренды (YYYY-MM-DD): ";
        getline(cin, startDate);
        if (!isValidDate(startDate)) {
            cout << "Дата некорректна, попробуйте снова\n";
        }
        else if (startDate < currentDate) {
            cout << "Дата начала аренды не может быть в прошлом.\n";
        }
        else {
            validStart = true;
        }
    } while (!validStart);

    string endDate;
    bool validEnd = false;
    do {
        cout << "\n\nВведите дату окончания аренды (YYYY-MM-DD): ";
        getline(cin, endDate);
        if (!isValidDate(endDate)) {
            cout << "Дата некорректна, попробуйте снова\n";
        }
        else if (startDate > endDate) {
            cout << "Дата начала аренды не может быть позже даты окончания.\n";
        }
        else {
            validEnd = true;
        }
    } while (!validEnd);
    int days = calculateDays(startDate, endDate);
    if (days < 1) {
        cout << "Минимальный срок аренды - 1 день!\n";
        system("pause");
        return;
    }
    int userID = user.getId();
    int carID = DataStorage::getInstance()
        .getCarIDByModel(available[choice]->getModel());
    bool saved = DataStorage::getInstance().addRental(userID, carID, startDate, endDate);
    double basePrice = available[choice]->getBasePrice();
    string carType = available[choice]->getType();
    double totalPrice = calculateTotalPrice(carType, basePrice, days);
    if (saved) {
        cout << "Аренда сохранена: " << startDate << " — " << endDate << "\n";
    }
    else {
        cout << "Ошибка при сохранении аренды.\n";
    }

    cout << "\nВы арендовали: "
        << available[choice]->getType() << " - "
        << available[choice]->getModel() << "\n"
        << "Стоимость аренды: " << totalPrice
        << " (" << days << " дней, "
        << "базовая цена: " << basePrice << "/день)\n";
    cout << "Нажмите любую клавишу, чтобы продолжить...\n";
    cin.get();
}

int App::calculateDays(const std::string& start, const std::string& end) const {
    std::tm startTm = {}, endTm = {};
    std::istringstream ssStart(start), ssEnd(end);
    ssStart >> std::get_time(&startTm, "%Y-%m-%d");
    ssEnd >> std::get_time(&endTm, "%Y-%m-%d");

    std::time_t startTime = std::mktime(&startTm);
    std::time_t endTime = std::mktime(&endTm);

    return static_cast<int>(std::difftime(endTime, startTime) / (60 * 60 * 24));
}

void App::viewUsers() {
    cout << "\n=== список пользователей===\n";
    vector<User> users = DataStorage::getInstance().getUsers();
    for (auto& u : users) {
        cout << "имя пользователя: " << u.username << " | возраст: " << u.age
            << " | Стаж: " << u.experience << " лет"
            << " | Заблокирован ли: " << (u.isBlocked ? "Да" : "Нет")
            << (u.isAdmin ? " | Admin" : "") << endl;
    }
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
}
void App::filterByAge() {
    int filterAge = getValidatedInput<int>("введите возраст", isValidInt, convertToInt);
    cout << "\n=== список пользователей===\n";
    vector<User> users = DataStorage::getInstance().getUsers();
    for (auto& u : users) {
        if (u.age>=filterAge)
        {
            cout << "имя пользователя: " << u.username << " | возраст: " << u.age
            << " | Стаж: " << u.experience << " лет"
            << " | Заблокирован ли: " << (u.isBlocked ? "Да" : "Нет")
            << (u.isAdmin ? " | Admin" : "") << endl;
        } {
           // cout << "пользователей с старше указанного возраста нет";
        }
        
    }
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();


}void App::sortInAlphabetOrder() {
   
    vector<User> users = DataStorage::getInstance().getUsers();
    sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.username < b.username;
        });

    cout << endl;
    for (const auto& u : users) {
    
            cout << "имя пользователя: " << u.username << " | возраст: " << u.age
            << " | Стаж: " << u.experience << " лет"
            << " | Заблокирован ли: " << (u.isBlocked ? "Да" : "Нет")
            << (u.isAdmin ? " | Admin" : "") << endl;
        
        
    }
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();


}

double App::calculateTotalPrice(const string& carType, double basePrice, int days)const{
    if (days <= 0) {
        return 0.0;
       }

    double multiplier =  1.0;
    if (carType ==  "Эконом")
    {
        if (days > 14) multiplier = 0.75;    // 25
        else if (days > 7) multiplier = 0.85;// 15
        else if (days > 3) multiplier = 0.90;//
    }
    else if (carType == "Комфорт")
    {
        if (days > 14) multiplier = 0.80;
        else if (days > 7) multiplier = 0.90;
    }
    else if (carType == "Бизнес")
    {
        if (days > 30) multiplier = 0.70;
        else if (days > 14) multiplier = 0.85;
    }
    else if (carType == "Премиум")
    {
        multiplier = 1.0 - (0.01 * days); // 1% скидка за каждый день
        if (multiplier < 0.7) multiplier = 0.7; // Максимум 30% скидки

    }
    return basePrice * days * multiplier;
}

void App::filterByExperience() {
    int filterExp = getValidatedInput<int>("\nвведите cnf;:", isValidInt, convertToInt);
    cout << "\n=== список пользователей===\n";
    vector<User> users = DataStorage::getInstance().getUsers();
    for (auto& u : users) {
        if (u.experience >= filterExp)
        {
            cout << "имя пользователя: " << u.username << " | возраст: " << u.age
                << " | Стаж: " << u.experience << " лет"
                << " | Заблокирован ли: " << (u.isBlocked ? "Да" : "Нет")
                << (u.isAdmin ? " | Admin" : "") << endl;
        } {
            //cout << " нет пользователей со стажем больше указанного";
        }

    }
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
}
void App::blockUnblockUser() {
    cin.ignore();
    string uname;
    uname = getValidatedInput<string>("\n\nВведите имя пользователя для блокировки/разблокировки:", isValidString, convertToString);
    User* user = DataStorage::getInstance().findUserByName(uname);
    if (user == nullptr) {
        cout << "Пользователь не найден." << endl;
        system("pause");
        return;
    }
    if (user->isAdmin) {
        cout << "Невозможно изменить учетную запись администратора." << endl;
        system("pause");
        delete user;
        return;
    }
    user->isBlocked = !user->isBlocked;
    DataStorage::getInstance().updateUser(*user);
    cout << "Пользователь " << user->username
        << (user->isBlocked ? " был заблокирован."
            : " был разблокирован.")
        << endl;
    cout << "Нажмите любую клавишу, чтобы продолжить..." << endl;
    cin.get();
    delete user;
}
void App::manageCars() {
    bool exitCars = false;
    while (!exitCars) {
        const string carOptions[] = { "Просмотр автомобилей", "Добавить автомобиль", "Удалить автомобиль",
                                     "назад" };
        int choice = showMenu("Панель изменения данных об авто", carOptions, 4);
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
