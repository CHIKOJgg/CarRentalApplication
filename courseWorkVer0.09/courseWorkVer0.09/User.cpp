#include "User.h"
#include "Utilities.h"

User::User()
    : id(-1), age(0), experience(0.0), rating(0), isBlocked(false), isAdmin(false) {}


User::User(const std::string& uname, const std::string& pass, int age, double exp,
    int rating, bool blocked, bool isAdmin)
    : User(-1, uname, pass, age, exp, rating, blocked, isAdmin) {}

User::User(int id, const std::string& uname, const std::string& pass, int age, double exp,
    int rating, bool blocked, bool isAdmin)
    : id(id),
    username(uname),
    hashedPassword(hashPassword(pass)),
    age(age),
    experience(exp),
    rating(rating),
    isBlocked(blocked),
    isAdmin(isAdmin) {}

bool User::checkPassword(const std::string& pass) const {
    return hashedPassword == hashPassword(pass);
}

int User::getId() const {
    return id;
}

void User::setId(int newId) {
    id = newId;
}
