#pragma once
#include <string>

class User {
public:
    std::string username;
    std::string hashedPassword;
    int age;
    double experience;
    int rating;
    bool isBlocked;
    bool isAdmin;

    User();
    User(const std::string& uname, const std::string& pass, int age, double exp,
        int rating = 0, bool blocked = false, bool isAdmin = false);

    bool checkPassword(const std::string& pass) const;
};

