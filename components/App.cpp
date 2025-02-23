#include "App.h"
#include "User.h"
#include "Car.h"
#include "DataStorage.h"
#include "InputValidator.h"
#include <iostream>
#include <string>
using namespace std;

App::App() {
    // Предзагрузка некоторых автомобилей для демонстрации
    DataStorage::getInstance().addCar(Car("Toyota Camry", 50.0, 2, "Стандарт"));
    DataStorage::getInstance().addCar(Car("BMW 5 Series", 100.0, 5, "Премиум"));
}

void App::run() {
    bool exitFlag = false;
    while (!exitFlag) {
        cout << "\nДобро пожаловать в систему проката автомобилей!\n";
        cout << "1. Регистрация\n";
        cout << "2. Вход\n";
        cout << "3. Выход\n";
        cout << "Выберите действие: ";
        
        string choice;
        getline(cin, choice);
        
        if (!isIntCorrect(choice)) {
            cout << "Неверный ввод, попробуйте снова.\n";
            continue;
        }
        
        int intChoice = stoi(choice);
        switch (intChoice) {
            case 1: {
                // Регистрация нового пользователя
                User newUser;
                newUser.registerUser();
                // Сохраняем пользователя во временное хранилище
                DataStorage::getInstance().addUser(newUser);
                break;
            }
            case 2: {
                // Вход в систему
                string uname, pass;
                cout << "Вход в систему:\n";
                cout << "Введите имя пользователя: ";
                getline(cin, uname);
                cout << "Введите пароль: ";
                getline(cin, pass);
                
                DataStorage& ds = DataStorage::getInstance();
                User* user = ds.findUserByName(uname);
                if(user == nullptr) {
                    cout << "Пользователь не найден.\n";
                    break;
                }
                if(!user->checkPassword(pass)) {
                    cout << "Неверный пароль.\n";
                    break;
                }
                if(user->getIsBlocked()) {
                    cout << "Ваш аккаунт заблокирован.\n";
                    break;
                }
                
                // Если вошёл администратор (имя "admin")
                if(user->getIsAdmin()) {
                    bool adminLogged = true;
                    while(adminLogged) {
                        cout << "\nАдмин-панель:\n";
                        cout << "1. Просмотр всех пользователей\n";
                        cout << "2. Блокировка пользователя\n";
                        cout << "3. Разблокировка пользователя\n";
                        cout << "4. Выход\n";
                        cout << "Выберите действие: ";
                        
                        string adminChoice;
                        getline(cin, adminChoice);
                        if(!isIntCorrect(adminChoice)) {
                            cout << "Неверный ввод, попробуйте снова.\n";
                            continue;
                        }
                        int intAdminChoice = stoi(adminChoice);
                        switch(intAdminChoice) {
                            case 1: {
                                // Вывод списка пользователей
                                cout << "\nСписок пользователей:\n";
                                for(auto &u : ds.getUsers()) {
                                    cout << "Пользователь: " << u.getUsername() 
                                              << " | Заблокирован: " 
                                              << (u.getIsBlocked() ? "Да" : "Нет") 
                                              << "\n";
                                }
                                break;
                            }
                            case 2: {
                                cout << "Введите имя пользователя для блокировки: ";
                                string blockName;
                                getline(cin, blockName);
                                User* userToBlock = ds.findUserByName(blockName);
                                if(userToBlock != nullptr) {
                                    userToBlock->blockUser();
                                    cout << "Пользователь " << blockName << " заблокирован.\n";
                                } else {
                                    cout << "Пользователь не найден.\n";
                                }
                                break;
                            }
                            case 3: {
                                cout << "Введите имя пользователя для разблокировки: ";
                                string unblockName;
                                getline(cin, unblockName);
                                User* userToUnblock = ds.findUserByName(unblockName);
                                if(userToUnblock != nullptr) {
                                    userToUnblock->unblockUser();
                                    cout << "Пользователь " << unblockName << " разблокирован.\n";
                                } else {
                                    cout << "Пользователь не найден.\n";
                                }
                                break;
                            }
                            case 4:
                                adminLogged = false;
                                break;
                            default:
                                cout << "Неверный выбор, попробуйте снова.\n";
                        }
                    }
                }
                else {
                    // Меню обычного пользователя
                    bool userLogged = true;
                    while(userLogged) {
                        cout << "\nМеню пользователя:\n";
                        cout << "1. Просмотр арендованных авто\n";
                        cout << "2. Выход\n";
                        cout << "Выберите действие: ";
                        
                        string userChoice;
                        getline(cin, userChoice);
                        if(!isIntCorrect(userChoice)) {
                            cout << "Неверный ввод, попробуйте снова.\n";
                            continue;
                        }
                        int intUserChoice = stoi(userChoice);
                        switch (intUserChoice) {
                            case 1: {
                                // Пока что выводим статический текст (реализацию аренды можно добавить позже)
                                cout << "\nСписок арендованных авто (данные временные, функционал в разработке).\n";
                                break;
                            }
                            case 2:
                                userLogged = false;
                                break;
                            default:
                                cout << "Неверный выбор, попробуйте снова.\n";
                        }
                    }
                }
                break;
            }
            case 3:
                exitFlag = true;
                break;
            default:
                cout << "Неверный выбор, попробуйте снова.\n";
        }
    }
}
