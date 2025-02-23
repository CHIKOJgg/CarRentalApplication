#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::string password;
    int rating;
    bool isBlocked;
    bool isAdmin; // Флаг для администратора

public:
    User();
    User(const std::string& username, const std::string& password, int rating = 0, bool isBlocked = false, bool isAdmin = false);
    
    // Функция регистрации пользователя (с проверкой ввода)
    void registerUser();
    
    // Проверка введённого пароля с сохранённым
    bool checkPassword(const std::string& pass) const;
    
    // Геттеры
    const std::string& getUsername() const;
    bool getIsBlocked() const;
    bool getIsAdmin() const;
    
    // Функции для администрирования
    void blockUser();
    void unblockUser();
};

#endif // USER_H
