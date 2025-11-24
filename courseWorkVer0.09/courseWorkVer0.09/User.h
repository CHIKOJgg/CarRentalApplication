#pragma once 
#include <string>

class User {
private:
    int id; 

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

    
    User(int id, const std::string& uname, const std::string& pass, int age, double exp,
        int rating, bool blocked, bool isAdmin);

    bool checkPassword(const std::string& pass) const;

    int getId() const;
    void setId(int newId);
};
