#include "User.h"
#include "InputValidator.h"

#include <iostream>
#include <string>

User::User() : rating(0), isBlocked(false), isAdmin(false) {}

User::User(const std::string& username, const std::string& password, int rating, bool isBlocked, bool isAdmin)
    : username(username), password(password), rating(rating), isBlocked(isBlocked), isAdmin(isAdmin) {}

void User::registerUser() {
    std::string uname, pass;
    std::cout << "\nРегистрация пользователя:\n";
    std::cout << "Введите имя пользователя: ";
    std::getline(std::cin, uname);
    while(!isStringCorrect(uname)) {
        std::cout << "Неверный ввод. Попробуйте снова: ";
        std::getline(std::cin, uname);
    }
    
    std::cout << "Введите пароль: ";
    std::getline(std::cin, pass);
    while(pass.empty()) { // Можно также использовать isStringCorrect
        std::cout << "Пароль не может быть пустым. Попробуйте снова: ";
        std::getline(std::cin, pass);
    }
    
    username = uname;
    password = pass;
    
    // Если имя "admin", то считаем пользователя администратором
    if(username == "admin") {
        isAdmin = true;
    }
    
    std::cout << "Пользователь " << username << " успешно зарегистрирован!\n";
}

bool User::checkPassword(const std::string& pass) const {
    return password == pass;
}

const std::string& User::getUsername() const {
    return username;
}

bool User::getIsBlocked() const {
    return isBlocked;
}

bool User::getIsAdmin() const {
    return isAdmin;
}

void User::blockUser() {
    isBlocked = true;
}

void User::unblockUser() {
    isBlocked = false;
}
